#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EOKeyManager.generated.h"

UCLASS()
class EARTHONLINE_API UEOKeyManager : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 计算字符串的SHA256哈希
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static FString ComputeSHA256(const FString& Input);

    // 验证内测密钥
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static bool VerifyBetaKey(const FString& Key);

    // 检查密钥是否已激活（本地存储）
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static bool IsKeyActivated();

    // 保存已激活的密钥哈希到本地
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static void SaveActivatedKey(const FString& Key);

    // 清除本地激活记录
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static void ClearActivation();

    // 获取当前版本号
    UFUNCTION(BlueprintCallable, Category = "密钥")
    static FString GetGameVersion();

private:
    // 合法的内测密钥SHA256哈希列表（5个）
    static const TArray<FString> ValidKeyHashes;

    // 版本号
    static const FString GameVersion;
};
