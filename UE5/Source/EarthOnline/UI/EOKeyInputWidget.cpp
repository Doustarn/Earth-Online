#include "EOKeyInputWidget.h"
#include "../Utils/EOLang.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "Styling/CoreStyle.h"

bool UEOKeyInputWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    GameState = 0;
    BuildUI();
    ShowState(0);
    return true;
}

void UEOKeyInputWidget::BuildUI()
{
    MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
    if (WidgetTree) WidgetTree->RootWidget = MainCanvas;
    BuildBackground();
}

void UEOKeyInputWidget::BuildBackground()
{
    UCanvasPanelSlot* S = nullptr;
    // 深色底
    UImage* Bg = NewObject<UImage>(this);
    Bg->SetColorAndOpacity(FLinearColor(0.04f, 0.05f, 0.10f, 1.0f));
    Bg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if ((S = MainCanvas->AddChildToCanvas(Bg)))
    {
        S->SetAnchors(FAnchors(0,0,1,1));
        S->SetOffsets(FMargin(0,0,0,0));
        S->SetZOrder(0);
    }
}

void UEOKeyInputWidget::ShowState(int32 NewState)
{
    GameState = NewState;
    if (MainCanvas)
    {
        // 清掉旧的动态子控件（保留背景 ZOrder=0）
        TArray<UWidget*> ToRemove;
        for (int32 i = 0; i < MainCanvas->GetChildrenCount(); i++)
        {
            UWidget* Child = MainCanvas->GetChildAt(i);
            if (!Child) continue;
            if (UCanvasPanelSlot* CPS = Cast<UCanvasPanelSlot>(Child->Slot))
            {
                if (CPS->GetZOrder() > 0) ToRemove.Add(Child);
            }
        }
        for (UWidget* W : ToRemove) MainCanvas->RemoveChild(W);
    }
    ClassButtons.Empty();
    ClassDescs.Empty();

    if (NewState == 0) ShowLoginState();
    else if (NewState == 1) ShowIdentityState();
    else if (NewState == 2) ShowClassState();
}

void UEOKeyInputWidget::ShowLoginState()
{
    UTextBlock* Title = MakeText(TEXT("IMMORTAL DAO ONLINE"), 40.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Title))
    {
        S->SetAnchors(FAnchors(0.5f, 0.3f, 0.5f, 0.3f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    UTextBlock* Sub = MakeText(TEXT("请输入你的道号"), 20.0f, FLinearColor(0.7f, 0.75f, 0.85f, 0.9f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Sub))
    {
        S->SetAnchors(FAnchors(0.5f, 0.45f, 0.5f, 0.45f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    // 输入框
    NameInput = NewObject<UEditableTextBox>(this);
    NameInput->SetHintText(FText::FromString(TEXT("输入道号...")));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(NameInput))
    {
        S->SetAnchors(FAnchors(0.5f, 0.52f, 0.5f, 0.52f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetSize(FVector2D(360.0f, 44.0f));
        S->SetZOrder(5);
    }

    ConfirmNameBtn = NewObject<UButton>(this);
    FButtonStyle BS = ConfirmNameBtn->WidgetStyle;
    BS.Normal.TintColor = FLinearColor(0.15f, 0.75f, 0.85f, 0.92f);
    BS.Hovered.TintColor = FLinearColor(0.30f, 0.92f, 1.0f, 1.0f);
    BS.Pressed.TintColor = FLinearColor(0.10f, 0.60f, 0.70f, 1.0f);
    ConfirmNameBtn->WidgetStyle = BS;
    UTextBlock* BtnLabel = NewObject<UTextBlock>(this);
    BtnLabel->SetText(FText::FromString(TEXT("进 入 仙 途")));
    BtnLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), 22));
    BtnLabel->SetColorAndOpacity(FLinearColor(0.02f, 0.08f, 0.10f, 1.0f));
    BtnLabel->SetJustification(ETextJustify::Center);
    ConfirmNameBtn->SetContent(BtnLabel);
    ConfirmNameBtn->SetPadding(FMargin(50.0f, 12.0f, 50.0f, 12.0f));
    ConfirmNameBtn->OnClicked.AddDynamic(this, &UEOKeyInputWidget::OnConfirmName);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(ConfirmNameBtn))
    {
        S->SetAnchors(FAnchors(0.5f, 0.62f, 0.5f, 0.62f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }
}

void UEOKeyInputWidget::ShowIdentityState()
{
    FString Identity = GenerateRandomIdentity();

    IdentityText = MakeText(Identity, 28.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(IdentityText))
    {
        S->SetAnchors(FAnchors(0.5f, 0.4f, 0.5f, 0.4f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    IdentitySub = MakeText(TEXT("前世记忆觉醒中..."), 16.0f, FLinearColor(0.6f, 0.65f, 0.75f, 0.8f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(IdentitySub))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    // 3秒后进职业选择（WeakObjectPtr 防止 widget 销毁后野指针）
    if (UWorld* W = GetWorld())
    {
        TWeakObjectPtr<UEOKeyInputWidget> WeakThis(this);
        W->GetTimerManager().SetTimer(IdentityTimer, [WeakThis]()
        {
            if (WeakThis.IsValid()) WeakThis->ShowState(2);
        }, 3.0f, false);
    }
}

void UEOKeyInputWidget::ShowClassState()
{
    UTextBlock* Title = MakeText(TEXT("选择你的修仙之路"), 32.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Title))
    {
        S->SetAnchors(FAnchors(0.5f, 0.15f, 0.5f, 0.15f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    // 5 职业卡片水平排列
    UHorizontalBox* Row = NewObject<UHorizontalBox>(this);
    if (UCanvasPanelSlot* S = MainCanvas->AddChildToCanvas(Row))
    {
        S->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        S->SetAlignment(FVector2D(0.5f, 0.5f));
        S->SetAutoSize(true);
        S->SetZOrder(5);
    }

    struct FClassInfo { const TCHAR* Name; const TCHAR* Desc; };
    static const FClassInfo Classes[] = {
        { TEXT("剑修"), TEXT("Sword\n近战单体\n御剑术·高暴击") },
        { TEXT("枪修"), TEXT("Spear\n近战突进\n横扫·破甲") },
        { TEXT("法修"), TEXT("Mage\n远程AOE\n雷火冰封") },
        { TEXT("丹修"), TEXT("Alchemist\n辅助治疗\n炼丹·回血") },
        { TEXT("符修"), TEXT("Talisman\n控场召唤\n画符·雷阵") },
    };

    for (int32 i = 0; i < 5; i++)
    {
        UButton* Card = MakeClassButton(Classes[i].Name, Classes[i].Desc, i);
        USizeBox* CardSize = NewObject<USizeBox>(this);
        CardSize->SetWidthOverride(160.0f);
        CardSize->SetHeightOverride(220.0f);
        CardSize->AddChild(Card);
        if (UHorizontalBoxSlot* HS = Row->AddChildToHorizontalBox(CardSize))
        {
            HS->SetPadding(FMargin(10.0f, 0.0f, 10.0f, 0.0f));
        }
    }
}

FString UEOKeyInputWidget::GenerateRandomIdentity()
{
    FRandomStream Rng(FDateTime::Now().GetTicks());
    static const TCHAR* Sects[] = { TEXT("青云宗"), TEXT("血刀门"), TEXT("太虚观"), TEXT("万佛洞"), TEXT("合欢谷"), TEXT("天煞盟") };
    static const TCHAR* Surnames[] = { TEXT("李"), TEXT("王"), TEXT("张"), TEXT("刘"), TEXT("陈"), TEXT("杨"), TEXT("赵"), TEXT("黄") };
    static const TCHAR* Given[] = { TEXT("逍遥"), TEXT("无忌"), TEXT("玄风"), TEXT("青云"), TEXT("无尘"), TEXT("傲雪"), TEXT("啸天"), TEXT("流云") };
    static const TCHAR* Realms[] = { TEXT("炼气3层"), TEXT("炼气5层"), TEXT("炼气7层"), TEXT("炼气9层"), TEXT("筑基初期"), TEXT("筑基中期"), TEXT("金丹期") };

    FString Sect = Sects[Rng.RandRange(0, 5)];
    FString Name = Surnames[Rng.RandRange(0, 7)] + FString(Given[Rng.RandRange(0, 7)]);
    // 80% 炼气, 15% 筑基, 5% 金丹
    int32 Roll = Rng.RandRange(0, 99);
    FString Realm;
    if (Roll < 60) Realm = Realms[Rng.RandRange(0, 3)];
    else if (Roll < 95) Realm = Realms[Rng.RandRange(4, 5)];
    else Realm = Realms[6];

    return FString::Printf(TEXT("你是 %s·%s · %s"), *Sect, *Name, *Realm);
}

UButton* UEOKeyInputWidget::MakeClassButton(const FString& Name, const FString& Desc, int32 Index)
{
    UButton* Btn = NewObject<UButton>(this);
    FButtonStyle BS = Btn->WidgetStyle;
    BS.Normal.TintColor = FLinearColor(0.08f, 0.10f, 0.18f, 0.9f);
    BS.Hovered.TintColor = FLinearColor(0.15f, 0.75f, 0.85f, 0.5f);
    BS.Pressed.TintColor = FLinearColor(0.12f, 0.15f, 0.25f, 1.0f);
    Btn->WidgetStyle = BS;

    UVerticalBox* VBox = NewObject<UVerticalBox>(this);
    UTextBlock* NameText = MakeText(Name, 26.0f, FLinearColor(0.30f, 0.90f, 1.0f, 1.0f));
    if (UVerticalBoxSlot* NS = VBox->AddChildToVerticalBox(NameText))
    {
        NS->SetHorizontalAlignment(HAlign_Center);
        NS->SetPadding(FMargin(0, 20, 0, 10));
    }
    UTextBlock* DescText = MakeText(Desc, 14.0f, FLinearColor(0.8f, 0.82f, 0.88f, 0.9f));
    if (UVerticalBoxSlot* DS = VBox->AddChildToVerticalBox(DescText))
    {
        DS->SetHorizontalAlignment(HAlign_Center);
        DS->SetPadding(FMargin(10, 5, 10, 20));
    }
    Btn->SetContent(VBox);
    Btn->OnClicked.AddDynamic(this, &UEOKeyInputWidget::OnClassSelected);
    Btn->SetPadding(FMargin(12, 12, 12, 12));

    ClassButtons.Add(Btn);
    ClassDescs.Add(NameText);
    return Btn;
}

void UEOKeyInputWidget::OnConfirmName()
{
    ShowState(1);
}

void UEOKeyInputWidget::OnClassSelected()
{
    // 选中职业，进入游戏
    OnLoadingComplete.Broadcast();
}

UTextBlock* UEOKeyInputWidget::MakeText(const FString& Text, float Size, const FLinearColor& Color)
{
    UTextBlock* T = NewObject<UTextBlock>(this);
    T->SetText(FText::FromString(Text));
    T->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"), Size));
    T->SetColorAndOpacity(Color);
    T->SetJustification(ETextJustify::Center);
    return T;
}
