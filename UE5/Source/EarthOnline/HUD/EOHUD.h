#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EOHUD.generated.h"

class UEOMainMenuWidget;
class UEOHUDWidget;
class UEOKeyInputWidget;

UCLASS()
class EARTHONLINE_API AEOHUD : public AHUD
{
    GENERATED_BODY()

public:
    AEOHUD();

    virtual void BeginPlay() override;

    // 显示主菜单
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu();

    // 显示游戏HUD
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameHUD();

    // 切换菜单
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleMenu();

    // 显示通知
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowNotification(const FString& Message);

    // 显示密钥输入
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowKeyInput();

    // 获取游戏 HUD Widget（供交互调用）
    UFUNCTION(BlueprintCallable, Category = "UI")
    UEOHUDWidget* GetGameHUD() const { return GameHUDWidget; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEOMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEOHUDWidget> GameHUDWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEOKeyInputWidget> KeyInputWidgetClass;

    UPROPERTY()
    UEOMainMenuWidget* MainMenuWidget;

    UPROPERTY()
    UEOHUDWidget* GameHUDWidget;

    UPROPERTY()
    UEOKeyInputWidget* KeyInputWidget;

    // 自动截图定时器（验收验证用）
    FTimerHandle ScreenshotTimer;

    bool bMenuVisible;
};
