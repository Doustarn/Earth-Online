// 地球Online - 基础几何体（立方体/球体/平面/圆柱体）
#pragma once

#include "CoreMinimal.h"
#include "EOBaseBuildable.h"
#include "EOBasicShapes.generated.h"

// 立方体
UCLASS()
class EARTHONLINE_API AEOBoxBuildable : public AEOBaseBuildable
{
    GENERATED_BODY()
public:
    AEOBoxBuildable();
};

// 球体
UCLASS()
class EARTHONLINE_API AEOSphereBuildable : public AEOBaseBuildable
{
    GENERATED_BODY()
public:
    AEOSphereBuildable();
};

// 平面
UCLASS()
class EARTHONLINE_API AEOPlaneBuildable : public AEOBaseBuildable
{
    GENERATED_BODY()
public:
    AEOPlaneBuildable();
};

// 圆柱体
UCLASS()
class EARTHONLINE_API AEOCylinderBuildable : public AEOBaseBuildable
{
    GENERATED_BODY()
public:
    AEOCylinderBuildable();
};

// 圆锥
UCLASS()
class EARTHONLINE_API AEOConeBuildable : public AEOBaseBuildable
{
    GENERATED_BODY()
public:
    AEOConeBuildable();
};
