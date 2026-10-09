#include "HUD/EOHUD.h"
#include "UI/EOMainMenuWidget.h"
#include "UI/EOHUDWidget.h"
#include "UI/EOKeyInputWidget.h"
#include "Security/EOKeyManager.h"

AEOHUD::AEOHUD()
{
    bMenuVisible = false;
}

void AEOHUD::BeginPlay()
{
    Super::BeginPlay();

    // 创建游戏HUD（默认隐藏）
    if (GameHUDWidgetClass)
    {
        GameHUDWidget = CreateWidget<UEOHUDWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            GameHUDWidget->AddToViewport();
            GameHUDWidget->SetVisibility(ESlateVisibility::Hidden);
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

    // 启动时显示密钥验证
    ShowKeyInput();
}

void AEOHUD::ShowKeyInput()
{
    if (KeyInputWidgetClass && !KeyInputWidget)
    {
        KeyInputWidget = CreateWidget<UEOKeyInputWidget>(GetWorld(), KeyInputWidgetClass);
    }

    if (KeyInputWidget)
    {
        KeyInputWidget->AddToViewport();
        KeyInputWidget->SetVisibility(ESlateVisibility::Visible);

        if (APlayerController* PC = GetOwningPlayerController())
        {
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
        }
    }
    else
    {
        // 如果没有设置Widget类，直接跳过验证（编辑器开发模式）
        ShowMainMenu();
    }
}

void AEOHUD::ShowMainMenu()
{
    if (KeyInputWidget)
    {
        KeyInputWidget->SetVisibility(ESlateVisibility::Hidden);
    }

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
    if (KeyInputWidget)
    {
        KeyInputWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (MainMenuWidget)
    {
        MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        bMenuVisible = false;
    }

    if (GameHUDWidget)
    {
        GameHUDWidget->SetVisibility(ESlateVisibility::Visible);
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
