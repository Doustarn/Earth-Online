#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOKeyInputWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class EARTHONLINE_API UEOKeyInputWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    // 密钥输入框
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* KeyInput;

    // 验证按钮
    UPROPERTY(meta = (BindWidget))
    UButton* VerifyButton;

    // 退出按钮
    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

    // 状态文本
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StatusText;

    // 版本号文本
    UPROPERTY(meta = (BindWidget))
    UTextBlock* VersionText;

    // 标题文本
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;

    UFUNCTION()
    void OnVerifyClicked();

    UFUNCTION()
    void OnQuitClicked();

    UFUNCTION()
    void OnKeyTextChanged(const FText& Text);

    // 验证成功后的回调
    virtual void OnVerificationSuccess();

    // 设置状态消息
    void SetStatus(const FString& Message, FColor Color);
};
