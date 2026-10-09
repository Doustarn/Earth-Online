#include "HUD/EOHUD.h"
#include "UI/EOMainMenuWidget.h"
#include "UI/EOHUDWidget.h"

AEOHUD::AEOHUD()
{
    bMenuVisible = false;
}

void AEOHUD::BeginPlay()
{
    Super::BeginPlay();

    // 创建游戏HUD
    if (GameHUDWidgetClass)
    {
        GameHUDWidget = CreateWidget<UEOHUDWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            GameHUDWidget->AddToViewport();
        }
    }

    // 创建主菜单（默认隐藏）
    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UEOMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AEOHUD::ShowMainMenu()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
        bMenuVisible = true;

        if (APlayerController* PC = GetOwningPlayerController())
        {
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
        }
    }
}

void AEOHUD::ShowGameHUD()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        bMenuVisible = false;
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void AEOHUD::ToggleMenu()
{
    if (bMenuVisible)
    {
        ShowGameHUD();
    }
    else
    {
        ShowMainMenu();
    }
}

void AEOHUD::ShowNotification(const FString& Message)
{
    if (GameHUDWidget)
    {
        GameHUDWidget->ShowNotification(Message);
    }
    UE_LOG(LogTemp, Log, TEXT("[通知] %s"), *Message);
}
