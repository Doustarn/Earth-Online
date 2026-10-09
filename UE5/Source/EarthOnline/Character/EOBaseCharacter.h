#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EOBaseCharacter.generated.h"

UCLASS()
class EARTHONLINE_API AEOBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEOBaseCharacter();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 玩家名称
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "玩家")
    FString PlayerName;

    // 玩家等级
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "玩家")
    int32 PlayerLevel;

    // 生命值
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "属性")
    float Health;

    // 最大生命值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "属性")
    float MaxHealth;

    // 体力
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "属性")
    float Stamina;

    // 最大体力
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "属性")
    float MaxStamina;

    UFUNCTION(BlueprintCallable, Category = "属性")
    void TakeDamage(float Damage);

    UFUNCTION(BlueprintCallable, Category = "属性")
    void Heal(float Amount);

protected:
    // 移动输入
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void JumpPressed();
    void SprintStart();
    void SprintEnd();

    // 是否在冲刺
    bool bIsSprinting;

    // 基础移动速度
    float BaseWalkSpeed;
    float SprintSpeed;
};
