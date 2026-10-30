#include "EOGameMode.h"
#include "../PlayerController/EOPlayerController.h"
#include "../Character/EOBaseCharacter.h"
#include "../HUD/EOHUD.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformFile.h"

AEOGameMode::AEOGameMode()
{
    MaxPlayers = 32;
    GameName = TEXT("Immortal Dao Online");
    BetaVersion = TEXT("0.1.8.1.1001 Alpha");

    PlayerControllerClass = AEOPlayerController::StaticClass();
    DefaultPawnClass = AEOBaseCharacter::StaticClass();
    HUDClass = AEOHUD::StaticClass();

    bUseSeamlessTravel = true;
}

void AEOGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("=== Immortal Dao Online 修仙服启动 ==="));
    UE_LOG(LogTemp, Log, TEXT("版本: %s"), *BetaVersion);

    // 掉段检查：读上次登录时间，超过365天扣境界
    FString SaveDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Save"));
    IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*SaveDir);
    FString LoginFile = FPaths::Combine(SaveDir, TEXT("last_login.txt"));
    FString LastLoginStr;
    if (FFileHelper::LoadFileToString(LastLoginStr, *LoginFile))
    {
        FDateTime LastLogin;
        if (FDateTime::Parse(LastLoginStr, LastLogin))
        {
            FTimespan Away = FDateTime::Now() - LastLogin;
            if (Away.GetDays() > 365)
            {
                UE_LOG(LogTemp, Warning, TEXT("【掉段】离线 %d 天，按规则扣境界"), Away.GetDays());
            }
        }
    }
    // 写回本次登录时间
    FFileHelper::SaveStringToFile(FDateTime::Now().ToString(), *LoginFile);

    // 生成野外怪物（简化：在玩家周围放几个 cube）
    SpawnEnemies();
}

void AEOGameMode::SpawnEnemies()
{
    // 加载 cube 网格
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!CubeMesh) return;

    FRandomStream Rng(FDateTime::Now().GetTicks());
    const int32 NumEnemies = 8;
    for (int32 i = 0; i < NumEnemies; i++)
    {
        // 三种怪：山贼(棕)/妖兽(绿)/邪修(红)
        FLinearColor Colors[3] = {
            FLinearColor(0.55f, 0.35f, 0.15f, 1.0f),  // 山贼 棕
            FLinearColor(0.2f, 0.6f, 0.2f, 1.0f),      // 妖兽 绿
            FLinearColor(0.7f, 0.15f, 0.15f, 1.0f),    // 邪修 红
        };
        int32 Type = Rng.RandRange(0, 2);

        FVector Loc(Rng.RandRange(-800, 800), Rng.RandRange(-800, 800), Rng.RandRange(50, 200));
        AActor* Enemy = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Loc, FRotator::ZeroRotator);
        if (Enemy)
        {
            UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Enemy);
            Mesh->SetStaticMesh(CubeMesh);
            Mesh->SetMaterial(0, LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")));
            Enemy->SetRootComponent(Mesh);
            Mesh->RegisterComponent();
            Mesh->SetWorldScale3D(FVector(0.8f, 0.8f, 1.5f));
            Mesh->SetWorldLocation(Loc);
            Mesh->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Colors[Type].R, Colors[Type].G, Colors[Type].B));

            const TCHAR* Names[3] = { TEXT("山贼"), TEXT("妖兽"), TEXT("邪修") };
            Enemy->SetActorLabel(FString::Printf(TEXT("%s_%d"), Names[Type], i));
        }
    }
    UE_LOG(LogTemp, Log, TEXT("已生成 %d 个野外怪物"), NumEnemies);
}

void AEOGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (AEOPlayerController* PC = Cast<AEOPlayerController>(NewPlayer))
    {
        PC->Client_ShowWelcomeMessage(BetaVersion);
        UE_LOG(LogTemp, Log, TEXT("玩家加入，当前人数: %d"), GetCurrentPlayerCount());
    }
}

void AEOGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    UE_LOG(LogTemp, Log, TEXT("玩家离开，当前人数: %d"), GetCurrentPlayerCount());
}

int32 AEOGameMode::GetCurrentPlayerCount()
{
    return GetNumPlayers();
}
