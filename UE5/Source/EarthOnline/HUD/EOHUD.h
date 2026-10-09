#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EOHUD.generated.h"

class UEOMainMenuWidget;
class UEOHUDWidget;

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

protected:
    // 主菜单Widget类
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEOMainMenuWidget> MainMenuWidgetClass;

    // 游戏HUD Widget类
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEOHUDWidget> GameHUDWidgetClass;

    // 主菜单实例
    UPROPERTY()
    UEOMainMenuWidget* MainMenuWidget;

    // 游戏HUD实例
    UPROPERTY()
    UEOHUDWidget* GameHUDWidget;

    bool bMenuVisible;
};
