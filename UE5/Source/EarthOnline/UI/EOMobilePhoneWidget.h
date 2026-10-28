// 地球Online - 手机虚拟机（VM）
// 玩家随身携带的虚拟手机：主屏应用网格 + 设置中心
// 所有游戏设置（操作/画面/音量）均在手机"设置"中完成
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOMobilePhoneWidget.generated.h"

class UCanvasPanel;
class UBorder;
class UVerticalBox;
class UHorizontalBox;
class UGridPanel;
class UButton;
class UTextBlock;
class USlider;

UCLASS()
class EARTHONLINE_API UEOMobilePhoneWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 打开 / 关闭手机
    void OpenPhone();
    void ClosePhone();
    bool IsPhoneOpen() const { return bPhoneOpen; }

    // 对外提供设置值（HUD/输入层读取）
    float GetLookSensitivity() const { return LookSensitivity; }
    int32 GetResolutionIndex() const { return ResolutionIndex; }
    int32 GetQualityIndex() const { return QualityIndex; }
    float GetMasterVolume() const { return MasterVolume; }
    float GetMusicVolume() const { return MusicVolume; }
    float GetSfxVolume() const { return SfxVolume; }

    // 应用设置
    void ApplySettings();

protected:
    void BuildPhoneUI();
    void ShowHomeScreen();
    void ShowSettingsScreen();
    void ShowSettingPage(int32 Page);

    UButton* MakeAppButton(const FString& IconText, const FString& Name);
    UButton* MakeTextButton(const FString& Content, float FontSize);
    UTextBlock* MakeText(const FString& Content, float FontSize, const FLinearColor& Color);
    USlider* MakeSliderRow(const FString& Label, float MinV, float MaxV, float DefaultV);

    // 应用/导航回调
    UFUNCTION()
    void HandleAppSettings();

    UFUNCTION()
    void HandleAppInventory();

    UFUNCTION()
    void HandleAppMap();

    UFUNCTION()
    void HandleAppQuest();

    UFUNCTION()
    void HandleAppShop();

    UFUNCTION()
    void HandleAppAbout();

    UFUNCTION()
    void HandleNavHome();

    UFUNCTION()
    void HandleNavClose();

    UFUNCTION()
    void HandleTabOperation();

    UFUNCTION()
    void HandleTabGraphics();

    UFUNCTION()
    void HandleTabAudio();

    UFUNCTION()
    void HandleRes0();

    UFUNCTION()
    void HandleRes1();

    UFUNCTION()
    void HandleRes2();

    UFUNCTION()
    void HandleQuality0();

    UFUNCTION()
    void HandleQuality1();

    UFUNCTION()
    void HandleQuality2();

    // 滑块回调
    UFUNCTION()
    void HandleSensitivityChanged(float Value);

    UFUNCTION()
    void HandleMasterChanged(float Value);

    UFUNCTION()
    void HandleMusicChanged(float Value);

    UFUNCTION()
    void HandleSfxChanged(float Value);

    // —— 控件树 ——
    UPROPERTY()
    UCanvasPanel* PhoneCanvas;

    UPROPERTY()
    UBorder* Overlay;

    UPROPERTY()
    UTextBlock* TimeText;

    UPROPERTY()
    UTextBlock* BatteryText;

    UPROPERTY()
    UTextBlock* PageTitle;

    UPROPERTY()
    UGridPanel* AppGrid;

    UPROPERTY()
    UVerticalBox* ScreenBox;

    UPROPERTY()
    UVerticalBox* SettingsBox;

    UPROPERTY()
    UHorizontalBox* BottomNav;

    // —— 状态 ——
    bool bPhoneOpen;
    int32 CurrentPage;
    int32 CurrentSettingPage;

    // —— 设置值 ——
    float LookSensitivity;
    int32 ResolutionIndex;
    int32 QualityIndex;
    float MasterVolume;
    float MusicVolume;
    float SfxVolume;

    // 防止GC
    UPROPERTY()
    TArray<UButton*> AllButtons;

    UPROPERTY()
    TArray<USlider*> AllSliders;
};
