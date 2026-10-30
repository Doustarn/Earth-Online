// Immortal Dao Online - 可建造物体基类
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EOBaseBuildable.generated.h"

UCLASS()
class EARTHONLINE_API AEOBaseBuildable : public AActor
{
    GENERATED_BODY()

public:
    AEOBaseBuildable();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buildable")
    UStaticMeshComponent* MeshComponent;

    // 物体显示名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildable")
    FString DisplayName;

    // 物体分类
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildable")
    FString Category;

    // 是否可被玩家编辑
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildable")
    bool bPlayerEditable = true;

    // 选中时高亮
    UFUNCTION(BlueprintCallable, Category = "Buildable")
    void SetSelected(bool bSelected);

    // 设置材质颜色
    UFUNCTION(BlueprintCallable, Category = "Buildable")
    void SetMaterialColor(FLinearColor NewColor);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UMaterialInterface* DefaultMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;
};
