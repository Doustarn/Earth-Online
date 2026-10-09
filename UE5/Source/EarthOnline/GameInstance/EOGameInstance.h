#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EOGameInstance.generated.h"

UCLASS()
class EARTHONLINE_API UEOGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UEOGameInstance();

    virtual void Init() override;

    // 服务器地址
    UPROPERTY(BlueprintReadWrite, Category = "网络")
    FString ServerAddress;

    // 服务器端口
    UPROPERTY(BlueprintReadWrite, Category = "网络")
    int32 ServerPort;

    // 玩家名称
    UPROPERTY(BlueprintReadWrite, Category = "玩家")
    FString PlayerName;

    // 是否为内测模式
    UPROPERTY(BlueprintReadWrite, Category = "内测")
    bool bIsBetaMode;

    // 加入服务器
    UFUNCTION(BlueprintCallable, Category = "网络")
    void JoinServer(const FString& Address, int32 Port);

    // 创建监听服务器
    UFUNCTION(BlueprintCallable, Category = "网络")
    void HostServer(int32 Port, int32 MaxPlayers);

    // 返回主菜单
    UFUNCTION(BlueprintCallable, Category = "网络")
    void ReturnToMainMenu();
};
