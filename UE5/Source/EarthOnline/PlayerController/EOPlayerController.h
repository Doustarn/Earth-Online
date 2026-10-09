#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EOPlayerController.generated.h"

UCLASS()
class EARTHONLINE_API AEOPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEOPlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // 客户端显示欢迎消息
    UFUNCTION(Client, Reliable)
    void Client_ShowWelcomeMessage(const FString& Version);

    // 显示主菜单
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu();

    // 显示游戏HUD
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameHUD();

protected:
    // 输入动作
    UFUNCTION()
    void ToggleMenu();
};
