#include "EOMainMenuWidget.h"
#include "../Utils/EOLang.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "RenderingThread.h"
#include "ImageUtils.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Styling/CoreStyle.h"

bool UEOMainMenuWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    UEOLang::LoadLang();
    BuildUI();
    return true;
}

void UEOMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    // 开始界面无需动画
}

void UEOMainMenuWidget::ProceedToLogin()
{
    OnStartGame.Broadcast();
}

void UEOMainMenuWidget::OnStartClicked()
{
    ProceedToLogin();
}

void UEOMainMenuWidget::OnIconClicked()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor(0.25f, 0.7f, 1.0f), TEXT("Coming soon..."));
    }
}

// ============================================================ 构建

void UEOMainMenuWidget::BuildUI()
{
    MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    if (WidgetTree) WidgetTree->RootWidget = MainCanvas;

    BuildBackground();
    BuildTitle();
    BuildCenterButtons();
    BuildBottomBar();
}

void UEOMainMenuWidget::BuildBackground()
{
    // 深色星空背景
    UTexture2D* BgTex = CreateStarTexture();
    UImage* Bg = NewObject<UImage>(this);
    if (BgTex)
    {
        Bg->SetBrushFromTexture(BgTex);
        Bg->SetColorAndOpacity(FLinearColor(1.2f, 1.2f, 1.4f, 1.0f));
    }
    else
    {
        Bg->SetColorAndOpacity(FLinearColor(0.02f, 0.03f, 0.07f, 1.0f));
    }
    Bg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Bg))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        S->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetZOrder(0);
    }
}

void UEOMainMenuWidget::BuildTitle()
{
    // 大标题 Immortal Dao Online
    UTextBlock* Title = MakeText(TEXT("IMMORTAL DAO ONLINE"), 52.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Title))
    {
        S->SetAnchors(FAnchors(0.5f, 0.35f, 0.5f, 0.35f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    // 中文副标题
    UTextBlock* Sub = MakeText(UEOLang::T(TEXT("仙 途 · 修 仙 在 线"), TEXT("Xianxia Online")), 22.0f, FLinearColor(0.7f, 0.75f, 0.85f, 0.9f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Sub))
    {
        S->SetAnchors(FAnchors(0.5f, 0.42f, 0.5f, 0.42f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }
}

void UEOMainMenuWidget::BuildCenterButtons()
{
    UVerticalBox* Menu = NewObject<UVerticalBox>(this);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Menu))
    {
        S->SetAnchors(FAnchors(0.5f, 0.72f, 0.5f, 0.72f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    // 语言选择
    UTextBlock* LangLabel = MakeText(TEXT("Language / 语言"), 14.0f, FLinearColor(0.6f, 0.65f, 0.75f, 0.8f));
    if (UVerticalBoxSlot* LS = Menu->AddChildToVerticalBox(LangLabel))
    {
        LS->SetHorizontalAlignment(HAlign_Center);
        LS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
    }

    UHorizontalBox* LangRow = NewObject<UHorizontalBox>(this);
    if (UVerticalBoxSlot* LRS = Menu->AddChildToVerticalBox(LangRow))
    {
        LRS->SetHorizontalAlignment(HAlign_Center);
        LRS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 20.0f));
    }
    UButton* BtnCN = MakeStyledButton(TEXT("简体中文"), 16.0f, FLinearColor(0.15f, 0.2f, 0.3f, 0.8f), FLinearColor(1.0f, 0.95f, 0.7f, 1.0f));
    if (UHorizontalBoxSlot* CS = LangRow->AddChildToHorizontalBox(BtnCN)) CS->SetPadding(FMargin(6.0f, 0.0f, 6.0f, 0.0f));
    UButton* BtnEN = MakeStyledButton(TEXT("English"), 16.0f, FLinearColor(0.15f, 0.2f, 0.3f, 0.8f), FLinearColor(0.9f, 0.95f, 1.0f, 1.0f));
    if (UHorizontalBoxSlot* ES = LangRow->AddChildToHorizontalBox(BtnEN)) ES->SetPadding(FMargin(6.0f, 0.0f, 6.0f, 0.0f));

    // 主按钮：开始游戏
    StartButton = MakeBlackMythButton(UEOLang::T(TEXT("开 始 游 戏"), TEXT("START GAME")), true);
    StartButton->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnStartClicked);
    if (UVerticalBoxSlot* BS = Menu->AddChildToVerticalBox(StartButton))
    {
        BS->SetHorizontalAlignment(HAlign_Center);
    }
}

void UEOMainMenuWidget::BuildTopBar()
{
    // 开始界面不设右上角图标
}

void UEOMainMenuWidget::BuildBottomBar()
{
    FlavorText = MakeText(TEXT("Immortal Dao Online · 大型修仙开放世界"), 13.0f, FLinearColor(0.65f, 0.7f, 0.75f, 0.9f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(FlavorText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -50.0f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    CopyrightText = MakeText(TEXT("v0.1.8.1.1001 Alpha"), 11.0f, FLinearColor(0.5f, 0.5f, 0.55f, 0.8f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(CopyrightText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -25.0f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }
}

// ============================================================ 辅助控件

UButton* UEOMainMenuWidget::MakeStyledButton(const FString& Text, float FontSize, const FLinearColor& Bg, const FLinearColor& Fg)
{
    UButton* Btn = NewObject<UButton>(this);
    Btn->SetBackgroundColor(Bg);
    Btn->SetVisibility(ESlateVisibility::Visible);
    FButtonStyle BtnStyle = Btn->WidgetStyle;
    BtnStyle.Normal.TintColor = Bg;
    BtnStyle.Hovered.TintColor = Bg + FLinearColor(0.06f, 0.06f, 0.06f, 0.0f);
    BtnStyle.Pressed.TintColor = Bg - FLinearColor(0.05f, 0.05f, 0.05f, 0.0f);
    Btn->WidgetStyle = BtnStyle;

    UTextBlock* Label = NewObject<UTextBlock>(this);
    Label->SetText(FText::FromString(Text));
    Label->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), FontSize));
    Label->SetColorAndOpacity(Fg);
    Label->SetJustification(ETextJustify::Center);
    Btn->SetContent(Label);
    Btn->SetPadding(FMargin(20.0f, 8.0f, 20.0f, 8.0f));
    return Btn;
}

UButton* UEOMainMenuWidget::MakeBlackMythButton(const FString& Text, bool bPrimary)
{
    UButton* Btn = NewObject<UButton>(this);
    Btn->SetVisibility(ESlateVisibility::Visible);

    FButtonStyle BtnStyle = Btn->WidgetStyle;
    if (bPrimary)
    {
        BtnStyle.Normal.TintColor = FLinearColor(0.15f, 0.75f, 0.85f, 0.92f);
        BtnStyle.Hovered.TintColor = FLinearColor(0.30f, 0.92f, 1.0f, 1.0f);
        BtnStyle.Pressed.TintColor = FLinearColor(0.10f, 0.60f, 0.70f, 1.0f);
    }
    else
    {
        BtnStyle.Normal.TintColor = FLinearColor(0.05f, 0.05f, 0.08f, 0.55f);
        BtnStyle.Hovered.TintColor = FLinearColor(0.15f, 0.75f, 0.85f, 0.35f);
        BtnStyle.Pressed.TintColor = FLinearColor(0.05f, 0.05f, 0.08f, 0.85f);
    }
    Btn->WidgetStyle = BtnStyle;

    UTextBlock* Label = NewObject<UTextBlock>(this);
    Label->SetText(FText::FromString(Text));
    Label->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), bPrimary ? 28 : 18));
    Label->SetColorAndOpacity(bPrimary ? FLinearColor(0.02f, 0.08f, 0.10f, 1.0f) : FLinearColor(0.92f, 0.92f, 0.94f, 1.0f));
    Label->SetJustification(ETextJustify::Center);
    Btn->SetContent(Label);

    Btn->SetPadding(bPrimary ? FMargin(60.0f, 16.0f, 60.0f, 16.0f) : FMargin(36.0f, 10.0f, 36.0f, 10.0f));
    return Btn;
}

UTextBlock* UEOMainMenuWidget::MakeText(const FString& Text, float Size, const FLinearColor& Color)
{
    UTextBlock* T = NewObject<UTextBlock>(this);
    T->SetText(FText::FromString(Text));
    T->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), Size));
    T->SetColorAndOpacity(Color);
    return T;
}

// ============================================================ 星空背景贴图

UTexture2D* UEOMainMenuWidget::CreateStarTexture()
{
    const int32 W = 512, H = 256;
    TArray<FColor> Pixels;
    Pixels.SetNum(W * H);
    FillStarPixels(Pixels, W, H);

    UTexture2D* Tex = UTexture2D::CreateTransient(W, H, PF_B8G8R8A8);
    if (!Tex) return nullptr;
    Tex->SRGB = true;
    Tex->CompressionSettings = TC_Default;
    FTexture2DMipMap& Mip = Tex->GetPlatformData()->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(Data, Pixels.GetData(), Pixels.Num() * sizeof(FColor));
    Mip.BulkData.Unlock();
    Tex->UpdateResource();
    return Tex;
}

void UEOMainMenuWidget::FillStarPixels(TArray<FColor>& Pixels, int32 W, int32 H)
{
    FRandomStream Rng(20261024);
    const FLinearColor Top(0.05f, 0.08f, 0.18f, 1.0f);
    const FLinearColor Bot(0.12f, 0.06f, 0.20f, 1.0f);
    for (int32 y = 0; y < H; y++)
    {
        const float t = FMath::Clamp((y / (float)H) * 1.2f, 0.0f, 1.0f);
        FLinearColor Col = FMath::Lerp(Top, Bot, t);
        for (int32 x = 0; x < W; x++)
        {
            Pixels[y * W + x] = Col.ToFColor(false);
        }
    }
    const int32 NumStars = 240;
    for (int32 i = 0; i < NumStars; i++)
    {
        const int32 sx = Rng.RandRange(0, W - 1);
        const int32 sy = Rng.RandRange(0, H - 1);
        const float Bright = 0.5f + Rng.FRand() * 0.5f;
        const int32 Size = Rng.FRand() < 0.85f ? 1 : 2;
        for (int32 dy = -Size; dy <= Size; dy++)
        {
            for (int32 dx = -Size; dx <= Size; dx++)
            {
                const int32 px = sx + dx;
                const int32 py = sy + dy;
                if (px < 0 || px >= W || py < 0 || py >= H) continue;
                const float Dist = FMath::Sqrt((float)(dx * dx + dy * dy));
                const float Fall = FMath::Clamp(1.0f - Dist / (float)(Size + 1), 0.0f, 1.0f);
                FLinearColor Base(Pixels[py * W + px].R / 255.0f, Pixels[py * W + px].G / 255.0f, Pixels[py * W + px].B / 255.0f, 1.0f);
                FLinearColor Star = Base + FLinearColor(Bright * Fall * 0.7f, Bright * Fall * 0.7f, Bright * Fall * 0.9f, 0.0f);
                Pixels[py * W + px] = Star.ToFColor(false);
            }
        }
    }
}
