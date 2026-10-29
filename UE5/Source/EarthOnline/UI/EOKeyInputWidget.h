#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EOKeyInputWidget.generated.h"

class UCanvasPanel;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UVerticalBox;
class UHorizontalBox;

/**
 * 登录/建角流程（Immortal Dao Online）：
 * State 0: 输入道号
 * State 1: 随机身份生成（宗门+姓名+境界）
 * State 2: 5 职业选择（剑修/枪修/法修/丹修/符修）
 * State 3: 完成，广播 OnLoadingComplete 进入大世界
 */
UCLASS()
class EARTHONLINE_API UEOKeyInputWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingComplete);
    UPROPERTY(BlueprintAssignable)
    FOnLoadingComplete OnLoadingComplete;

protected:
    UCanvasPanel* MainCanvas;

    // 状态机
    int32 GameState; // 0=登录 1=身份 2=职业 3=完成
    FTimerHandle IdentityTimer;

    // State 0: 登录
    UTextBlock* LoginTitle;
    UEditableTextBox* NameInput;
    UButton* ConfirmNameBtn;
    UTextBlock* LoginHint;

    // State 1: 随机身份
    UTextBlock* IdentityText;
    UTextBlock* IdentitySub;

    // State 2: 职业选择
    UTextBlock* ClassTitle;
    UVerticalBox* ClassRow;
    TArray<UButton*> ClassButtons;
    TArray<UTextBlock*> ClassDescs;

    void BuildUI();
    void BuildBackground();
    void ShowState(int32 NewState);
    void ShowLoginState();
    void ShowIdentityState();
    void ShowClassState();

    FString GenerateRandomIdentity();

    UTextBlock* MakeText(const FString& Text, float Size, const FLinearColor& Color);
    UButton* MakeClassButton(const FString& Name, const FString& Desc, int32 Index);

    UFUNCTION()
    void OnConfirmName();
    UFUNCTION()
    void OnClassSelected();
};
