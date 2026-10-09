#include "UI/EOKeyInputWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Security/EOKeyManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/EOGameInstance.h"

bool UEOKeyInputWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (VerifyButton)
    {
        VerifyButton->OnClicked.AddDynamic(this, &UEOKeyInputWidget::OnVerifyClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UEOKeyInputWidget::OnQuitClicked);
    }

    if (KeyInput)
    {
        KeyInput->OnTextChanged.AddDynamic(this, &UEOKeyInputWidget::OnKeyTextChanged);
        KeyInput->SetHintText(FText::FromString(TEXT("请输入内测密钥，格式：EO-018-XXXX-XXXX-XXXX")));
    }

    if (VersionText)
    {
        VersionText->SetText(FText::FromString(FString::Printf(TEXT("v%s Beta"), *UEOKeyManager::GetGameVersion())));
    }

    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("地球Online 内测激活")));
    }

    // 检查是否已激活
    if (UEOKeyManager::IsKeyActivated())
    {
        SetStatus(TEXT("检测到已激活的密钥，正在进入游戏..."), FColor::Green);
        OnVerificationSuccess();
    }
    else
    {
        SetStatus(TEXT("请输入内测密钥以继续"), FColor::White);
    }

    return true;
}

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
        // 验证成功，保存激活状态
        UEOKeyManager::SaveActivatedKey(InputKey);
        SetStatus(TEXT("密钥验证成功！正在进入游戏..."), FColor::Green);
        OnVerificationSuccess();
    }
    else
    {
        SetStatus(TEXT("密钥无效，请检查后重试"), FColor::Red);
    }
}

void UEOKeyInputWidget::OnQuitClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->ConsoleCommand("quit");
    }
}

void UEOKeyInputWidget::OnKeyTextChanged(const FText& Text)
{
    // 自动转大写
    FString UpperText = Text.ToString().ToUpper();
    if (UpperText != Text.ToString())
    {
        KeyInput->SetText(FText::FromString(UpperText));
    }
}

void UEOKeyInputWidget::OnVerificationSuccess()
{
    // 延迟后切换到主菜单或游戏
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        this->RemoveFromParent();

        // 显示主菜单
        if (AEOHUD* HUD = Cast<AEOHUD>(GetOwningPlayer()->GetHUD()))
        {
            HUD->ShowMainMenu();
        }
    }, 1.5f, false);
}

void UEOKeyInputWidget::SetStatus(const FString& Message, FColor Color)
{
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(Message));
        StatusText->SetColorAndOpacity(FSlateColor(Color));
    }
}
