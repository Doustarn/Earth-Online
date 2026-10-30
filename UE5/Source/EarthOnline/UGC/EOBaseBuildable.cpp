// Immortal Dao Online - 可建造物体基类实现
#include "EOBaseBuildable.h"
#include "Components/StaticMeshComponent.h"

AEOBaseBuildable::AEOBaseBuildable()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void AEOBaseBuildable::BeginPlay()
{
    Super::BeginPlay();

    if (MeshComponent && MeshComponent->GetMaterial(0))
    {
        DefaultMaterial = MeshComponent->GetMaterial(0);
        DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
    }
}

void AEOBaseBuildable::SetSelected(bool bSelected)
{
    if (DynamicMaterial)
    {
        if (bSelected)
        {
            DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength"), 1.0f);
        }
        else
        {
            DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength"), 0.0f);
        }
    }
}

void AEOBaseBuildable::SetMaterialColor(FLinearColor NewColor)
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), NewColor);
    }
}
