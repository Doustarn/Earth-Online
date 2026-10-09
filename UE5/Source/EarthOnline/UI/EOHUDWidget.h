#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UVerticalBox;

UCLASS()
class EARTHONLINE_API UEOHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 显示通知
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotification(const FString& Message);

protected:
    // 玩家名称
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerNameText;

    // 生命值条
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    // 体力条
    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaBar;

    // 坐标显示
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PositionText;

    // 通知列表
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* NotificationList;

    // 通知自动消失时间
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    float NotificationDuration;

    TArray<FString> PendingNotifications;
};
