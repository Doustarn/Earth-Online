#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOMainMenuWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class EARTHONLINE_API UEOMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    // 创建服务器按钮
    UPROPERTY(meta = (BindWidget))
    UButton* HostButton;

    // 加入服务器按钮
    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

    // 退出游戏按钮
    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

    // 服务器地址输入框
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ServerAddressInput;

    // 端口输入框
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* PortInput;

    // 版本号文本
    UPROPERTY(meta = (BindWidget))
    UTextBlock* VersionText;

    UFUNCTION()
    void OnHostClicked();

    UFUNCTION()
    void OnJoinClicked();

    UFUNCTION()
    void OnQuitClicked();
};
