#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EOGameMode.generated.h"

UCLASS()
class EARTHONLINE_API AEOGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AEOGameMode();

    virtual void BeginPlay() override;

    // 最大玩家数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "游戏设置")
    int32 MaxPlayers;

    // 游戏名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "游戏设置")
    FString GameName;

    // 内测版本号
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "内测")
    FString BetaVersion;

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    UFUNCTION(BlueprintCallable, Category = "游戏")
    int32 GetCurrentPlayerCount();
};
