#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOKeyInputWidget.generated.h"

class UCanvasPanel;
class UImage;
class UButton;
class UTextBlock;
class UBorder;
class UEditableTextBox;
class UTexture2D;

/**
 * 登录 / 加载界面：非黑背景，中央转动地球圆球，
 * 4 段加载动画（人物生成中→场景生成中→海洋生成中→加载中）连在一起，段间闪黑切换。
 * 保留密钥验证：验证成功后才开始 4 段加载，加载完成进入游戏 HUD。
 */
UCLASS()
class EARTHONLINE_API UEOKeyInputWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 开始 4 段加载动画（验证通过后调用）
    void BeginLoading();

    // 强制直接完成加载（用于已激活情况）
    void CompleteImmediately();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingComplete);
    UPROPERTY(BlueprintAssignable)
    FOnLoadingComplete OnLoadingComplete;

protected:
    UCanvasPanel* MainCanvas;
    UImage* EarthImage;          // 转动地球
    UTexture2D* GeneratedEarth;
    float EarthRotation;

    UTextBlock* StageText;       // 当前加载阶段文字
    UImage* FlashImage;          // 闪黑遮罩
    UTextBlock* SubText;         // "正在进入地球Online..."

    // 密钥验证区
    UBorder* KeyPanel;
    UEditableTextBox* KeyInput;
    UButton* VerifyButton;
    UTextBlock* StatusText;

    int32 CurrentStage;
    bool bLoading;
    FTimerHandle StageTimer;
    FTimerHandle FlashTimer;
    FTimerHandle FlashHideTimer;

    void BuildUI();
    void BuildBackground();
    void BuildEarth();
    void BuildStageText();
    void BuildKeyVerify();

    void SetStatus(const FString& Message, FColor Color);

    UTexture2D* CreateEarthTexture();
    void FillEarthPixels(TArray<FColor>& Pixels, int32 W, int32 H);

    UButton* MakeStyledButton(const FString& Text, float FontSize, const FLinearColor& Bg, const FLinearColor& Fg);
    UTextBlock* MakeText(const FString& Text, float Size, const FLinearColor& Color);

    void AdvanceStage();
    void ShowFlash();
    void FinishLoading();
    void SetStageText(const FString& Text);

    UFUNCTION()
    void OnVerifyClicked();
    UFUNCTION()
    void OnKeyTextChanged(const FText& Text);
};
