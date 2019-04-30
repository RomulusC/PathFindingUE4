// Rommulluss Caraiman

#include "AStarNode.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "AStarPathFindingComponent.h" 


// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
AAStarNode::AAStarNode()
{	
		gCost = INFINITY;	
		hCost = INFINITY;	
		fCost = INFINITY;
	
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	this->RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(FName("NodePreset"));	
	BoxComponent->bMultiBodyOverlap = true;
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->SetUseCCD(true);
	BoxComponent->SetupAttachment(GetRootComponent());	
}

// Called when the game starts or when spawned
void AAStarNode::BeginPlay()
{
	Super::BeginPlay();	
	BoxComponent->SetCollisionProfileName(FName("SpawnedNodePreset"));		
}

// Called every frame
void AAStarNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	if (bNodeDrawExtent)
		DrawDebugBox(GetWorld(), this->GetActorLocation(), this->GetComponentsBoundingBox().GetExtent(), ExtentsColor, true, 5);
}

