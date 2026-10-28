#include "EOMobilePhoneWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Styling/CoreStyle.h"
#include "Blueprint/WidgetTree.h"

void UEOMobilePhoneWidget::NativeConstruct()
{
    Super::NativeConstruct();

    LookSensitivity = 1.0f;
    ResolutionIndex = 1;
    QualityIndex = 1;
    MasterVolume = 0.8f;
    MusicVolume = 0.7f;
    SfxVolume = 0.9f;

    bPhoneOpen = false;
    CurrentPage = 0;
    CurrentSettingPage = 0;

    BuildPhoneUI();
    SetVisibility(ESlateVisibility::Collapsed);
}

void UEOMobilePhoneWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bPhoneOpen && TimeText)
    {
        FDateTime Now = FDateTime::Now();
        TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Now.GetHour(), Now.GetMinute())));
    }
}

// ================================================================ 构建UI

void UEOMobilePhoneWidget::BuildPhoneUI()
{
    PhoneCanvas = NewObject<UCanvasPanel>(this);
    PhoneCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    // 全屏半透明遮罩
    Overlay = NewObject<UBorder>(this);
    Overlay->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f));
    if (UCanvasPanelSlot* OverlaySlot = PhoneCanvas->AddChildToCanvas(Overlay))
    {
        OverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        OverlaySlot->SetOffsets(FMargin(0.0f));
        OverlaySlot->SetZOrder(0);
    }

    // 手机机身（居中 340x680）
    UBorder* PhoneBody = NewObject<UBorder>(this);
    PhoneBody->SetBrushColor(FLinearColor(0.07f, 0.08f, 0.10f, 1.0f));
    PhoneBody->SetPadding(FMargin(8.0f));
    if (UCanvasPanelSlot* BodySlot = PhoneCanvas->AddChildToCanvas(PhoneBody))
    {
        BodySlot->SetAnchors(FAnchors(0.5f));
        BodySlot->SetAlignment(FVector2D(0.5f, 0.5f));
        BodySlot->SetSize(FVector2D(340.0f, 680.0f));
        BodySlot->SetZOrder(1);
    }

    UVerticalBox* PhoneBox = NewObject<UVerticalBox>(this);
    PhoneBody->SetContent(PhoneBox);

    // —— 状态栏 ——
    UHorizontalBox* StatusBar = NewObject<UHorizontalBox>(this);
    StatusBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if (UVerticalBoxSlot* SBS = PhoneBox->AddChildToVerticalBox(StatusBar))
    {
        SBS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 2.0f));
    }

    TimeText = MakeText(TEXT("00:00"), 14.0f, FLinearColor(0.92f, 0.92f, 0.92f, 1.0f));
    if (UHorizontalBoxSlot* TS = StatusBar->AddChildToHorizontalBox(TimeText))
    {
        TS->SetHorizontalAlignment(HAlign_Left);
    }

    UTextBlock* SignalText = MakeText(TEXT("▮▮▮ 5G"), 13.0f, FLinearColor(0.35f, 0.9f, 0.45f, 1.0f));
    if (UHorizontalBoxSlot* SS = StatusBar->AddChildToHorizontalBox(SignalText))
    {
        SS->SetPadding(FMargin(6.0f, 0.0f));
        SS->SetHorizontalAlignment(HAlign_Center);
    }

    BatteryText = MakeText(TEXT("电池 87%"), 13.0f, FLinearColor(0.95f, 0.95f, 0.95f, 1.0f));
    if (UHorizontalBoxSlot* BS = StatusBar->AddChildToHorizontalBox(BatteryText))
    {
        BS->SetPadding(FMargin(6.0f, 0.0f, 0.0f, 0.0f));
        BS->SetHorizontalAlignment(HAlign_Right);
    }

    // —— 页面标题 ——
    PageTitle = MakeText(TEXT("手机 · 主屏"), 18.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* PTS = PhoneBox->AddChildToVerticalBox(PageTitle))
    {
        PTS->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 4.0f));
        PTS->SetHorizontalAlignment(HAlign_Center);
    }

    // —— 屏幕内容区 ——
    ScreenBox = NewObject<UVerticalBox>(this);
    if (UVerticalBoxSlot* SCS = PhoneBox->AddChildToVerticalBox(ScreenBox))
    {
        SCS->SetPadding(FMargin(4.0f, 0.0f, 4.0f, 0.0f));
        SCS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }

    // —— 底部导航 ——
    BottomNav = NewObject<UHorizontalBox>(this);
    if (UVerticalBoxSlot* BNS = PhoneBox->AddChildToVerticalBox(BottomNav))
    {
        BNS->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 2.0f));
    }

    UButton* HomeBtn = MakeTextButton(TEXT("⌂ 主页"), 14.0f);
    HomeBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleNavHome);
    if (UHorizontalBoxSlot* HS = BottomNav->AddChildToHorizontalBox(HomeBtn))
    {
        HS->SetPadding(FMargin(4.0f));
        HS->SetHorizontalAlignment(HAlign_Left);
    }

    UButton* CloseBtn = MakeTextButton(TEXT("✕ 关闭"), 14.0f);
    CloseBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleNavClose);
    if (UHorizontalBoxSlot* CS = BottomNav->AddChildToHorizontalBox(CloseBtn))
    {
        CS->SetPadding(FMargin(4.0f));
        CS->SetHorizontalAlignment(HAlign_Right);
    }

    WidgetTree->RootWidget = PhoneCanvas;

    ShowHomeScreen();
}

void UEOMobilePhoneWidget::ShowHomeScreen()
{
    if (!ScreenBox) return;

    ScreenBox->ClearChildren();
    if (PageTitle)
    {
        PageTitle->SetText(FText::FromString(TEXT("手机 · 主屏")));
    }
    CurrentPage = 0;

    AppGrid = NewObject<UGridPanel>(this);
    if (UVerticalBoxSlot* GS = ScreenBox->AddChildToVerticalBox(AppGrid))
    {
        GS->SetPadding(FMargin(0.0f, 4.0f));
        GS->SetHorizontalAlignment(HAlign_Center);
    }

    struct FAppDef
    {
        const TCHAR* Icon;
        const TCHAR* Name;
    };

    FAppDef Apps[] = {
        { TEXT("⚙"), TEXT("设置") },
        { TEXT("🎒"), TEXT("背包") },
        { TEXT("🗺"), TEXT("地图") },
        { TEXT("📋"), TEXT("任务") },
        { TEXT("🏪"), TEXT("商店") },
        { TEXT("ℹ"), TEXT("关于") },
    };

    const int32 Cols = 2;
    for (int32 i = 0; i < 6; ++i)
    {
        UButton* AppBtn = MakeAppButton(Apps[i].Icon, Apps[i].Name);
        switch (i)
        {
        case 0: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppSettings); break;
        case 1: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppInventory); break;
        case 2: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppMap); break;
        case 3: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppQuest); break;
        case 4: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppShop); break;
        case 5: AppBtn->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleAppAbout); break;
        default: break;
        }

        if (UGridSlot* AS = AppGrid->AddChildToGrid(AppBtn, i / Cols, i % Cols))
        {
            AS->SetPadding(FMargin(6.0f, 8.0f));
            AS->SetHorizontalAlignment(HAlign_Fill);
        }
    }
}

void UEOMobilePhoneWidget::ShowSettingsScreen()
{
    if (!ScreenBox) return;

    ScreenBox->ClearChildren();
    if (PageTitle)
    {
        PageTitle->SetText(FText::FromString(TEXT("设置")));
    }
    CurrentPage = 1;

    // Tab 行
    UHorizontalBox* Tabs = NewObject<UHorizontalBox>(this);
    if (UVerticalBoxSlot* TS = ScreenBox->AddChildToVerticalBox(Tabs))
    {
        TS->SetPadding(FMargin(0.0f, 2.0f));
        TS->SetHorizontalAlignment(HAlign_Center);
    }

    UButton* Tab0 = MakeTextButton(TEXT("操作"), 15.0f);
    Tab0->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleTabOperation);
    if (UHorizontalBoxSlot* TBS = Tabs->AddChildToHorizontalBox(Tab0))
    {
        TBS->SetPadding(FMargin(6.0f));
    }

    UButton* Tab1 = MakeTextButton(TEXT("画面"), 15.0f);
    Tab1->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleTabGraphics);
    if (UHorizontalBoxSlot* TBS = Tabs->AddChildToHorizontalBox(Tab1))
    {
        TBS->SetPadding(FMargin(6.0f));
    }

    UButton* Tab2 = MakeTextButton(TEXT("音量"), 15.0f);
    Tab2->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleTabAudio);
    if (UHorizontalBoxSlot* TBS = Tabs->AddChildToHorizontalBox(Tab2))
    {
        TBS->SetPadding(FMargin(6.0f));
    }

    SettingsBox = NewObject<UVerticalBox>(this);
    if (UVerticalBoxSlot* SS = ScreenBox->AddChildToVerticalBox(SettingsBox))
    {
        SS->SetPadding(FMargin(0.0f, 6.0f, 0.0f, 0.0f));
        SS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }

    ShowSettingPage(CurrentSettingPage);
}

void UEOMobilePhoneWidget::ShowSettingPage(int32 Page)
{
    if (!SettingsBox) return;

    SettingsBox->ClearChildren();
    CurrentSettingPage = Page;

    if (Page == 0)
    {
        USlider* SensSlider = MakeSliderRow(TEXT("摇杆/视角灵敏度"), 0.5f, 2.0f, LookSensitivity);
        if (SensSlider)
        {
            SensSlider->OnValueChanged.AddDynamic(this, &UEOMobilePhoneWidget::HandleSensitivityChanged);
        }

        UTextBlock* Tip = MakeText(TEXT("操作提示：\n右侧虚拟摇杆控制移动\n跳跃键跳跃\n手机内可调整全部设置"), 13.0f, FLinearColor(0.75f, 0.75f, 0.78f, 1.0f));
        if (UVerticalBoxSlot* TS = SettingsBox->AddChildToVerticalBox(Tip))
        {
            TS->SetPadding(FMargin(4.0f, 12.0f, 4.0f, 0.0f));
        }
    }
    else if (Page == 1)
    {
        UTextBlock* ResTitle = MakeText(TEXT("分辨率"), 15.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        if (UVerticalBoxSlot* RS = SettingsBox->AddChildToVerticalBox(ResTitle))
        {
            RS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 2.0f));
        }

        UHorizontalBox* ResRow = NewObject<UHorizontalBox>(this);
        if (UVerticalBoxSlot* RRS = SettingsBox->AddChildToVerticalBox(ResRow))
        {
            RRS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 6.0f));
        }

        const TCHAR* ResNames[] = { TEXT("720P"), TEXT("1080P"), TEXT("1440P") };
        UButton* Res0 = MakeTextButton(ResNames[0], 14.0f);
        Res0->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleRes0);
        UButton* Res1 = MakeTextButton(ResNames[1], 14.0f);
        Res1->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleRes1);
        UButton* Res2 = MakeTextButton(ResNames[2], 14.0f);
        Res2->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleRes2);
        if (UHorizontalBoxSlot* OS = ResRow->AddChildToHorizontalBox(Res0)) { OS->SetPadding(FMargin(4.0f)); }
        if (UHorizontalBoxSlot* OS = ResRow->AddChildToHorizontalBox(Res1)) { OS->SetPadding(FMargin(4.0f)); }
        if (UHorizontalBoxSlot* OS = ResRow->AddChildToHorizontalBox(Res2)) { OS->SetPadding(FMargin(4.0f)); }

        UTextBlock* QualityTitle = MakeText(TEXT("画质"), 15.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        if (UVerticalBoxSlot* QS = SettingsBox->AddChildToVerticalBox(QualityTitle))
        {
            QS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 2.0f));
        }

        UHorizontalBox* QualityRow = NewObject<UHorizontalBox>(this);
        if (UVerticalBoxSlot* QRS = SettingsBox->AddChildToVerticalBox(QualityRow))
        {
            QRS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 6.0f));
        }

        const TCHAR* QualityNames[] = { TEXT("低"), TEXT("中"), TEXT("高") };
        UButton* Q0 = MakeTextButton(QualityNames[0], 14.0f);
        Q0->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleQuality0);
        UButton* Q1 = MakeTextButton(QualityNames[1], 14.0f);
        Q1->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleQuality1);
        UButton* Q2 = MakeTextButton(QualityNames[2], 14.0f);
        Q2->OnClicked.AddDynamic(this, &UEOMobilePhoneWidget::HandleQuality2);
        if (UHorizontalBoxSlot* OS = QualityRow->AddChildToHorizontalBox(Q0)) { OS->SetPadding(FMargin(4.0f)); }
        if (UHorizontalBoxSlot* OS = QualityRow->AddChildToHorizontalBox(Q1)) { OS->SetPadding(FMargin(4.0f)); }
        if (UHorizontalBoxSlot* OS = QualityRow->AddChildToHorizontalBox(Q2)) { OS->SetPadding(FMargin(4.0f)); }

        UTextBlock* CurVal = MakeText(
            FString::Printf(TEXT("当前：%s · 画质%s"), ResNames[ResolutionIndex], QualityNames[QualityIndex]),
            13.0f, FLinearColor(0.6f, 0.85f, 1.0f, 1.0f));
        if (UVerticalBoxSlot* CS = SettingsBox->AddChildToVerticalBox(CurVal))
        {
            CS->SetPadding(FMargin(4.0f, 2.0f, 4.0f, 0.0f));
        }
    }
    else
    {
        USlider* MasterS = MakeSliderRow(TEXT("主音量"), 0.0f, 1.0f, MasterVolume);
        if (MasterS)
        {
            MasterS->OnValueChanged.AddDynamic(this, &UEOMobilePhoneWidget::HandleMasterChanged);
        }

        USlider* MusicS = MakeSliderRow(TEXT("音乐音量"), 0.0f, 1.0f, MusicVolume);
        if (MusicS)
        {
            MusicS->OnValueChanged.AddDynamic(this, &UEOMobilePhoneWidget::HandleMusicChanged);
        }

        USlider* SfxS = MakeSliderRow(TEXT("音效音量"), 0.0f, 1.0f, SfxVolume);
        if (SfxS)
        {
            SfxS->OnValueChanged.AddDynamic(this, &UEOMobilePhoneWidget::HandleSfxChanged);
        }
    }
}

// ================================================================ 控件工厂

UButton* UEOMobilePhoneWidget::MakeAppButton(const FString& IconText, const FString& Name)
{
    UButton* Btn = NewObject<UButton>(this);
    Btn->SetBackgroundColor(FLinearColor(0.12f, 0.15f, 0.19f, 1.0f));

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    VBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Btn->AddChild(VBox);

    UTextBlock* Icon = MakeText(IconText, 26.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* IS = VBox->AddChildToVerticalBox(Icon))
    {
        IS->SetHorizontalAlignment(HAlign_Center);
        IS->SetPadding(FMargin(0.0f, 6.0f, 0.0f, 2.0f));
    }

    UTextBlock* Label = MakeText(Name, 13.0f, FLinearColor(0.92f, 0.92f, 0.92f, 1.0f));
    if (UVerticalBoxSlot* LS = VBox->AddChildToVerticalBox(Label))
    {
        LS->SetHorizontalAlignment(HAlign_Center);
        LS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
    }

    AllButtons.Add(Btn);
    return Btn;
}

UButton* UEOMobilePhoneWidget::MakeTextButton(const FString& Content, float FontSize)
{
    UButton* Btn = NewObject<UButton>(this);
    Btn->SetBackgroundColor(FLinearColor(0.18f, 0.22f, 0.30f, 1.0f));

    UTextBlock* Label = MakeText(Content, FontSize, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    Btn->AddChild(Label);

    AllButtons.Add(Btn);
    return Btn;
}

USlider* UEOMobilePhoneWidget::MakeSliderRow(const FString& Label, float MinV, float MaxV, float DefaultV)
{
    UTextBlock* LabelText = MakeText(Label, 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* LS = SettingsBox->AddChildToVerticalBox(LabelText))
    {
        LS->SetPadding(FMargin(4.0f, 6.0f, 4.0f, 2.0f));
    }

    USlider* Slider = NewObject<USlider>(this);
    Slider->SetMinValue(MinV);
    Slider->SetMaxValue(MaxV);
    Slider->SetValue(DefaultV);
    Slider->SetStepSize(0.01f);
    if (UVerticalBoxSlot* SS = SettingsBox->AddChildToVerticalBox(Slider))
    {
        SS->SetPadding(FMargin(10.0f, 0.0f, 10.0f, 4.0f));
    }

    AllSliders.Add(Slider);
    return Slider;
}

UTextBlock* UEOMobilePhoneWidget::MakeText(const FString& Content, float FontSize, const FLinearColor& Color)
{
    UTextBlock* Text = NewObject<UTextBlock>(this);
    Text->SetText(FText::FromString(Content));
    Text->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), FontSize));
    Text->SetColorAndOpacity(FSlateColor(Color));
    return Text;
}

// ================================================================ 应用回调

void UEOMobilePhoneWidget::HandleAppSettings()
{
    ShowSettingsScreen();
}

void UEOMobilePhoneWidget::HandleAppInventory()
{
    if (PageTitle) PageTitle->SetText(FText::FromString(TEXT("背包（开发中）")));
}

void UEOMobilePhoneWidget::HandleAppMap()
{
    if (PageTitle) PageTitle->SetText(FText::FromString(TEXT("地图（开发中）")));
}

void UEOMobilePhoneWidget::HandleAppQuest()
{
    if (PageTitle) PageTitle->SetText(FText::FromString(TEXT("任务（开发中）")));
}

void UEOMobilePhoneWidget::HandleAppShop()
{
    if (PageTitle) PageTitle->SetText(FText::FromString(TEXT("商店（开发中）")));
}

void UEOMobilePhoneWidget::HandleAppAbout()
{
    if (PageTitle) PageTitle->SetText(FText::FromString(TEXT("关于\n地球Online v0.2.0\n逗星科技")));
}

void UEOMobilePhoneWidget::HandleNavHome()
{
    ShowHomeScreen();
}

void UEOMobilePhoneWidget::HandleNavClose()
{
    ClosePhone();
}

void UEOMobilePhoneWidget::HandleTabOperation()
{
    ShowSettingPage(0);
}

void UEOMobilePhoneWidget::HandleTabGraphics()
{
    ShowSettingPage(1);
}

void UEOMobilePhoneWidget::HandleTabAudio()
{
    ShowSettingPage(2);
}

void UEOMobilePhoneWidget::HandleRes0() { ResolutionIndex = 0; ShowSettingPage(1); }
void UEOMobilePhoneWidget::HandleRes1() { ResolutionIndex = 1; ShowSettingPage(1); }
void UEOMobilePhoneWidget::HandleRes2() { ResolutionIndex = 2; ShowSettingPage(1); }

void UEOMobilePhoneWidget::HandleQuality0() { QualityIndex = 0; ShowSettingPage(1); }
void UEOMobilePhoneWidget::HandleQuality1() { QualityIndex = 1; ShowSettingPage(1); }
void UEOMobilePhoneWidget::HandleQuality2() { QualityIndex = 2; ShowSettingPage(1); }

void UEOMobilePhoneWidget::HandleSensitivityChanged(float Value)
{
    LookSensitivity = Value;
    ApplySettings();
}

void UEOMobilePhoneWidget::HandleMasterChanged(float Value)
{
    MasterVolume = Value;
    ApplySettings();
}

void UEOMobilePhoneWidget::HandleMusicChanged(float Value)
{
    MusicVolume = Value;
    ApplySettings();
}

void UEOMobilePhoneWidget::HandleSfxChanged(float Value)
{
    SfxVolume = Value;
    ApplySettings();
}

// ================================================================ 打开/关闭

void UEOMobilePhoneWidget::OpenPhone()
{
    SetVisibility(ESlateVisibility::Visible);
    bPhoneOpen = true;

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->SetInputMode(FInputModeGameAndUI());
        PC->bShowMouseCursor = true;
    }
}

void UEOMobilePhoneWidget::ClosePhone()
{
    SetVisibility(ESlateVisibility::Collapsed);
    bPhoneOpen = false;

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void UEOMobilePhoneWidget::ApplySettings()
{
    UE_LOG(LogTemp, Log, TEXT("[手机设置] 灵敏度=%.2f 分辨率=%d 画质=%d 主音量=%.2f 音乐=%.2f 音效=%.2f"),
        LookSensitivity, ResolutionIndex, QualityIndex, MasterVolume, MusicVolume, SfxVolume);
}
