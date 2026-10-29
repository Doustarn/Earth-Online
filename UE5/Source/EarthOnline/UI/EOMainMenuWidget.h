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
 * 开始界面：Immortal Dao Online 标题 + 语言选择 + 开始按钮
 * 点击开始游戏后切换到登录/加载界面（由 AEOHUD 串联）
 */
UCLASS()
class EARTHONLINE_API UEOMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void ProceedToLogin();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGame);

    UPROPERTY(BlueprintAssignable)
    FOnStartGame OnStartGame;

protected:
    UCanvasPanel* MainCanvas;

    UButton* StartButton;
    UTextBlock* TapAnywhereText;
    UTextBlock* ServerInfoText;
    UTextBlock* FlavorText;
    UTextBlock* CopyrightText;
    UTextBlock* AgeRatingText;

    void BuildUI();
    void BuildBackground();
    void BuildTitle();
    void BuildCenterButtons();
    void BuildTopBar();
    void BuildBottomBar();

    UTexture2D* CreateStarTexture();
    void FillStarPixels(TArray<FColor>& Pixels, int32 W, int32 H);

    UButton* MakeStyledButton(const FString& Text, float FontSize, const FLinearColor& Bg, const FLinearColor& Fg);
    UButton* MakeBlackMythButton(const FString& Text, bool bPrimary);
    UTextBlock* MakeText(const FString& Text, float Size, const FLinearColor& Color);

    UFUNCTION()
    void OnStartClicked();

    UFUNCTION()
    void OnIconClicked();
};
