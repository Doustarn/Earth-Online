#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOMainMenuWidget.generated.h"

class UCanvasPanel;
class UImage;
class UButton;
class UTextBlock;
class UBorder;
class UTexture2D;

/**
 * 开始界面：真实地球转动背景 + 开始游戏 + 服务器信息 + 游戏文案 + 功能图标
 * 点击开始游戏后切换到登录/加载界面（由 EOHUD 串联）
 */
UCLASS()
class EARTHONLINE_API UEOMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 从开始界面进入登录/加载界面（由 AEOHUD 调用）
    void ProceedToLogin();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGame);

    UPROPERTY(BlueprintAssignable)
    FOnStartGame OnStartGame;

protected:
    UCanvasPanel* MainCanvas;
    UImage* EarthImage;          // 转动的地球（代码生成贴图）
    float EarthRotation;
    UTexture2D* GeneratedEarth;  // 地球贴图

    UButton* StartButton;        // 开始游戏
    UTextBlock* TapAnywhereText;
    UTextBlock* ServerInfoText;
    UTextBlock* FlavorText;      // 底部游戏介绍
    UTextBlock* CopyrightText;
    UTextBlock* AgeRatingText;   // 适龄提示

    void BuildUI();
    void BuildBackground();
    void BuildEarth();
    void BuildCenterButtons();
    void BuildTopBar();
    void BuildBottomBar();

    UTexture2D* CreateEarthTexture();           // 程序化生成地球贴图
    void FillEarthPixels(TArray<FColor>& Pixels, int32 W, int32 H);

    UTexture2D* CreateStarTexture();            // 程序化生成星空背景贴图
    void FillStarPixels(TArray<FColor>& Pixels, int32 W, int32 H);

    // 辅助：生成带文字/图标的圆角按钮
    UButton* MakeStyledButton(const FString& Text, float FontSize, const FLinearColor& Bg, const FLinearColor& Fg);
    UButton* MakeBlackMythButton(const FString& Text, bool bPrimary);
    UTextBlock* MakeText(const FString& Text, float Size, const FLinearColor& Color);

    UFUNCTION()
    void OnStartClicked();

    UFUNCTION()
    void OnIconClicked();   // 右上角功能图标（设置/音量/语音/备忘）点击反应
};
