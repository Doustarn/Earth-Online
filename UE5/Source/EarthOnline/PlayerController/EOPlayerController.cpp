#include "EOPlayerController.h"
#include "../HUD/EOHUD.h"
#include "../UI/EOHUDWidget.h"
#include "../UGC/EOBuilderComponent.h"
#include "../UGC/EOUGCManager.h"
#include "InputCoreTypes.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "EngineUtils.h"

AEOPlayerController::AEOPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    BuilderComponent = CreateDefaultSubobject<UEOBuilderComponent>(TEXT("BuilderComponent"));
}

void AEOPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("[EOPlayerController] BeginPlay，IsLocalController=%d"), IsLocalController() ? 1 : 0);

    // 场景若无定向光，添加一个暖光定向光，避免地面一片黑
    UWorld* World = GetWorld();
    if (World)
    {
        bool bHasDirectionalLight = false;
        for (TActorIterator<ADirectionalLight> It(World); It; ++It)
        {
            bHasDirectionalLight = true;
            break;
        }
        if (!bHasDirectionalLight)
        {
            ADirectionalLight* Light = World->SpawnActor<ADirectionalLight>(
                ADirectionalLight::StaticClass(),
                FVector(0.0f, 0.0f, 1200.0f),
                FRotator(-55.0f, -20.0f, 0.0f));
            if (Light)
            {
                Light->SetMobility(EComponentMobility::Movable);
                Light->GetLightComponent()->SetIntensity(8.0f);
                Light->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.85f));
                UE_LOG(LogTemp, Warning, TEXT("[EOPlayerController] 场景无定向光，已添加暖光定向光提亮地面"));
            }
        }

        // 补一个天光（SkyLight）作环境光，避免地平线以下/背光面一片黑
        bool bHasSkyLight = false;
        for (TActorIterator<ASkyLight> It(World); It; ++It)
        {
            bHasSkyLight = true;
            break;
        }
        if (!bHasSkyLight)
        {
            ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
            if (Sky)
            {
                Sky->GetLightComponent()->Mobility = EComponentMobility::Movable;
                Sky->GetLightComponent()->SetIntensity(2.5f);
                Sky->GetLightComponent()->SetLightColor(FLinearColor(0.72f, 0.82f, 1.0f, 1.0f));
                UE_LOG(LogTemp, Warning, TEXT("[EOPlayerController] 已添加天光（SkyLight）环境光，修复下半黑"));
            }
        }
    }

    // 注意：不要在这里 ShowGameHUD —— 开始界面流程由 AEOHUD::BeginPlay 统一管理（开始→登录→加载→游戏）
}

void AEOPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &AEOPlayerController::ToggleMenu);
        InputComponent->BindAction("EditMode", IE_Pressed, this, &AEOPlayerController::ToggleEditMode);
        InputComponent->BindAction("BuildAction", IE_Pressed, this, &AEOPlayerController::OnBuildAction);
        InputComponent->BindAction("DeleteSelected", IE_Pressed, this, &AEOPlayerController::OnDeleteSelected);
        InputComponent->BindAction("CycleBuildMode", IE_Pressed, this, &AEOPlayerController::OnCycleBuildMode);

        // —— 交互键（F 键 + 手柄 B 键）——
        InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AEOPlayerController::OnInteractKey);
        InputComponent->BindKey(EKeys::Gamepad_FaceButton_Right, IE_Pressed, this, &AEOPlayerController::OnInteractKey);

        // —— `~` 键 锁定/释放鼠标（仿悟空）——
        InputComponent->BindKey(EKeys::Tilde, IE_Pressed, this, &AEOPlayerController::OnToggleMouseLock);
    }
}

void AEOPlayerController::OnToggleMouseLock()
{
    // 切换鼠标：锁定（隐藏光标、视角控制）↔ 释放（显示光标、可点 UI）
    bShowMouseCursor = !bShowMouseCursor;

    if (bShowMouseCursor)
    {
        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        SetInputMode(Mode);
        UE_LOG(LogTemp, Warning, TEXT("[EOPlayerController] `~` 释放鼠标（可操作界面）"));
    }
    else
    {
        FInputModeGameOnly Mode;
        SetInputMode(Mode);
        UE_LOG(LogTemp, Warning, TEXT("[EOPlayerController] `~` 锁定鼠标（视角控制）"));
    }
}

void AEOPlayerController::Client_ShowWelcomeMessage_Implementation(const FString& Version)
{
    UE_LOG(LogTemp, Log, TEXT("欢迎来到Immortal Dao Online！版本: %s"), *Version);

    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        HUD->ShowNotification(FString::Printf(TEXT("欢迎来到Immortal Dao Online内测版 v%s"), *Version));
    }
}

void AEOPlayerController::ShowMainMenu()
{
    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        HUD->ShowMainMenu();
    }
}

void AEOPlayerController::ShowGameHUD()
{
    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        HUD->ShowGameHUD();
    }
}

void AEOPlayerController::ToggleMenu()
{
    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        HUD->ToggleMenu();
    }
}

void AEOPlayerController::OnInteractKey()
{
    // F 键/手柄B键交互：调用 HUD 的交互逻辑
    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        if (HUD->GetGameHUD())
        {
            HUD->GetGameHUD()->PerformInteraction();
        }
    }
}

void AEOPlayerController::ToggleEditMode()
{
    if (!BuilderComponent) return;

    if (BuilderComponent->IsBuilding())
    {
        BuilderComponent->SetBuildMode(EEOBuildMode::None);
        bShowMouseCursor = true;
        UE_LOG(LogTemp, Log, TEXT("[UGC] 退出编辑模式"));
    }
    else
    {
        BuilderComponent->SetBuildMode(EEOBuildMode::Place);
        bShowMouseCursor = true;
        UE_LOG(LogTemp, Log, TEXT("[UGC] 进入编辑模式 - 按E放置，Q切换模式，Delete删除"));
    }
}

bool AEOPlayerController::IsInEditMode() const
{
    return BuilderComponent && BuilderComponent->IsBuilding();
}

void AEOPlayerController::OnBuildAction()
{
    if (!BuilderComponent || !BuilderComponent->IsBuilding()) return;

    if (BuilderComponent->GetBuildMode() == EEOBuildMode::Place)
    {
        BuilderComponent->ConfirmPlacement();
    }
}

void AEOPlayerController::OnDeleteSelected()
{
    if (BuilderComponent && BuilderComponent->IsBuilding())
    {
        BuilderComponent->DeleteSelected();
    }
}

void AEOPlayerController::OnCycleBuildMode()
{
    if (!BuilderComponent) return;

    EEOBuildMode Current = BuilderComponent->GetBuildMode();
    EEOBuildMode Next = EEOBuildMode::None;

    switch (Current)
    {
        case EEOBuildMode::None: Next = EEOBuildMode::Place; break;
        case EEOBuildMode::Place: Next = EEOBuildMode::Move; break;
        case EEOBuildMode::Move: Next = EEOBuildMode::Rotate; break;
        case EEOBuildMode::Rotate: Next = EEOBuildMode::Scale; break;
        case EEOBuildMode::Scale: Next = EEOBuildMode::Delete; break;
        case EEOBuildMode::Delete: Next = EEOBuildMode::Place; break;
    }

    BuilderComponent->SetBuildMode(Next);
}
