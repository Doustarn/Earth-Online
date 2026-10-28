// 地球Online - 玩家建造组件
// 支持放置、移动、旋转、缩放物体
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EOBuilderComponent.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EEOBuildMode : uint8
{
    None,
    Place,
    Move,
    Rotate,
    Scale,
    Delete
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EARTHONLINE_API UEOBuilderComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEOBuilderComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 设置建造模式
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void SetBuildMode(EEOBuildMode NewMode);

    // 获取当前建造模式
    UFUNCTION(BlueprintCallable, Category = "Builder")
    EEOBuildMode GetBuildMode() const { return BuildMode; }

    // 设置要放置的物体类
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void SetPlaceableClass(TSubclassOf<AActor> ActorClass);

    // 开始放置预览
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void StartPlacement();

    // 确认放置
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void ConfirmPlacement();

    // 取消放置
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void CancelPlacement();

    // 选择物体
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void SelectActor(AActor* Actor);

    // 取消选择
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void DeselectActor();

    // 删除选中的物体
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void DeleteSelected();

    // 复制选中的物体
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void DuplicateSelected();

    // 启用/禁用网格吸附
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void SetGridSnap(bool bEnable, float GridSize = 100.f);

    // 启用/禁用旋转吸附
    UFUNCTION(BlueprintCallable, Category = "Builder")
    void SetRotationSnap(bool bEnable, float RotationStep = 45.f);

    // 获取选中的物体
    UFUNCTION(BlueprintCallable, Category = "Builder")
    AActor* GetSelectedActor() const { return SelectedActor; }

    // 是否在建造模式
    UFUNCTION(BlueprintCallable, Category = "Builder")
    bool IsBuilding() const { return BuildMode != EEOBuildMode::None; }

    // 保存当前场景为地图
    UFUNCTION(BlueprintCallable, Category = "Builder")
    bool SaveCurrentMap(const FString& MapName);

    // 加载地图
    UFUNCTION(BlueprintCallable, Category = "Builder")
    bool LoadMap(const FString& MapName);

protected:
    virtual void BeginPlay() override;

private:
    EEOBuildMode BuildMode = EEOBuildMode::None;

    UPROPERTY()
    TSubclassOf<AActor> PlaceableClass;

    UPROPERTY()
    AActor* PreviewActor = nullptr;

    UPROPERTY()
    AActor* SelectedActor = nullptr;

    bool bGridSnap = true;
    float GridSize = 100.f;
    bool bRotationSnap = true;
    float RotationStep = 45.f;

    // 射线检测获取放置位置
    FVector GetPlacementLocation() const;

    // 应用网格吸附
    FVector SnapToGrid(const FVector& Location) const;

    // 更新预览物体位置
    void UpdatePreview();

    // 销毁预览物体
    void DestroyPreview();
};
