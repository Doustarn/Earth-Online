#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UVerticalBox;
class UCanvasPanel;
class UCanvasPanelSlot;
class UButton;
class UImage;
class UBorder;
class UEOMobilePhoneWidget;
class AActor;

UCLASS()
class EARTHONLINE_API UEOHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 显示通知
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotification(const FString& Message);

    // —— 交互 / 教程 / 警报 ——
    void ShowInteractionPrompt(const FString& Label);   // 显示交互提示（如 "F 开门"）
    void HideInteractionPrompt();
    void ShowAlert(const FString& Title, const FString& Body, const FString& Btn1, const FString& Btn2);
    void HideAlert();
    void HideTutorial();
    void PerformInteraction();    // F 键交互

protected:
    // —— 动态主界面构建 ——
    void BuildMainHUD();
    void BuildActionArea(UCanvasPanel* Canvas);   // 右下角：移动/跳跃/动作键
    void BuildStatusArea(UCanvasPanel* Canvas);   // 左下角：身体状态（无血条）
    void BuildMiniMap(UCanvasPanel* Canvas);      // 左上角：小地图 + 设置
    void BuildTopRightBar(UCanvasPanel* Canvas);  // 右上角：功能图标排
    void BuildHotbar(UCanvasPanel* Canvas);       // 中下：物品栏
    void BuildPhoneEntry(UCanvasPanel* Canvas);   // 手机入口
    void BuildInteractionArea(UCanvasPanel* Canvas); // 中央交互提示
    void BuildTutorial(UCanvasPanel* Canvas);     // 新手教程
    void BuildAlert(UCanvasPanel* Canvas);        // 警报弹窗
    void UpdateHUD(float DeltaTime);
    void UpdateJoystick(float DeltaTime);
    void UpdateInteraction(float DeltaTime);      // 检测对准交互物

    // 控件工厂
    UButton* MakeButton(const FString& Content, float FontSize);
    UTextBlock* MakeText(const FString& Content, float FontSize, const FLinearColor& Color);

    // 程序化圆角纹理 / 圆角按钮样式
    class UTexture2D* CreateRoundedTexture(int32 Size, float CornerRadius, const FLinearColor& Color);
    void StyleRoundedButton(UButton* Btn, const FLinearColor& Color, float Radius = 9.0f);

    UFUNCTION()
    void OnJoystickPressed();

    UFUNCTION()
    void OnJoystickReleased();

    UFUNCTION()
    void OnJumpClicked();

    UFUNCTION()
    void OnSprintClicked();

    UFUNCTION()
    void OnCrouchClicked();

    UFUNCTION()
    void OnInteractClicked();

    UFUNCTION()
    void OnPhoneClicked();

    UFUNCTION()
    void OnSlotClicked();

    UFUNCTION()
    void OnTutorialCloseClicked();

    UFUNCTION()
    void OnAlertBtn1Clicked();

    UFUNCTION()
    void OnAlertBtn2Clicked();

    // —— 动态UI控件 ——
    UPROPERTY()
    UCanvasPanel* MainCanvas;

    // 交互提示区
    UPROPERTY()
    UTextBlock* InteractPromptText;

    // 新手教程
    UPROPERTY()
    UBorder* TutorialPanel;

    UPROPERTY()
    UButton* TutorialCloseButton;

    // 警报弹窗
    UPROPERTY()
    UBorder* AlertPanel;

    UPROPERTY()
    UTextBlock* AlertTitleText;

    UPROPERTY()
    UTextBlock* AlertBodyText;

    UPROPERTY()
    UButton* AlertBtn1;

    UPROPERTY()
    UButton* AlertBtn2;

    UPROPERTY()
    AActor* CurrentInteractTarget;

    bool bTutorialShown;
    bool bAlertActive;
    FTimerHandle AlertDemoTimer;

    // 动作区
    UPROPERTY()
    UButton* JoystickButton;

    UPROPERTY()
    UImage* JoystickThumb;

    UPROPERTY()
    UImage* MiniMapPlayerDot;

    UPROPERTY()
    UCanvasPanelSlot* JoystickThumbSlot;

    UPROPERTY()
    UButton* JumpButton;

    UPROPERTY()
    UButton* SprintButton;

    UPROPERTY()
    UButton* CrouchButton;

    UPROPERTY()
    UButton* InteractButton;

    UPROPERTY()
    UButton* PhoneButton;

    // 状态区
    UPROPERTY()
    UTextBlock* HealthValueText;

    UPROPERTY()
    UTextBlock* StaminaValueText;

    UPROPERTY()
    UTextBlock* LevelValueText;

    UPROPERTY()
    UTextBlock* PosValueText;

    // 物品栏
    UPROPERTY()
    TArray<UButton*> HotbarButtons;

    UPROPERTY()
    TArray<UTextBlock*> HotbarLabelTexts;

    // 手机
    UPROPERTY()
    UEOMobilePhoneWidget* PhoneWidget;

    // 摇杆状态
    bool bJoystickPressed;
    FVector2D JoystickPressPos;
    int32 SelectedSlot;
    bool bSprinting;

    // 物品栏物品名
    TArray<FString> HotbarItems = { TEXT("草"), TEXT("土"), TEXT("石"), TEXT("木"), TEXT("板"), TEXT("台"), TEXT("炉"), TEXT("炬"), TEXT("麦") };

    // —— 兼容旧 UMG 资产的绑定控件（保留原有逻辑）——
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerNameText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PositionText;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* NotificationList;

    // 通知自动消失时间
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    float NotificationDuration;

    TArray<FString> PendingNotifications;
};
