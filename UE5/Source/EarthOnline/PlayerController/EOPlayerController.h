#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EOPlayerController.generated.h"

class UEOBuilderComponent;

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

    // 建造组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC")
    UEOBuilderComponent* BuilderComponent;

    // 切换编辑模式
    UFUNCTION(BlueprintCallable, Category = "UGC")
    void ToggleEditMode();

    // 是否在编辑模式
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool IsInEditMode() const;

protected:
    // 输入动作
    UFUNCTION()
    void ToggleMenu();

    UFUNCTION()
    void OnBuildAction();

    UFUNCTION()
    void OnDeleteSelected();

    UFUNCTION()
    void OnCycleBuildMode();

    // F 键 / 手柄 B 键交互
    UFUNCTION()
    void OnInteractKey();

    // `~` 键 锁定/释放鼠标（仿悟空：点按切换视角控制与鼠标操作）
    UFUNCTION()
    void OnToggleMouseLock();
};
