// Rommulluss Caraiman 

#pragma once
#include "DrawDebugHelpers.h"
#include "AStarNode.h"
#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "Components/ActorComponent.h"
#include "AStarPathFindingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTARALGORITHM_API UAStarPathFindingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAStarPathFindingComponent();		
	
	UPROPERTY(EditAnywhere)
		bool bDrawBoxExtents;
	UPROPERTY(EditAnywhere)
		bool bDrawPathBoxExtents;
	//UPROPERTY(EditAnywhere)
		//bool bLineOfSightOptimisation;

	UPROPERTY(EditAnywhere)
		FVector FStartNodeExtent = FVector(0.0f);

	UPROPERTY(EditAnywhere)
		FVector FEndNodeExtent	 = FVector(0.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:	
	bool fin = false;
	//Predicate Lambda: Sorts Heap structure of Nodes by smallest fCost first
	 const TFunction<bool(const AAStarNode& a, AAStarNode& b)> SORTING_PREDICATE_LAMBDA = [&](const AAStarNode& a, AAStarNode& b) {return a.fCost < b.fCost;};	//Predicate to keep Array of Nodes in priority queue heap format

	//Methods
	void Algorithm();
	void SpawnNodes();		
	bool IsEndInSight(AAStarNode* _node);	

	//Elements
	bool bPathfindingRequired = false;
	AAStarNode* travelToNextNode;
	FVector moveVector;

	UPROPERTY(EditAnywhere)
		AActor* AEndActor=nullptr;

	TArray<AAStarNode*> ANodePriority;
	TArray<AAStarNode*> ANodePathList;
	AAStarNode* StartNode;
	AAStarNode* EndNode;

	static struct FSuccessorPositions
	{
	private:	
		void AssignConstants(const FVector _nodeSpacing);
		TArray<FVector> FVectorConstants;
	public:
		TArray<FVector> FVectorSuccessors;	

		FSuccessorPositions(){}
		FSuccessorPositions(const FVector _FNodeExtent)
		{
			AssignConstants(_FNodeExtent);
			FVectorSuccessors.SetNum(FVectorConstants.Num());
		}
		~FSuccessorPositions(){}

		int Num() { return FVectorSuccessors.Num(); }

		void AssignNewPositions(const FVector _currentNodePosition)
		{
			for (int i = 0; i < FVectorSuccessors.Num(); i++)
			{
				FVectorSuccessors[i] = FVectorConstants[i] + _currentNodePosition;
			}
		}		
	};

	FSuccessorPositions SuccessorPositions;
	
};
