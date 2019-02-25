// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AStarNode.h"
#include "CoreMinimal.h"
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
		bool b_visualCollisionBox;

	UPROPERTY(EditAnywhere)
		FVector m_sizeOfNodeBoundingBox = FVector(0.0f);

	UPROPERTY(EditAnywhere)
		FVector m_endNodeBoundingBox = FVector(0.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	TArray<AAStarNode*> GetPriorityListNodes() { return this->m_listPriorityNodes; }
private:
	//Methods
	void  Algorithm();
	void SpawnNodes();

	//Elements
	bool check = false;
	

	UPROPERTY(EditAnywhere)
		AActor* m_EndActor=nullptr;
	
	TArray<AAStarNode*> m_listPriorityNodes;
	TArray<AAStarNode*> m_pathList;

	bool finishedPathfinding = false;

	static struct PositionOffset
	{
	private:
		
		
		

	public:
		TArray<FVector> p_successor;
		TArray<FVector> p_constants;
		PositionOffset() 
		{

		}		
		PositionOffset(const FVector _nodeSpacing)
		{

			AssignConstants(_nodeSpacing);
			p_successor.SetNum(p_constants.Num());

		}
		~PositionOffset()
		{
		}
		int Num() { return p_successor.Num(); }
		void AssignNewPositions(const FVector _currentNodePosition)
		{
			for (int i = 0; i < p_successor.Num(); i++)
			{

				p_successor[i] = p_constants[i] + _currentNodePosition;
				
				

			}

		}
		
	private:

		void AssignConstants(const FVector _nodeSpacing);
		
	};

	PositionOffset successor_struct;
	
};
