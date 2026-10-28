#include "EOMainMenuWidget.h"
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

bool UEOMainMenuWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    EarthRotation = 0.0f;

    BuildUI();

    return true;
}

void UEOMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 地球持续旋转
    if (EarthImage)
    {
        EarthRotation += InDeltaTime * 8.0f;
        EarthImage->SetRenderTransformAngle(EarthRotation);
    }
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
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor(0.25f, 0.7f, 1.0f), TEXT("该功能开发中，敬请期待"));
    }
}

// ============================================================ 构建

void UEOMainMenuWidget::BuildUI()
{
    MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    if (WidgetTree) WidgetTree->RootWidget = MainCanvas;

    BuildBackground();
    BuildEarth();
    BuildCenterButtons();
    BuildTopBar();
    BuildBottomBar();
}

void UEOMainMenuWidget::BuildBackground()
{
    // 真实太空星空背景（优先加载 Content/UI/space_bg.jpg），失败回退程序化星空
    UTexture2D* BgTex = nullptr;
    const FString BgPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Content/UI/space_bg.jpg"));
    if (FPaths::FileExists(BgPath))
    {
        BgTex = FImageUtils::ImportFileAsTexture2D(BgPath);
    }
    if (!BgTex)
    {
        BgTex = CreateStarTexture();
    }
    UImage* Bg = NewObject<UImage>(this);
    if (BgTex)
    {
        Bg->SetBrushFromTexture(BgTex);
        Bg->SetColorAndOpacity(FLinearColor(1.85f, 1.85f, 1.95f, 1.0f));
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

void UEOMainMenuWidget::BuildEarth()
{
    GeneratedEarth = CreateEarthTexture();
    if (!GeneratedEarth) return;

    EarthImage = NewObject<UImage>(this);
    EarthImage->SetBrushFromTexture(GeneratedEarth);
    EarthImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    EarthImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(EarthImage))
    {
        S->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        S->SetAlignment(FVector2D(0.0f, 1.0f));
        S->SetPosition(FVector2D(30.0f, -30.0f));
        S->SetSize(FVector2D(1050.0f, 1050.0f));
        S->SetZOrder(1);
    }
}

void UEOMainMenuWidget::BuildCenterButtons()
{
    // 中央：开始游戏按钮 + tap anywhere
    UVerticalBox* Center = NewObject<UVerticalBox>(this);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Center))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, -40.0f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    StartButton = MakeStyledButton(TEXT("开始游戏"), 30.0f, FLinearColor(0.14f, 0.36f, 0.62f, 0.62f), FLinearColor(0.96f, 0.98f, 1.0f, 1.0f));
    StartButton->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnStartClicked);
    if (UVerticalBoxSlot* BS = Center->AddChildToVerticalBox(StartButton))
    {
        BS->SetHorizontalAlignment(HAlign_Center);
        BS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
    }

    TapAnywhereText = MakeText(TEXT("tap anywhere to start"), 15.0f, FLinearColor(0.7f, 0.75f, 0.8f, 0.9f));
    if (UVerticalBoxSlot* TS = Center->AddChildToVerticalBox(TapAnywhereText))
    {
        TS->SetHorizontalAlignment(HAlign_Center);
    }

    // 服务器信息（地球下方）
    ServerInfoText = MakeText(TEXT("亚洲 · 中国服 · 山西"), 16.0f, FLinearColor(0.55f, 0.75f, 0.95f, 0.95f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(ServerInfoText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -160.0f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }
}

void UEOMainMenuWidget::BuildTopBar()
{

    // 右上：设置 / 音量 / 语音 / 备忘录 图标
    const TArray<FString> IconLabels = { TEXT("设置"), TEXT("音量"), TEXT("语音"), TEXT("备忘") };
    float XOff = -100.0f;
    for (int32 i = 0; i < IconLabels.Num(); i++)
    {
        UButton* IconBtn = MakeStyledButton(IconLabels[i], 20.0f, FLinearColor(0.08f, 0.1f, 0.15f, 0.7f), FLinearColor(0.85f, 0.9f, 1.0f, 1.0f));
        IconBtn->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnIconClicked);
        if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(IconBtn))
        {
            S->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
            S->SetAlignment(FVector2D(1.0f, 0.0f));
            S->SetPosition(FVector2D(XOff, 22.0f));
            S->SetSize(FVector2D(52.0f, 52.0f));
            S->SetZOrder(6);
        }
        XOff -= 62.0f;
    }
}

void UEOMainMenuWidget::BuildBottomBar()
{
    // 底部简短版本信息（不照抄参考图长文案）
    FlavorText = MakeText(TEXT("地球Online · 大型开放世界多人联机"), 13.0f, FLinearColor(0.65f, 0.7f, 0.75f, 0.9f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(FlavorText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -78.0f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    CopyrightText = MakeText(TEXT("v0.1.8 Beta 内测版"), 11.0f, FLinearColor(0.5f, 0.5f, 0.55f, 0.8f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(CopyrightText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -30.0f));
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

// ============================================================ 程序化地球贴图

UTexture2D* UEOMainMenuWidget::CreateEarthTexture()
{
    // 透明背景地球PNG：CreateTransient 保留 alpha，圆外透出提亮的背景星云（无黑圈、无方形边框）
    const FString EarthPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Content/UI/earth_glow.png"));
    if (FPaths::FileExists(EarthPath))
    {
        UTexture2D* RealTex = FImageUtils::ImportFileAsTexture2D(EarthPath);
        if (RealTex)
        {
            const int32 W = RealTex->GetSizeX();
            const int32 H = RealTex->GetSizeY();
            if (W > 0 && H > 0 && RealTex->GetPlatformData() && RealTex->GetPlatformData()->Mips.Num() > 0)
            {
                TArray<FColor> Pixels;
                Pixels.SetNum(W * H);
                {
                    FTexture2DMipMap& SMip = RealTex->GetPlatformData()->Mips[0];
                    FColor* Src = (FColor*)SMip.BulkData.Lock(LOCK_READ_ONLY);
                    if (Src)
                    {
                        FMemory::Memcpy(Pixels.GetData(), Src, W * H * sizeof(FColor));
                        SMip.BulkData.Unlock();
                    }
                }
                UTexture2D* Tex = UTexture2D::CreateTransient(W, H, PF_B8G8R8A8);
                if (Tex)
                {
                    Tex->SRGB = true;
                    Tex->CompressionSettings = TC_Default;
                    Tex->NeverStream = true;
                    FTexture2DMipMap& Mip = Tex->GetPlatformData()->Mips[0];
                    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
                    FMemory::Memcpy(Data, Pixels.GetData(), W * H * sizeof(FColor));
                    Mip.BulkData.Unlock();
                    Tex->UpdateResource();
                    return Tex;
                }
            }
            return RealTex;
        }
    }
    const FString FallbackPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Content/UI/earth_texture.png"));
    if (FPaths::FileExists(FallbackPath))
        return FImageUtils::ImportFileAsTexture2D(FallbackPath);
    return nullptr;
}

void UEOMainMenuWidget::FillEarthPixels(TArray<FColor>& Pixels, int32 W, int32 H)
{
    // 简化大陆块定义（cx, cy 归一化，rx, ry 为半径）
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

            // 海洋：纬度渐变（两极偏白，赤道偏蓝）
            float Polar = FMath::Abs(ny - 0.5f) * 2.0f;
            FLinearColor Ocean(
                0.02f + 0.05f * Polar,
                0.15f + 0.12f * Polar,
                0.40f + 0.18f * (1.0f - Polar),
                1.0f);

            // 大陆判定
            float LandMask = 0.0f;
            for (int32 i = 0; i < NumLand; i++)
            {
                float dx = (nx - LandBlocks[i].cx) / LandBlocks[i].rx;
                float dy = (ny - LandBlocks[i].cy) / LandBlocks[i].ry;
                float Dist = FMath::Sqrt(dx * dx + dy * dy);
                // 带边缘羽化与抖动
                float Edge = FMath::Clamp(1.0f - Dist, 0.0f, 1.0f);
                // 抖动让海岸不规则
                float Jitter = FMath::Sin(nx * 40.0f + ny * 33.0f + i * 7.0f) * 0.12f;
                float Mask = FMath::Clamp(Edge - 0.55f + Jitter, 0.0f, 1.0f);
                LandMask = FMath::Max(LandMask, Mask);
            }

            // 大陆：随纬度渐变颜色（低纬深绿，高纬浅棕）
            FLinearColor Land(
                0.16f + 0.10f * Polar,
                0.45f - 0.10f * Polar,
                0.20f - 0.06f * Polar,
                1.0f);

            FLinearColor Col = Ocean * (1.0f - LandMask) + Land * LandMask;

            // 云层：伪随机白色半透明点
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
    // 深蓝→深紫垂直渐变 + 白色星星点
    FRandomStream Rng(20261024);
    const FLinearColor Top(0.07f, 0.11f, 0.26f, 1.0f);
    const FLinearColor Bot(0.14f, 0.07f, 0.24f, 1.0f);
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
