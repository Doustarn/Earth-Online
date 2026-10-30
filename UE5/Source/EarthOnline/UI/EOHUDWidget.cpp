#include "EOHUDWidget.h"
#include "EOMobilePhoneWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Styling/CoreStyle.h"
#include "Misc/Paths.h"
#include "Engine/Texture2D.h"
#include "Blueprint/WidgetTree.h"
#include "../Character/EOBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

bool UEOHUDWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    UE_LOG(LogTemp, Warning, TEXT("[EOHUDWidget] Initialize 被调用，动态构建HUD元素"));

    NotificationDuration = 5.0f;

    // 必须在 Initialize（Slate 构建之前）设置 RootWidget 并构建控件，
    // NativeConstruct 在 Slate 树构建后才调用，此时改 RootWidget 不会生效，HUD 将永远不渲染
    BuildMainHUD();
    UE_LOG(LogTemp, Warning, TEXT("[EOHUDWidget] Initialize 构建完成，RootWidget=%s"),
        WidgetTree && WidgetTree->RootWidget ? TEXT("已设置") : TEXT("为空"));

    return true;
}

void UEOHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("[EOHUDWidget] NativeConstruct 被调用，开始构建HUD"));

    bJoystickPressed = false;
    SelectedSlot = 0;
    bSprinting = false;
    bAlertActive = false;
    CurrentInteractTarget = nullptr;

    HotbarItems = { TEXT("草"), TEXT("土"), TEXT("石"), TEXT("木"), TEXT("板"), TEXT("台"), TEXT("炉"), TEXT("炬"), TEXT("麦") };

    // 创建手机虚拟机（独立层，默认收起）
    PhoneWidget = CreateWidget<UEOMobilePhoneWidget>(GetWorld(), UEOMobilePhoneWidget::StaticClass());
    if (PhoneWidget)
    {
        PhoneWidget->AddToViewport(10);
    }

    // 首次进入显示新手教程
    if (TutorialPanel && !bTutorialShown)
    {
        TutorialPanel->SetVisibility(ESlateVisibility::Visible);
        bTutorialShown = true;
    }
}

void UEOHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateHUD(InDeltaTime);
    UpdateInteraction(InDeltaTime);
}

// ================================================================ 构建主界面

void UEOHUDWidget::BuildMainHUD()
{
    // RootWidget 必须用 WidgetTree->ConstructWidget 创建（注册进 WidgetTree，Slate 才能渲染），
    // 且要先设 RootWidget 再加子控件。NewObject 创建 + 后设 RootWidget 会导致 HUD 永远不渲染
    MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    if (WidgetTree)
    {
        WidgetTree->RootWidget = MainCanvas;
    }

    BuildActionArea(MainCanvas);
    BuildStatusArea(MainCanvas);
    BuildMiniMap(MainCanvas);
    BuildTopRightBar(MainCanvas);
    BuildHotbar(MainCanvas);
    BuildInteractionArea(MainCanvas);
    BuildTutorial(MainCanvas);
    BuildAlert(MainCanvas);
    BuildCultivationBar(MainCanvas);
    BuildSkillBar(MainCanvas);
}

void UEOHUDWidget::BuildActionArea(UCanvasPanel* Canvas)
{
    // —— 移动虚拟摇杆（右下角）——
    JoystickButton = NewObject<UButton>(this);
    JoystickButton->SetBackgroundColor(FLinearColor(0.15f, 0.15f, 0.18f, 0.55f));
    JoystickButton->OnPressed.AddDynamic(this, &UEOHUDWidget::OnJoystickPressed);
    JoystickButton->OnReleased.AddDynamic(this, &UEOHUDWidget::OnJoystickReleased);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(JoystickButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-205.0f, -205.0f));
        S->SetSize(FVector2D(160.0f, 160.0f));
        S->SetZOrder(2);
    }

    // 摇杆头
    UCanvasPanel* JoystickInner = NewObject<UCanvasPanel>(this);
    JoystickInner->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    JoystickButton->AddChild(JoystickInner);

    JoystickThumb = NewObject<UImage>(this);
    JoystickThumb->SetColorAndOpacity(FLinearColor(0.85f, 0.85f, 0.9f, 0.8f));
    JoystickThumbSlot = JoystickInner->AddChildToCanvas(JoystickThumb);
    if (JoystickThumbSlot)
    {
        JoystickThumbSlot->SetAnchors(FAnchors(0.0f));
        JoystickThumbSlot->SetPosition(FVector2D(50.0f, 50.0f));
        JoystickThumbSlot->SetSize(FVector2D(60.0f, 60.0f));
    }

    // —— 跳跃 ——
    JumpButton = MakeButton(TEXT("跳"), 20.0f);
    StyleRoundedButton(JumpButton, FLinearColor(0.10f, 0.42f, 0.26f, 0.62f));
    JumpButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnJumpClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(JumpButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-60.0f, -85.0f));
        S->SetSize(FVector2D(95.0f, 95.0f));
        S->SetZOrder(3);
    }

    // —— 冲刺 ——
    SprintButton = MakeButton(TEXT("跑"), 16.0f);
    StyleRoundedButton(SprintButton, FLinearColor(0.42f, 0.36f, 0.10f, 0.62f));
    SprintButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnSprintClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(SprintButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-175.0f, -65.0f));
        S->SetSize(FVector2D(72.0f, 72.0f));
        S->SetZOrder(3);
    }

    // —— 蹲下 ——
    CrouchButton = MakeButton(TEXT("蹲"), 16.0f);
    StyleRoundedButton(CrouchButton, FLinearColor(0.14f, 0.20f, 0.28f, 0.62f));
    CrouchButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnCrouchClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(CrouchButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-65.0f, -205.0f));
        S->SetSize(FVector2D(72.0f, 72.0f));
        S->SetZOrder(3);
    }

    // —— 交互 ——
    InteractButton = MakeButton(TEXT("手"), 16.0f);
    StyleRoundedButton(InteractButton, FLinearColor(0.14f, 0.30f, 0.55f, 0.62f));
    InteractButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnInteractClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(InteractButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-170.0f, -190.0f));
        S->SetSize(FVector2D(72.0f, 72.0f));
        S->SetZOrder(3);
    }

    // —— 手机入口（右下角上方）——
    PhoneButton = MakeButton(TEXT("📱"), 22.0f);
    PhoneButton->SetBackgroundColor(FLinearColor(0.2f, 0.35f, 0.6f, 0.85f));
    PhoneButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnPhoneClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(PhoneButton))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetAlignment(FVector2D(1.0f, 1.0f));
        S->SetPosition(FVector2D(-70.0f, -300.0f));
        S->SetSize(FVector2D(70.0f, 70.0f));
        S->SetZOrder(4);
    }
}

void UEOHUDWidget::BuildStatusArea(UCanvasPanel* Canvas)
{
    // 左下角身体状态面板（无血条，纯数值/状态文字）
    UBorder* Panel = NewObject<UBorder>(this);
    Panel->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f));
    Panel->SetPadding(FMargin(12.0f, 8.0f, 12.0f, 8.0f));
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(Panel))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetAlignment(FVector2D(0.0f, 0.0f));
        S->SetPosition(FVector2D(18.0f, 18.0f));
        S->SetAutoSize(true);
        S->SetZOrder(2);
    }

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    Panel->SetContent(VBox);

    UTextBlock* Title = MakeText(TEXT("· 身体状态 ·"), 14.0f, FLinearColor(0.85f, 0.9f, 0.95f, 1.0f));
    if (UVerticalBoxSlot* TS = VBox->AddChildToVerticalBox(Title))
    {
        TS->SetHorizontalAlignment(HAlign_Center);
        TS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
    }

    HealthValueText = MakeText(TEXT("健康 100/100"), 15.0f, FLinearColor(0.4f, 0.9f, 0.5f, 1.0f));
    if (UVerticalBoxSlot* HS = VBox->AddChildToVerticalBox(HealthValueText))
    {
        HS->SetPadding(FMargin(0.0f, 2.0f));
    }

    StaminaValueText = MakeText(TEXT("体力 100/100"), 15.0f, FLinearColor(0.4f, 0.75f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* SS = VBox->AddChildToVerticalBox(StaminaValueText))
    {
        SS->SetPadding(FMargin(0.0f, 2.0f));
    }

    LevelValueText = MakeText(TEXT("等级 1"), 15.0f, FLinearColor(1.0f, 0.85f, 0.4f, 1.0f));
    if (UVerticalBoxSlot* LS = VBox->AddChildToVerticalBox(LevelValueText))
    {
        LS->SetPadding(FMargin(0.0f, 2.0f));
    }

    PosValueText = MakeText(TEXT("坐标 0, 0, 0"), 13.0f, FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
    if (UVerticalBoxSlot* PS = VBox->AddChildToVerticalBox(PosValueText))
    {
        PS->SetPadding(FMargin(0.0f, 2.0f, 0.0f, 0.0f));
    }
}

// ================================================================ 左上：小地图 + 设置

void UEOHUDWidget::BuildMiniMap(UCanvasPanel* Canvas)
{
    // —— 左上设置按钮 ——
    UButton* SettingsBtn = MakeButton(TEXT("设置"), 13.0f);
    StyleRoundedButton(SettingsBtn, FLinearColor(0.08f, 0.14f, 0.26f, 0.72f));
    SettingsBtn->OnClicked.AddDynamic(this, &UEOHUDWidget::OnPhoneClicked);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(SettingsBtn))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetAlignment(FVector2D(0.0f, 0.0f));
        S->SetPosition(FVector2D(18.0f, 18.0f));
        S->SetSize(FVector2D(64.0f, 34.0f));
        S->SetZOrder(4);
    }

    // —— 小地图框（半透明深色，含地形色块示意）——
    UBorder* MapBox = NewObject<UBorder>(this);
    MapBox->SetBrushColor(FLinearColor(0.04f, 0.08f, 0.15f, 0.78f));
    MapBox->SetPadding(FMargin(6.0f));
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(MapBox))
    {
        S->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
        S->SetAlignment(FVector2D(0.0f, 0.0f));
        S->SetPosition(FVector2D(18.0f, 62.0f));
        S->SetSize(FVector2D(150.0f, 150.0f));
        S->SetZOrder(4);
    }

    UCanvasPanel* MapInner = NewObject<UCanvasPanel>(this);
    MapBox->SetContent(MapInner);

    auto AddMapBlock = [&](FLinearColor Color, FVector2D Pos, FVector2D Size)
    {
        UImage* Block = NewObject<UImage>(this);
        Block->SetColorAndOpacity(Color);
        if (UCanvasPanelSlot* S = MapInner->AddChildToCanvas(Block))
        {
            S->SetAnchors(FAnchors(0.0f));
            S->SetPosition(Pos);
            S->SetSize(Size);
        }
    };

    // 地形色块示意：陆地（绿）/ 水域（蓝）/ 道路（灰）
    AddMapBlock(FLinearColor(0.20f, 0.45f, 0.25f, 0.8f), FVector2D(0, 0), FVector2D(70, 60));   // 左陆地
    AddMapBlock(FLinearColor(0.18f, 0.42f, 0.50f, 0.8f), FVector2D(72, 0), FVector2D(66, 50));   // 水域
    AddMapBlock(FLinearColor(0.24f, 0.50f, 0.28f, 0.8f), FVector2D(0, 62), FVector2D(138, 30));  // 下陆地
    AddMapBlock(FLinearColor(0.35f, 0.35f, 0.35f, 0.9f), FVector2D(66, 90), FVector2D(14, 48));  // 道路
    AddMapBlock(FLinearColor(0.30f, 0.55f, 0.35f, 0.9f), FVector2D(82, 100), FVector2D(56, 38)); // 森林

    // 玩家定位点（亮白点）
    UImage* PlayerDot = NewObject<UImage>(this);
    PlayerDot->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    if (UCanvasPanelSlot* S = MapInner->AddChildToCanvas(PlayerDot))
    {
        S->SetAnchors(FAnchors(0.0f));
        S->SetPosition(FVector2D(68.0f, 68.0f));
        S->SetSize(FVector2D(10.0f, 10.0f));
    }
}

// ================================================================ 右上：功能图标排

void UEOHUDWidget::BuildTopRightBar(UCanvasPanel* Canvas)
{
    UHorizontalBox* HBox = NewObject<UHorizontalBox>(this);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(HBox))
    {
        S->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
        S->SetAlignment(FVector2D(1.0f, 0.0f));
        S->SetPosition(FVector2D(-18.0f, 18.0f));
        S->SetAutoSize(true);
        S->SetZOrder(4);
    }

    auto AddTopBtn = [&](const FString& Label, UButton*& OutBtn)
    {
        UButton* B = MakeButton(Label, 13.0f);
        StyleRoundedButton(B, FLinearColor(0.08f, 0.14f, 0.26f, 0.72f));
        B->OnClicked.AddDynamic(this, &UEOHUDWidget::OnPhoneClicked);
        if (UHorizontalBoxSlot* HS = HBox->AddChildToHorizontalBox(B))
        {
            HS->SetPadding(FMargin(4.0f));
        }
        OutBtn = B;
    };

    UButton* BagBtn = nullptr;
    UButton* MailBtn = nullptr;
    UButton* QuestBtn = nullptr;
    UButton* RefBtn = nullptr;
    AddTopBtn(TEXT("包"), BagBtn);
    AddTopBtn(TEXT("邮"), MailBtn);
    AddTopBtn(TEXT("任"), QuestBtn);
    AddTopBtn(TEXT("更"), RefBtn);
}

void UEOHUDWidget::BuildHotbar(UCanvasPanel* Canvas)
{
    // 中下物品栏（迷你世界风格 9 格）
    UHorizontalBox* HBox = NewObject<UHorizontalBox>(this);
    HBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(HBox))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -26.0f));
        S->SetAutoSize(true);
        S->SetZOrder(2);
    }

    for (int32 i = 0; i < 9; ++i)
    {
        USizeBox* Box = NewObject<USizeBox>(this);
        Box->SetWidthOverride(58.0f);
        Box->SetHeightOverride(58.0f);
        if (UHorizontalBoxSlot* BS = HBox->AddChildToHorizontalBox(Box))
        {
            BS->SetPadding(FMargin(3.0f));
        }

        UButton* Btn = NewObject<UButton>(this);
        Btn->SetBackgroundColor(FLinearColor(0.06f, 0.10f, 0.18f, 0.72f));
        Btn->OnClicked.AddDynamic(this, &UEOHUDWidget::OnSlotClicked);
        Box->AddChild(Btn);

        FString ItemName = (i < HotbarItems.Num()) ? HotbarItems[i] : TEXT("?");
        UTextBlock* Label = MakeText(ItemName, 15.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        Btn->AddChild(Label);

        HotbarButtons.Add(Btn);
        HotbarLabelTexts.Add(Label);
    }
}

// ================================================================ 每帧更新

void UEOHUDWidget::UpdateHUD(float DeltaTime)
{
    AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player) return;

    // 状态区（无血条，数值显示）
    if (HealthValueText)
    {
        HealthValueText->SetText(FText::FromString(
            FString::Printf(TEXT("健康 %d/%d"), FMath::CeilToInt(Player->Health), FMath::CeilToInt(Player->MaxHealth))));
    }
    if (StaminaValueText)
    {
        StaminaValueText->SetText(FText::FromString(
            FString::Printf(TEXT("体力 %d/%d"), FMath::CeilToInt(Player->Stamina), FMath::CeilToInt(Player->MaxStamina))));
    }
    if (LevelValueText)
    {
        LevelValueText->SetText(FText::FromString(FString::Printf(TEXT("等级 %d"), Player->PlayerLevel)));
    }
    if (PosValueText)
    {
        FVector Loc = Player->GetActorLocation();
        PosValueText->SetText(FText::FromString(
            FString::Printf(TEXT("坐标 %.0f, %.0f, %.0f"), Loc.X, Loc.Y, Loc.Z)));
    }

    // 物品栏选中高亮
    for (int32 i = 0; i < HotbarButtons.Num(); ++i)
    {
        if (HotbarButtons[i])
        {
            HotbarButtons[i]->SetBackgroundColor(
                (i == SelectedSlot) ? FLinearColor(0.85f, 0.65f, 0.10f, 0.95f) : FLinearColor(0.06f, 0.10f, 0.18f, 0.72f));
        }
    }

    // 摇杆
    UpdateJoystick(DeltaTime);
}

void UEOHUDWidget::UpdateJoystick(float DeltaTime)
{
    // 手机打开时不响应摇杆
    if (PhoneWidget && PhoneWidget->IsPhoneOpen())
    {
        return;
    }

    if (!bJoystickPressed) return;

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    FVector2D MousePos;
    if (!PC->GetMousePosition(MousePos.X, MousePos.Y)) return;

    FVector2D Delta = MousePos - JoystickPressPos;
    const float MaxRadius = 55.0f;
    if (Delta.Size() > MaxRadius)
    {
        Delta = Delta.GetClampedToMaxSize(MaxRadius);
    }

    // 摇杆头跟随
    if (JoystickThumbSlot)
    {
        JoystickThumbSlot->SetPosition(FVector2D(50.0f, 50.0f) + Delta);
    }

    // 移动输入
    AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        FVector2D Dir = Delta / MaxRadius;
        FVector MoveVec = Player->GetActorForwardVector() * (-Dir.Y) + Player->GetActorRightVector() * Dir.X;
        Player->AddMovementInput(MoveVec, 1.0f);
    }
}

// ================================================================ 回调

void UEOHUDWidget::OnJoystickPressed()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    FVector2D MousePos;
    if (PC->GetMousePosition(MousePos.X, MousePos.Y))
    {
        JoystickPressPos = MousePos;
    }
    bJoystickPressed = true;
}

void UEOHUDWidget::OnJoystickReleased()
{
    bJoystickPressed = false;
    if (JoystickThumbSlot)
    {
        JoystickThumbSlot->SetPosition(FVector2D(50.0f, 50.0f));
    }
}

void UEOHUDWidget::OnJumpClicked()
{
    if (AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->Jump();
    }
}

void UEOHUDWidget::OnSprintClicked()
{
    if (AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();
        if (MoveComp)
        {
            bSprinting = !bSprinting;
            MoveComp->MaxWalkSpeed = bSprinting ? 600.0f : 300.0f;
        }
    }
}

void UEOHUDWidget::OnCrouchClicked()
{
    if (AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        if (Player->bIsCrouched)
        {
            Player->UnCrouch();
        }
        else
        {
            Player->Crouch();
        }
    }
}

void UEOHUDWidget::OnInteractClicked()
{
    ShowNotification(TEXT("交互功能开发中"));
}

void UEOHUDWidget::OnPhoneClicked()
{
    if (PhoneWidget)
    {
        if (PhoneWidget->IsPhoneOpen())
        {
            PhoneWidget->ClosePhone();
        }
        else
        {
            PhoneWidget->OpenPhone();
        }
    }
}

void UEOHUDWidget::OnSlotClicked()
{
    for (int32 i = 0; i < HotbarButtons.Num(); ++i)
    {
        if (HotbarButtons[i] && HotbarButtons[i]->IsHovered())
        {
            SelectedSlot = i;
            ShowNotification(FString::Printf(TEXT("选中物品：%s"), *HotbarItems[i]));
            break;
        }
    }
}

// ================================================================ 工具

UButton* UEOHUDWidget::MakeButton(const FString& Content, float FontSize)
{
    UButton* Btn = NewObject<UButton>(this);
    Btn->SetVisibility(ESlateVisibility::Visible);
    UTextBlock* Label = MakeText(Content, FontSize, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    Btn->AddChild(Label);
    return Btn;
}

UTextBlock* UEOHUDWidget::MakeText(const FString& Content, float FontSize, const FLinearColor& Color)
{
    UTextBlock* Text = NewObject<UTextBlock>(this);
    Text->SetText(FText::FromString(Content));
    // 用中文字体（DroidSansFallback 含中文与基础拉丁），避免中文显示成"? "
    Text->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), FontSize));
    Text->SetColorAndOpacity(FSlateColor(Color));
    return Text;
}

UTexture2D* UEOHUDWidget::CreateRoundedTexture(int32 Size, float CornerRadius, const FLinearColor& Color)
{
    UTexture2D* Tex = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8);
    if (!Tex)
    {
        return nullptr;
    }
    Tex->SRGB = false;
    Tex->AddressX = TA_Clamp;
    Tex->AddressY = TA_Clamp;
    Tex->Filter = TF_Bilinear;

    FTexture2DMipMap& Mip = Tex->GetPlatformData()->Mips[0];
    uint8* Pixels = (uint8*)Mip.BulkData.Lock(LOCK_READ_WRITE);

    const float MaxR = FMath::Max(0.5f, CornerRadius);
    for (int32 Y = 0; Y < Size; ++Y)
    {
        for (int32 X = 0; X < Size; ++X)
        {
            float EdgeX = FMath::Min((float)X, (float)(Size - 1 - X));
            float EdgeY = FMath::Min((float)Y, (float)(Size - 1 - Y));
            float cx = FMath::Max(0.0f, EdgeX - (Size - MaxR));
            float cy = FMath::Max(0.0f, EdgeY - (Size - MaxR));
            float d = FMath::Sqrt(cx * cx + cy * cy);
            float Alpha = (d > MaxR) ? 0.0f : 1.0f;
            if (d > MaxR - 1.0f)
            {
                Alpha = FMath::Clamp(MaxR - d, 0.0f, 1.0f);
            }
            Alpha *= Color.A;

            int32 I = (Y * Size + X) * 4;
            Pixels[I + 0] = (uint8)(FMath::Clamp(Color.B, 0.0f, 1.0f) * 255.0f);
            Pixels[I + 1] = (uint8)(FMath::Clamp(Color.G, 0.0f, 1.0f) * 255.0f);
            Pixels[I + 2] = (uint8)(FMath::Clamp(Color.R, 0.0f, 1.0f) * 255.0f);
            Pixels[I + 3] = (uint8)(FMath::Clamp(Alpha, 0.0f, 1.0f) * 255.0f);
        }
    }

    Mip.BulkData.Unlock();
    Tex->UpdateResource();
    return Tex;
}

void UEOHUDWidget::StyleRoundedButton(UButton* Btn, const FLinearColor& Color, float Radius)
{
    if (!Btn)
    {
        return;
    }
    UTexture2D* Tex = CreateRoundedTexture(64, Radius, Color);
    if (!Tex)
    {
        return;
    }
    FSlateBrush Brush;
    Brush.SetResourceObject(Tex);
    Brush.ImageSize = FVector2D(64.0f, 64.0f);
    Btn->WidgetStyle.SetNormal(Brush);
    Btn->WidgetStyle.SetHovered(Brush);
    Btn->WidgetStyle.SetPressed(Brush);
}

void UEOHUDWidget::ShowNotification(const FString& Message)
{
    if (NotificationList)
    {
        UTextBlock* NewNote = NewObject<UTextBlock>(this);
        if (NewNote)
        {
            NewNote->SetText(FText::FromString(Message));
            NewNote->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
            NotificationList->AddChildToVerticalBox(NewNote);

            PendingNotifications.Add(Message);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[通知] %s"), *Message);
}

// ================================================================ 交互提示区

void UEOHUDWidget::BuildInteractionArea(UCanvasPanel* Canvas)
{
    // 屏幕中央偏下的交互提示（"F 开门"）
    InteractPromptText = MakeText(TEXT(""), 22.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    InteractPromptText->SetJustification(ETextJustify::Center);
    InteractPromptText->SetVisibility(ESlateVisibility::Collapsed);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(InteractPromptText))
    {
        S->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        S->SetAlignment(FVector2D(0.5f, 1.0f));
        S->SetPosition(FVector2D(0.0f, -140.0f));
        S->SetSize(FVector2D(700.0f, 40.0f));
        S->SetZOrder(8);
    }
}

void UEOHUDWidget::ShowInteractionPrompt(const FString& Label)
{
    if (InteractPromptText)
    {
        InteractPromptText->SetText(FText::FromString(Label));
        InteractPromptText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UEOHUDWidget::HideInteractionPrompt()
{
    if (InteractPromptText)
    {
        InteractPromptText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UEOHUDWidget::UpdateInteraction(float DeltaTime)
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        HideInteractionPrompt();
        return;
    }

    // 从玩家视角中心发射射线检测可交互物（Tag "Interactable"）
    AActor* Target = nullptr;
    FString InteractLabel = TEXT("");
    FHitResult Hit;
    const FVector Start = PC->PlayerCameraManager ? PC->PlayerCameraManager->GetCameraLocation() : PC->GetPawn()->GetActorLocation();
    const FVector End = Start + (PC->PlayerCameraManager ? PC->PlayerCameraManager->GetCameraRotation().Vector() : PC->GetControlRotation().Vector()) * 300.0f;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->ActorHasTag(TEXT("Interactable")))
        {
            Target = HitActor;
            // 交互标签：优先用 Actor 名称，简单友好化
            FString ActorName = HitActor->GetName();
            if (ActorName.Contains(TEXT("门"))) InteractLabel = TEXT("开门");
            else if (ActorName.Contains(TEXT("衣"))) InteractLabel = TEXT("穿衣服");
            else if (ActorName.Contains(TEXT("箱"))) InteractLabel = TEXT("打开箱子");
            else InteractLabel = TEXT("交互");
        }
    }

    if (Target && !InteractLabel.IsEmpty())
    {
        CurrentInteractTarget = Target;
        ShowInteractionPrompt(FString::Printf(TEXT("F  %s"), *InteractLabel));
    }
    else
    {
        CurrentInteractTarget = nullptr;
        HideInteractionPrompt();
    }
}

void UEOHUDWidget::PerformInteraction()
{
    if (!CurrentInteractTarget) return;

    // 简单交互：旋转可交互物（门开合）示意
    if (USceneComponent* Root = CurrentInteractTarget->GetRootComponent())
    {
        FRotator R = Root->GetComponentRotation();
        R.Yaw += 90.0f;
        Root->SetWorldRotation(R);
        ShowNotification(TEXT("已交互"));
    }
    CurrentInteractTarget = nullptr;
    HideInteractionPrompt();
}

// ================================================================ 新手教程

void UEOHUDWidget::BuildTutorial(UCanvasPanel* Canvas)
{
    TutorialPanel = NewObject<UBorder>(this);
    TutorialPanel->SetBrushColor(FLinearColor(0.03f, 0.05f, 0.1f, 0.88f));
    TutorialPanel->SetPadding(FMargin(40.0f, 32.0f, 40.0f, 32.0f));
    TutorialPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(TutorialPanel))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, 0.0f));
        S->SetAutoSize(true);
        S->SetZOrder(40);
    }

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    TutorialPanel->SetContent(VBox);

    UTextBlock* Title = MakeText(TEXT("· 新手教程 ·"), 26.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    Title->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* TS = VBox->AddChildToVerticalBox(Title))
    {
        TS->SetHorizontalAlignment(HAlign_Fill);
        TS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
    }

    UTextBlock* Content = MakeText(
        TEXT("W / A / S / D  移动\n鼠标   视角\n空格   跳跃\nShift   奔跑\nC   蹲下\nF   交互\n手机   随身系统"),
        18.0f, FLinearColor(0.85f, 0.9f, 0.95f, 1.0f));
    Content->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* CS = VBox->AddChildToVerticalBox(Content))
    {
        CS->SetHorizontalAlignment(HAlign_Fill);
        CS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));
    }

    TutorialCloseButton = MakeButton(TEXT("知道了，开始探索"), 18.0f);
    TutorialCloseButton->SetBackgroundColor(FLinearColor(0.15f, 0.5f, 0.3f, 0.95f));
    TutorialCloseButton->OnClicked.AddDynamic(this, &UEOHUDWidget::OnTutorialCloseClicked);
    if (UVerticalBoxSlot* B = VBox->AddChildToVerticalBox(TutorialCloseButton))
    {
        B->SetHorizontalAlignment(HAlign_Center);
    }
}

void UEOHUDWidget::HideTutorial()
{
    if (TutorialPanel) TutorialPanel->SetVisibility(ESlateVisibility::Collapsed);
    bTutorialShown = true;
}

void UEOHUDWidget::OnTutorialCloseClicked()
{
    HideTutorial();
}

// ================================================================ 警报弹窗

void UEOHUDWidget::BuildAlert(UCanvasPanel* Canvas)
{
    AlertPanel = NewObject<UBorder>(this);
    AlertPanel->SetBrushColor(FLinearColor(0.1f, 0.02f, 0.02f, 0.9f));
    AlertPanel->SetPadding(FMargin(36.0f, 28.0f, 36.0f, 28.0f));
    AlertPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(AlertPanel))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetPosition(FVector2D(0.0f, 0.0f));
        S->SetAutoSize(true);
        S->SetZOrder(45);
    }

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    AlertPanel->SetContent(VBox);

    AlertTitleText = MakeText(TEXT("警示"), 26.0f, FLinearColor(1.0f, 0.4f, 0.3f, 1.0f));
    AlertTitleText->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* TS = VBox->AddChildToVerticalBox(AlertTitleText))
    {
        TS->SetHorizontalAlignment(HAlign_Fill);
        TS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
    }

    AlertBodyText = MakeText(TEXT(""), 18.0f, FLinearColor(0.95f, 0.9f, 0.9f, 1.0f));
    AlertBodyText->SetJustification(ETextJustify::Center);
    if (UVerticalBoxSlot* BS = VBox->AddChildToVerticalBox(AlertBodyText))
    {
        BS->SetHorizontalAlignment(HAlign_Fill);
        BS->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));
    }

    UHorizontalBox* BtnRow = NewObject<UHorizontalBox>(this);
    if (UVerticalBoxSlot* R = VBox->AddChildToVerticalBox(BtnRow))
    {
        R->SetHorizontalAlignment(HAlign_Center);
    }

    AlertBtn1 = MakeButton(TEXT("交战"), 18.0f);
    AlertBtn1->SetBackgroundColor(FLinearColor(0.55f, 0.2f, 0.2f, 0.95f));
    AlertBtn1->OnClicked.AddDynamic(this, &UEOHUDWidget::OnAlertBtn1Clicked);
    if (UHorizontalBoxSlot* S1 = BtnRow->AddChildToHorizontalBox(AlertBtn1))
    {
        S1->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    }

    AlertBtn2 = MakeButton(TEXT("撤退"), 18.0f);
    AlertBtn2->SetBackgroundColor(FLinearColor(0.2f, 0.35f, 0.55f, 0.95f));
    AlertBtn2->OnClicked.AddDynamic(this, &UEOHUDWidget::OnAlertBtn2Clicked);
    if (UHorizontalBoxSlot* S2 = BtnRow->AddChildToHorizontalBox(AlertBtn2))
    {
        S2->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
    }
}

void UEOHUDWidget::ShowAlert(const FString& Title, const FString& Body, const FString& Btn1, const FString& Btn2)
{
    if (AlertPanel)
    {
        AlertPanel->SetVisibility(ESlateVisibility::Visible);
        if (AlertTitleText) AlertTitleText->SetText(FText::FromString(Title));
        if (AlertBodyText) AlertBodyText->SetText(FText::FromString(Body));
        if (AlertBtn1)
        {
            UTextBlock* L1 = NewObject<UTextBlock>(this);
            L1->SetText(FText::FromString(Btn1));
            L1->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 18));
            L1->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            AlertBtn1->SetContent(L1);
        }
        if (AlertBtn2)
        {
            UTextBlock* L2 = NewObject<UTextBlock>(this);
            L2->SetText(FText::FromString(Btn2));
            L2->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 18));
            L2->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            AlertBtn2->SetContent(L2);
        }
        bAlertActive = true;
    }
}

void UEOHUDWidget::HideAlert()
{
    if (AlertPanel) AlertPanel->SetVisibility(ESlateVisibility::Collapsed);
    bAlertActive = false;
}

void UEOHUDWidget::OnAlertBtn1Clicked()
{
    HideAlert();
    ShowNotification(TEXT("选择：交战"));
}

void UEOHUDWidget::OnAlertBtn2Clicked()
{
    HideAlert();
    ShowNotification(TEXT("选择：撤退"));
}

// ============================================================ 修仙 HUD

void UEOHUDWidget::BuildCultivationBar(UCanvasPanel* Canvas)
{
    // 左下角：境界 + 等级 x/10 + 经验条 + 灵石
    UVerticalBox* Box = NewObject<UVerticalBox>(this);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(Box))
    {
        S->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        S->SetPosition(FVector2D(20.0f, -160.0f));
        S->SetAutoSize(true);
        S->SetZOrder(10);
    }

    // 境界名
    RealmText = MakeText(TEXT("炼气期 · Qi Refining"), 18.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* RS = Box->AddChildToVerticalBox(RealmText)) RS->SetPadding(FMargin(0, 0, 0, 2));

    // 等级 x/10
    LevelText = MakeText(TEXT("第 3 层 / 共 10 层"), 14.0f, FLinearColor(0.8f, 0.85f, 0.9f, 0.9f));
    if (UVerticalBoxSlot* LS = Box->AddChildToVerticalBox(LevelText)) LS->SetPadding(FMargin(0, 0, 0, 4));

    // 经验条
    ExpBar = NewObject<UProgressBar>(this);
    ExpBar->SetPercent(0.3f);
    ExpBar->SetFillColorAndOpacity(FLinearColor(0.15f, 0.75f, 0.85f, 1.0f));
    ExpBar->SetBackgroundColor(FLinearColor(0.1f, 0.1f, 0.15f, 0.6f));
    USizeBox* ExpSize = NewObject<USizeBox>(this);
    ExpSize->SetWidthOverride(280.0f);
    ExpSize->SetHeightOverride(12.0f);
    ExpSize->AddChild(ExpBar);
    if (UVerticalBoxSlot* ES = Box->AddChildToVerticalBox(ExpSize))
    {
        ES->SetHorizontalAlignment(HAlign_Fill);
        ES->SetPadding(FMargin(0, 4, 0, 4));
    }

    // 灵石
    SpiritStoneText = MakeText(TEXT("灵石: 128 下品"), 14.0f, FLinearColor(0.6f, 0.85f, 1.0f, 0.95f));
    Box->AddChildToVerticalBox(SpiritStoneText);
}

void UEOHUDWidget::BuildSkillBar(UCanvasPanel* Canvas)
{
    // 右下角：Q/W/E/R 四个技能
    UHorizontalBox* Row = NewObject<UHorizontalBox>(this);
    if (UCanvasPanelSlot* S = Canvas->AddChildToCanvas(Row))
    {
        S->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
        S->SetPosition(FVector2D(-260.0f, -140.0f));
        S->SetAutoSize(true);
        S->SetZOrder(10);
    }

    const TCHAR* SkillKeys[] = { TEXT("Q"), TEXT("W"), TEXT("E"), TEXT("R") };
    const TCHAR* SkillNames[] = { TEXT("普攻"), TEXT("御剑"), TEXT("万剑"), TEXT("闪避") };
    for (int32 i = 0; i < 4; i++)
    {
        UButton* SkillBtn = NewObject<UButton>(this);
        FButtonStyle BS = SkillBtn->WidgetStyle;
        BS.Normal.TintColor = FLinearColor(0.08f, 0.10f, 0.18f, 0.85f);
        BS.Hovered.TintColor = FLinearColor(0.15f, 0.75f, 0.85f, 0.5f);
        BS.Pressed.TintColor = FLinearColor(0.2f, 0.25f, 0.4f, 1.0f);
        SkillBtn->WidgetStyle = BS;

        UVerticalBox* VB = NewObject<UVerticalBox>(this);
        UTextBlock* KeyText = MakeText(FString(SkillKeys[i]), 16.0f, FLinearColor(1.0f, 0.9f, 0.6f, 1.0f));
        KeyText->SetJustification(ETextJustify::Center);
        VB->AddChildToVerticalBox(KeyText);
        UTextBlock* NameText = MakeText(FString(SkillNames[i]), 11.0f, FLinearColor(0.8f, 0.82f, 0.88f, 0.9f));
        NameText->SetJustification(ETextJustify::Center);
        VB->AddChildToVerticalBox(NameText);
        SkillBtn->SetContent(VB);
        SkillBtn->SetPadding(FMargin(10, 8, 10, 8));

        if (UHorizontalBoxSlot* HS = Row->AddChildToHorizontalBox(SkillBtn))
        {
            HS->SetPadding(FMargin(6, 0, 6, 0));
        }
        SkillButtons.Add(SkillBtn);
    }
}
