// 地球Online - 玩家建造组件实现
#include "EOBuilderComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EOUGCManager.h"

UEOBuilderComponent::UEOBuilderComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEOBuilderComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UEOBuilderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (BuildMode == EEOBuildMode::Place && PreviewActor)
    {
        UpdatePreview();
    }
}

void UEOBuilderComponent::SetBuildMode(EEOBuildMode NewMode)
{
    if (BuildMode == NewMode) return;

    CancelPlacement();
    BuildMode = NewMode;

    if (NewMode == EEOBuildMode::Place && PlaceableClass)
    {
        StartPlacement();
    }

    UE_LOG(LogTemp, Log, TEXT("[Builder] 建造模式: %d"), (int32)NewMode);
}

void UEOBuilderComponent::SetPlaceableClass(TSubclassOf<AActor> ActorClass)
{
    PlaceableClass = ActorClass;
    if (BuildMode == EEOBuildMode::Place)
    {
        CancelPlacement();
        StartPlacement();
    }
}

void UEOBuilderComponent::StartPlacement()
{
    if (!PlaceableClass || !GetWorld()) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    PreviewActor = GetWorld()->SpawnActor<AActor>(PlaceableClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (PreviewActor)
    {
        PreviewActor->SetActorEnableCollision(false);
        UpdatePreview();
    }
}

void UEOBuilderComponent::ConfirmPlacement()
{
    if (!PreviewActor || !GetWorld()) return;

    FVector Location = PreviewActor->GetActorLocation();
    FRotator Rotation = PreviewActor->GetActorRotation();
    FVector Scale = PreviewActor->GetActorScale3D();

    DestroyPreview();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(PlaceableClass, Location, Rotation, Params);
    if (NewActor)
    {
        NewActor->SetActorScale3D(Scale);
        SelectActor(NewActor);
        UE_LOG(LogTemp, Log, TEXT("[Builder] 放置物体: %s"), *NewActor->GetName());
    }
}

void UEOBuilderComponent::CancelPlacement()
{
    DestroyPreview();
}

void UEOBuilderComponent::SelectActor(AActor* Actor)
{
    if (SelectedActor)
    {
        SelectedActor->SetActorHiddenInGame(false);
    }

    SelectedActor = Actor;

    if (SelectedActor)
    {
        UE_LOG(LogTemp, Log, TEXT("[Builder] 选中: %s"), *SelectedActor->GetName());
    }
}

void UEOBuilderComponent::DeselectActor()
{
    SelectedActor = nullptr;
}

void UEOBuilderComponent::DeleteSelected()
{
    if (SelectedActor && GetWorld())
    {
        FString Name = SelectedActor->GetName();
        GetWorld()->DestroyActor(SelectedActor);
        SelectedActor = nullptr;
        UE_LOG(LogTemp, Log, TEXT("[Builder] 删除: %s"), *Name);
    }
}

void UEOBuilderComponent::DuplicateSelected()
{
    if (!SelectedActor || !GetWorld()) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FVector Location = SelectedActor->GetActorLocation() + FVector(100, 0, 0);
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(SelectedActor->GetClass(), Location, SelectedActor->GetActorRotation(), Params);
    if (NewActor)
    {
        NewActor->SetActorScale3D(SelectedActor->GetActorScale3D());
        SelectActor(NewActor);
    }
}

void UEOBuilderComponent::SetGridSnap(bool bEnable, float InGridSize)
{
    bGridSnap = bEnable;
    GridSize = InGridSize;
}

void UEOBuilderComponent::SetRotationSnap(bool bEnable, float InRotationStep)
{
    bRotationSnap = bEnable;
    RotationStep = InRotationStep;
}

bool UEOBuilderComponent::SaveCurrentMap(const FString& MapName)
{
    UEOUGCManager* UGCManager = GetWorld()->GetGameInstance()->GetSubsystem<UEOUGCManager>();
    if (!UGCManager || !GetWorld()) return false;

    FUGCMapData MapData;
    MapData.MapName = MapName;
    MapData.Author = TEXT("Player");
    MapData.ModifiedTime = FDateTime::UtcNow();

    // 收集场景中所有可编辑物体
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    for (AActor* Actor : AllActors)
    {
        if (!Actor || Actor->IsA(APawn::StaticClass())) continue;

        FUGCObjectData ObjData;
        ObjData.ObjectClassPath = Actor->GetClass()->GetPathName();
        ObjData.Location = Actor->GetActorLocation();
        ObjData.Rotation = Actor->GetActorRotation();
        ObjData.Scale = Actor->GetActorScale3D();
        MapData.Objects.Add(ObjData);
    }

    return UGCManager->SaveMap(MapData, MapName);
}

bool UEOBuilderComponent::LoadMap(const FString& MapName)
{
    UEOUGCManager* UGCManager = GetWorld()->GetGameInstance()->GetSubsystem<UEOUGCManager>();
    if (!UGCManager || !GetWorld()) return false;

    FUGCMapData MapData;
    if (!UGCManager->LoadMap(MapName, MapData)) return false;

    // 清除现有物体（保留玩家）
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && !Actor->IsA(APawn::StaticClass()))
        {
            GetWorld()->DestroyActor(Actor);
        }
    }

    // 生成保存的物体
    for (const FUGCObjectData& ObjData : MapData.Objects)
    {
        UClass* ActorClass = LoadObject<UClass>(nullptr, *ObjData.ObjectClassPath);
        if (ActorClass)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, ObjData.Location, ObjData.Rotation, Params);
            if (NewActor)
            {
                NewActor->SetActorScale3D(ObjData.Scale);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[Builder] 加载地图: %s (%d个物体)"), *MapName, MapData.Objects.Num());
    return true;
}

FVector UEOBuilderComponent::GetPlacementLocation() const
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return FVector::ZeroVector;

    FHitResult Hit;
    if (PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        return SnapToGrid(Hit.ImpactPoint);
    }
    return FVector::ZeroVector;
}

FVector UEOBuilderComponent::SnapToGrid(const FVector& Location) const
{
    if (!bGridSnap) return Location;
    return FVector(
        FMath::GridSnap(Location.X, GridSize),
        FMath::GridSnap(Location.Y, GridSize),
        Location.Z
    );
}

void UEOBuilderComponent::UpdatePreview()
{
    if (!PreviewActor) return;
    PreviewActor->SetActorLocation(GetPlacementLocation());
}

void UEOBuilderComponent::DestroyPreview()
{
    if (PreviewActor && GetWorld())
    {
        GetWorld()->DestroyActor(PreviewActor);
        PreviewActor = nullptr;
    }
}
