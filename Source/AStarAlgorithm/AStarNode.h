// Rommulluss Caraiman 

#pragma once
#include "Collision.h"
#include "Engine/CollisionProfile.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNode.generated.h"

UCLASS()
class ASTARALGORITHM_API AAStarNode : public AActor
{
	GENERATED_BODY()

private:
	USceneComponent* SceneComponent;
	bool collisionChange = false;
	
public:
	// Sets default values for this actor's properties
	AAStarNode();

	bool bNodeDrawExtent = false;
	FColor ExtentsColor = FColor::Green;
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxComponent;

	AAStarNode* ParentNode;
	TArray<AAStarNode*> ArrayNeighbours;

	UPROPERTY(VisibleAnywhere)
		float gCost = INFINITY;

	UPROPERTY(VisibleAnywhere)
		float hCost = INFINITY;

	UPROPERTY(VisibleAnywhere)
		float fCost = INFINITY;

	bool IsNodeVisited = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
