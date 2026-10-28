// 地球Online - 基础几何体实现
#include "EOBasicShapes.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AEOBoxBuildable::AEOBoxBuildable()
{
    DisplayName = TEXT("立方体");
    Category = TEXT("基础形状");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshFinder.Succeeded() && MeshComponent)
    {
        MeshComponent->SetStaticMesh(MeshFinder.Object);
    }
}

AEOSphereBuildable::AEOSphereBuildable()
{
    DisplayName = TEXT("球体");
    Category = TEXT("基础形状");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (MeshFinder.Succeeded() && MeshComponent)
    {
        MeshComponent->SetStaticMesh(MeshFinder.Object);
    }
}

AEOPlaneBuildable::AEOPlaneBuildable()
{
    DisplayName = TEXT("平面");
    Category = TEXT("基础形状");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (MeshFinder.Succeeded() && MeshComponent)
    {
        MeshComponent->SetStaticMesh(MeshFinder.Object);
    }
}

AEOCylinderBuildable::AEOCylinderBuildable()
{
    DisplayName = TEXT("圆柱体");
    Category = TEXT("基础形状");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshFinder.Succeeded() && MeshComponent)
    {
        MeshComponent->SetStaticMesh(MeshFinder.Object);
    }
}

AEOConeBuildable::AEOConeBuildable()
{
    DisplayName = TEXT("圆锥");
    Category = TEXT("基础形状");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (MeshFinder.Succeeded() && MeshComponent)
    {
        MeshComponent->SetStaticMesh(MeshFinder.Object);
    }
}
