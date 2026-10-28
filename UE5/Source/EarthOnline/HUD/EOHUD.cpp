#include "EOHUD.h"
#include "../UI/EOMainMenuWidget.h"
#include "../UI/EOHUDWidget.h"
#include "../UI/EOKeyInputWidget.h"
#include "../Security/EOKeyManager.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"
#include "RenderingThread.h"

// 手写 24 位 BMP 保存（无外部依赖，供验收截图）
static bool SaveBMP(int32 Width, int32 Height, const TArray<FColor>& Pixels, const FString& Path)
{
    const int32 RowSize = ((Width * 3 + 3) & ~3);
    const int32 DataOffset = 54;
    const int32 TotalSize = DataOffset + RowSize * Height;
    if (Pixels.Num() < Width * Height) return false;

    TArray<uint8> D;
    D.SetNum(TotalSize);
    D[0] = 'B'; D[1] = 'M';
    uint32 FileSize = TotalSize;
    FMemory::Memcpy(&D[2], &FileSize, 4);
    uint32 OffBits = DataOffset;
    FMemory::Memcpy(&D[10], &OffBits, 4);
    int32 InfoSize = 40;
    FMemory::Memcpy(&D[14], &InfoSize, 4);
    FMemory::Memcpy(&D[18], &Width, 4);
    FMemory::Memcpy(&D[22], &Height, 4);
    int16 Planes = 1;
    FMemory::Memcpy(&D[26], &Planes, 2);
    int16 Bpp = 24;
    FMemory::Memcpy(&D[28], &Bpp, 2);
    uint32 ImgSize = RowSize * Height;
    FMemory::Memcpy(&D[34], &ImgSize, 4);

    for (int32 Y = 0; Y < Height; Y++)
    {
        const FColor* Row = &Pixels[(Height - 1 - Y) * Width];
        for (int32 X = 0; X < Width; X++)
        {
            const int32 Dst = DataOffset + Y * RowSize + X * 3;
            D[Dst + 0] = Row[X].B;
            D[Dst + 1] = Row[X].G;
            D[Dst + 2] = Row[X].R;
        }
    }
    return FFileHelper::SaveArrayToFile(D, *Path);
}

AEOHUD::AEOHUD()
{
    // C++ 直接指定 Widget 类（项目无蓝图资产，必须在构造函数分配，否则 HUD 永远无法创建）
    GameHUDWidgetClass = UEOHUDWidget::StaticClass();
    MainMenuWidgetClass = UEOMainMenuWidget::StaticClass();
    KeyInputWidgetClass = UEOKeyInputWidget::StaticClass();

    bMenuVisible = false;
}

void AEOHUD::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("[EOHUD] BeginPlay 开始，GameHUDWidgetClass=%s KeyInputWidgetClass=%s"),
        GameHUDWidgetClass ? *GameHUDWidgetClass->GetName() : TEXT("空"),
        KeyInputWidgetClass ? *KeyInputWidgetClass->GetName() : TEXT("空"));

    // 创建游戏HUD（默认隐藏）
    if (GameHUDWidgetClass)
    {
        GameHUDWidget = CreateWidget<UEOHUDWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            GameHUDWidget->AddToViewport();
            GameHUDWidget->SetVisibility(ESlateVisibility::Hidden);
            UE_LOG(LogTemp, Warning, TEXT("[EOHUD] GameHUDWidget 创建成功并加入视口（隐藏）"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[EOHUD] GameHUDWidget 创建失败！"));
        }
    }

    // 创建主菜单（默认隐藏）
    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UEOMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
            // 绑定开始游戏 → 登录界面（必须在这里绑，ShowMainMenu 的 !MainMenuWidget 分支不会走到）
            MainMenuWidget->OnStartGame.AddDynamic(this, &AEOHUD::ShowKeyInput);
        }
    }

    // 启动时先显示开始界面
    ShowMainMenu();
}

void AEOHUD::ShowKeyInput()
{
    if (KeyInputWidgetClass && !KeyInputWidget)
    {
        KeyInputWidget = CreateWidget<UEOKeyInputWidget>(GetWorld(), KeyInputWidgetClass);
        if (KeyInputWidget)
        {
            KeyInputWidget->AddToViewport();
            // 登录/加载完成 → 进入游戏HUD
            KeyInputWidget->OnLoadingComplete.AddDynamic(this, &AEOHUD::ShowGameHUD);
        }
    }

    if (KeyInputWidget)
    {
        if (MainMenuWidget) MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        if (GameHUDWidget) GameHUDWidget->SetVisibility(ESlateVisibility::Hidden);
        KeyInputWidget->SetVisibility(ESlateVisibility::Visible);

        if (APlayerController* PC = GetOwningPlayerController())
        {
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
            PC->bEnableClickEvents = true;
            PC->bEnableMouseOverEvents = true;
        }
    }
    else
    {
        // 如果没有设置Widget类，直接进入游戏
        ShowGameHUD();
    }
}

void AEOHUD::ShowMainMenu()
{
    if (MainMenuWidgetClass && !MainMenuWidget)
    {
        MainMenuWidget = CreateWidget<UEOMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            // 点击"开始游戏" → 进入登录/加载界面
            MainMenuWidget->OnStartGame.AddDynamic(this, &AEOHUD::ShowKeyInput);
        }
    }

    if (MainMenuWidget)
    {
        if (KeyInputWidget) KeyInputWidget->SetVisibility(ESlateVisibility::Hidden);
        if (GameHUDWidget) GameHUDWidget->SetVisibility(ESlateVisibility::Hidden);
        MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
        bMenuVisible = true;

        if (APlayerController* PC = GetOwningPlayerController())
        {
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
            PC->bEnableClickEvents = true;
            PC->bEnableMouseOverEvents = true;
        }
    }
}

void AEOHUD::ShowGameHUD()
{
    UE_LOG(LogTemp, Warning, TEXT("[EOHUD] ShowGameHUD 被调用，GameHUDWidget=%s"),
        GameHUDWidget ? TEXT("有效") : TEXT("空"));

    if (KeyInputWidget)
    {
        KeyInputWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (MainMenuWidget)
    {
        MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        bMenuVisible = false;
    }

    if (GameHUDWidget)
    {
        GameHUDWidget->SetVisibility(ESlateVisibility::Visible);
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        // 游戏HUD含虚拟摇杆/动作键/手机等可点击UI，需显示鼠标并允许UI交互
        PC->SetInputMode(FInputModeGameAndUI());
        PC->bShowMouseCursor = true;
    }
}

void AEOHUD::ToggleMenu()
{
    if (bMenuVisible)
    {
        ShowGameHUD();
    }
    else
    {
        ShowMainMenu();
    }
}

void AEOHUD::ShowNotification(const FString& Message)
{
    if (GameHUDWidget)
    {
        GameHUDWidget->ShowNotification(Message);
    }
    UE_LOG(LogTemp, Log, TEXT("[通知] %s"), *Message);
}
