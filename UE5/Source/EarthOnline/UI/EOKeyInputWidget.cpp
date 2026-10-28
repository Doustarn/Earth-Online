#include "EOKeyInputWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "RenderingThread.h"
#include "ImageUtils.h"
#include "../Security/EOKeyManager.h"
#include "../HUD/EOHUD.h"

bool UEOKeyInputWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    EarthRotation = 0.0f;
    CurrentStage = -1;
    bLoading = false;

    BuildUI();

    // 检查是否已激活
    if (UEOKeyManager::IsKeyActivated())
    {
        SetStatus(TEXT("已激活，正在进入地球..."), FColor::Green);
        BeginLoading();
    }
    else
    {
        SetStatus(TEXT("请输入接入密钥以继续"), FColor::White);
    }

    return true;
}

void UEOKeyInputWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (EarthImage)
    {
        EarthRotation += InDeltaTime * 6.0f;
        EarthImage->SetRenderTransformAngle(EarthRotation);
    }
}

void UEOKeyInputWidget::BeginLoading()
{
    if (bLoading) return;
    bLoading = true;
    CurrentStage = 0;

    if (KeyPanel) KeyPanel->SetVisibility(ESlateVisibility::Collapsed);

    // 显示第一段加载
    StageText->SetVisibility(ESlateVisibility::Visible);
    SubText->SetVisibility(ESlateVisibility::Visible);
    SetStageText(TEXT("人物生成中"));

    // 依次推进
    GetWorld()->GetTimerManager().SetTimer(StageTimer, this, &UEOKeyInputWidget::AdvanceStage, 2.0f, true);
    GetWorld()->GetTimerManager().SetTimer(FlashTimer, this, &UEOKeyInputWidget::ShowFlash, 2.0f - 0.25f, true);
}

void UEOKeyInputWidget::CompleteImmediately()
{
    FinishLoading();
}

void UEOKeyInputWidget::AdvanceStage()
{
    CurrentStage++;
    if (CurrentStage < 4)
    {
        static const FString StageNames[] = { TEXT("人物生成中"), TEXT("场景生成中"), TEXT("海洋生成中"), TEXT("加载中") };
        SetStageText(StageNames[CurrentStage]);
        // 段间闪黑
        if (FlashImage)
        {
            FlashImage->SetVisibility(ESlateVisibility::Visible);
            FlashImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
            GetWorld()->GetTimerManager().SetTimer(FlashHideTimer, [this]()
            {
                if (FlashImage) FlashImage->SetVisibility(ESlateVisibility::Hidden);
            }, 0.3f, false);
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(StageTimer);
        GetWorld()->GetTimerManager().ClearTimer(FlashTimer);
        FinishLoading();
    }
}

void UEOKeyInputWidget::ShowFlash()
{
    // 每段结束前的闪黑由 AdvanceStage 触发，这里作为兜底
}

void UEOKeyInputWidget::FinishLoading()
{
    bLoading = false;
    if (SubText) SubText->SetText(FText::FromString(TEXT("载入完成，欢迎来到地球Online")));
    SetStatus(TEXT(""), FColor::White);

    FTimerHandle H;
    GetWorld()->GetTimerManager().SetTimer(H, [this]()
    {
        this->RemoveFromParent();
        OnLoadingComplete.Broadcast();
    }, 1.2f, false);
}

void UEOKeyInputWidget::SetStageText(const FString& Text)
{
    if (StageText)
    {
        StageText->SetText(FText::FromString(Text));
    }
    if (SubText)
    {
        SubText->SetText(FText::FromString(TEXT("正在进入地球Online...")));
    }
}

// ============================================================ 构建

void UEOKeyInputWidget::BuildUI()
{
    MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    if (WidgetTree) WidgetTree->RootWidget = MainCanvas;

    BuildBackground();
    BuildEarth();
    BuildStageText();
    BuildKeyVerify();
}

void UEOKeyInputWidget::BuildBackground()
{
    // 非黑背景：深蓝→星空渐变
    UImage* Bg = NewObject<UImage>(this);
    Bg->SetColorAndOpacity(FLinearColor(0.03f, 0.06f, 0.12f, 1.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Bg))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        S->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetZOrder(0);
    }

    // 顶部亮度渐变（模拟天空）
    UImage* Grad = NewObject<UImage>(this);
    Grad->SetColorAndOpacity(FLinearColor(0.05f, 0.1f, 0.18f, 0.6f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Grad))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.5f));
        S->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetZOrder(1);
    }

    // 闪黑遮罩（默认隐藏）
    FlashImage = NewObject<UImage>(this);
    FlashImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
    FlashImage->SetVisibility(ESlateVisibility::Hidden);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(FlashImage))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        S->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetZOrder(50);
    }
}

void UEOKeyInputWidget::BuildEarth()
{
    GeneratedEarth = CreateEarthTexture();
    EarthImage = NewObject<UImage>(this);
    EarthImage->SetBrushFromTexture(GeneratedEarth);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(EarthImage))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, -30.0f));
        S->SetSize(FVector2D(520.0f, 520.0f));
        S->SetZOrder(5);
    }
}

void UEOKeyInputWidget::BuildStageText()
{
    StageText = MakeText(TEXT(""), 30.0f, FLinearColor(0.9f, 0.95f, 1.0f, 1.0f));
    StageText->SetJustification(ETextJustify::Center);
    StageText->SetVisibility(ESlateVisibility::Collapsed);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(StageText))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, 210.0f));
        S->SetSize(FVector2D(900.0f, 50.0f));
        S->SetZOrder(10);
    }

    SubText = MakeText(TEXT("正在进入地球Online..."), 16.0f, FLinearColor(0.7f, 0.8f, 0.95f, 0.9f));
    SubText->SetJustification(ETextJustify::Center);
    SubText->SetVisibility(ESlateVisibility::Collapsed);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(SubText))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, 265.0f));
        S->SetSize(FVector2D(900.0f, 30.0f));
        S->SetZOrder(10);
    }
}

void UEOKeyInputWidget::BuildKeyVerify()
{
    KeyPanel = NewObject<UBorder>(this);
    KeyPanel->SetBrushColor(FLinearColor(0.05f, 0.1f, 0.18f, 0.85f));
    KeyPanel->SetPadding(FMargin(30.0f, 24.0f, 30.0f, 24.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(KeyPanel))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, -20.0f));
        S->SetAutoSize(true);
        S->SetZOrder(20);
    }

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    KeyPanel->SetContent(VBox);

    UTextBlock* Title = MakeText(TEXT("地球Online · 通行证接入"), 22.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    Title->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* TS = VBox->AddChildToVerticalBox(Title))
    {
        TS->SetHorizontalAlignment(HAlign_Fill);
        TS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    KeyInput = NewObject<UEditableTextBox>(this);
    KeyInput->SetHintText(FText::FromString(TEXT("请输入接入密钥 / 通行证")));
    KeyInput->SetIsReadOnly(false);
    KeyInput->OnTextChanged.AddDynamic(this, &UEOKeyInputWidget::OnKeyTextChanged);
    if (UVerticalBoxSlot* KS = VBox->AddChildToVerticalBox(KeyInput))
    {
        KS->SetHorizontalAlignment(HAlign_Fill);
        KS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
    }

    VerifyButton = MakeStyledButton(TEXT("接入地球"), 18.0f, FLinearColor(0.12f, 0.55f, 0.32f, 0.95f), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    VerifyButton->OnClicked.AddDynamic(this, &UEOKeyInputWidget::OnVerifyClicked);
    if (UVerticalBoxSlot* VS = VBox->AddChildToVerticalBox(VerifyButton))
    {
        VS->SetHorizontalAlignment(HAlign_Center);
        VS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    StatusText = MakeText(TEXT("请输入接入密钥以继续"), 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.9f));
    StatusText->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* SS = VBox->AddChildToVerticalBox(StatusText))
    {
        SS->SetHorizontalAlignment(HAlign_Fill);
    }
}

// ============================================================ 交互

void UEOKeyInputWidget::OnVerifyClicked()
{
    if (!KeyInput) return;
    FString InputKey = KeyInput->GetText().ToString();
    if (InputKey.IsEmpty())
    {
        SetStatus(TEXT("密钥不能为空"), FColor::Red);
        return;
    }
    SetStatus(TEXT("正在验证密钥..."), FColor::Yellow);
    if (UEOKeyManager::VerifyBetaKey(InputKey))
    {
        UEOKeyManager::SaveActivatedKey(InputKey);
        SetStatus(TEXT("密钥验证成功！正在进入游戏..."), FColor::Green);
        BeginLoading();
    }
    else
    {
        SetStatus(TEXT("密钥无效，请检查后重试"), FColor::Red);
    }
}

void UEOKeyInputWidget::OnKeyTextChanged(const FText& Text)
{
    if (KeyInput)
    {
        FString UpperText = Text.ToString().ToUpper();
        if (UpperText != Text.ToString())
        {
            KeyInput->SetText(FText::FromString(UpperText));
        }
    }
}

void UEOKeyInputWidget::SetStatus(const FString& Message, FColor Color)
{
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(Message));
        StatusText->SetColorAndOpacity(FSlateColor(Color));
    }
}

// ============================================================ 辅助控件

UButton* UEOKeyInputWidget::MakeStyledButton(const FString& Text, float FontSize, const FLinearColor& Bg, const FLinearColor& Fg)
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
    return Btn;
}

UTextBlock* UEOKeyInputWidget::MakeText(const FString& Text, float Size, const FLinearColor& Color)
{
    UTextBlock* T = NewObject<UTextBlock>(this);
    T->SetText(FText::FromString(Text));
    T->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), Size));
    T->SetColorAndOpacity(Color);
    return T;
}

// ============================================================ 地球贴图

UTexture2D* UEOKeyInputWidget::CreateEarthTexture()
{
    // 使用"圆形地球+亮星空"合成图（不透明，无透明黑圈问题）
    const FString EarthPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Content/UI/earth_glow.png"));
    if (FPaths::FileExists(EarthPath))
    {
        UTexture2D* Tex = FImageUtils::ImportFileAsTexture2D(EarthPath);
        if (Tex) return Tex;
    }
    const FString FallbackPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Content/UI/earth_texture.png"));
    if (FPaths::FileExists(FallbackPath))
        return FImageUtils::ImportFileAsTexture2D(FallbackPath);
    return nullptr;
}

void UEOKeyInputWidget::FillEarthPixels(TArray<FColor>& Pixels, int32 W, int32 H)
{
    static const struct { float cx, cy, rx, ry; } LandBlocks[] = {
        { 0.16f, 0.30f, 0.20f, 0.18f },  // 北美
        { 0.28f, 0.62f, 0.12f, 0.14f },  // 南美
        { 0.55f, 0.26f, 0.34f, 0.18f },  // 欧亚
        { 0.53f, 0.52f, 0.14f, 0.13f },  // 非洲
        { 0.80f, 0.36f, 0.10f, 0.09f },  // 亚洲东南
        { 0.86f, 0.70f, 0.08f, 0.07f },  // 澳洲
        { 0.50f, 0.93f, 0.32f, 0.05f },  // 南极
        { 0.68f, 0.45f, 0.08f, 0.08f },  // 印度
    };
    const int32 NumLand = sizeof(LandBlocks) / sizeof(LandBlocks[0]);

    for (int32 y = 0; y < H; y++)
    {
        const float ny = y / (float)H;
        for (int32 x = 0; x < W; x++)
        {
            const float nx = x / (float)W;
            const int32 Idx = y * W + x;
            float Polar = FMath::Abs(ny - 0.5f) * 2.0f;
            FLinearColor Ocean(0.02f + 0.05f * Polar, 0.15f + 0.12f * Polar, 0.40f + 0.18f * (1.0f - Polar), 1.0f);
            float LandMask = 0.0f;
            for (int32 i = 0; i < NumLand; i++)
            {
                float dx = (nx - LandBlocks[i].cx) / LandBlocks[i].rx;
                float dy = (ny - LandBlocks[i].cy) / LandBlocks[i].ry;
                float Dist = FMath::Sqrt(dx * dx + dy * dy);
                float Edge = FMath::Clamp(1.0f - Dist, 0.0f, 1.0f);
                float Jitter = FMath::Sin(nx * 40.0f + ny * 33.0f + i * 7.0f) * 0.12f;
                float Mask = FMath::Clamp(Edge - 0.55f + Jitter, 0.0f, 1.0f);
                LandMask = FMath::Max(LandMask, Mask);
            }
            FLinearColor Land(0.16f + 0.10f * Polar, 0.45f - 0.10f * Polar, 0.20f - 0.06f * Polar, 1.0f);
            FLinearColor Col = Ocean * (1.0f - LandMask) + Land * LandMask;
            float CloudNoise = FMath::Frac(FMath::Sin(nx * 57.0f + ny * 41.0f) * 43758.5453f);
            if (CloudNoise > 0.72f)
            {
                float CloudAlpha = FMath::Clamp((CloudNoise - 0.72f) / 0.28f, 0.0f, 1.0f) * 0.35f;
                Col = Col * (1.0f - CloudAlpha) + FLinearColor(0.95f, 0.96f, 1.0f, 1.0f) * CloudAlpha;
            }
            Pixels[Idx] = Col.ToFColor(false);
        }
    }

    // 圆形透明边缘：把矩形贴图裁成圆球（边缘外透明），旋转时不再是"正方形在转"
    const float Cx = (W - 1) / 2.0f;
    const float Cy = (H - 1) / 2.0f;
    for (int32 y = 0; y < H; y++)
    {
        for (int32 x = 0; x < W; x++)
        {
            const float ncx = (x - Cx) / Cx;
            const float ncy = (y - Cy) / Cy;
            if (FMath::Sqrt(ncx * ncx + ncy * ncy) > 1.0f)
            {
                Pixels[y * W + x] = FColor(0, 0, 0, 0); // 透明
            }
        }
    }
}
