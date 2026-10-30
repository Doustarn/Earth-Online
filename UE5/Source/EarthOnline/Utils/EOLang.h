#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EOLang.generated.h"

/**
 * 全局语言管理：0=简体中文, 1=English
 * 注册时按 IP/选择写入 Save/lang.txt，启动时读取
 */
UCLASS()
class EARTHONLINE_API UEOLang : public UObject
{
    GENERATED_BODY()

public:
    static int32 CurrentLang;  // 0=CN, 1=EN

    // 根据当前语言返回对应文本
    static FString T(const FString& CN, const FString& EN);

    // 从 Save/lang.txt 读语言设置
    static void LoadLang();

    // 写入语言设置
    static void SaveLang(int32 Lang);
};
