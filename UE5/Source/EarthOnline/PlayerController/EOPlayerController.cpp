#include "PlayerController/EOPlayerController.h"
#include "HUD/EOHUD.h"

AEOPlayerController::AEOPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AEOPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        ShowGameHUD();
    }
}

void AEOPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &AEOPlayerController::ToggleMenu);
    }
}

void AEOPlayerController::Client_ShowWelcomeMessage_Implementation(const FString& Version)
{
    UE_LOG(LogTemp, Log, TEXT("欢迎来到地球Online！版本: %s"), *Version);

    if (AEOHUD* HUD = Cast<AEOHUD>(GetHUD()))
    {
        HUD->ShowNotification(FString::Printf(TEXT("欢迎来到地球Online内测版 v%s"), *Version));
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
