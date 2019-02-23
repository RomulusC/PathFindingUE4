// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AStarNode.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathFinding.generated.h"

UCLASS()
class ASTARALGORITHM_API APathFinding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APathFinding();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AAStarNode> SpawnNode;

	void SpawnNodes();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere)
		float m_nodeVolume;
	float m_nodeDistBetween;
	UPROPERTY(EditAnywhere)
		AAStarNode* m_startNode;
	UPROPERTY(EditAnywhere)
		AAStarNode* m_endNode;

	TArray<AAStarNode*> m_listPriorityNodes;
	TArray<AAStarNode*> m_pathList;
	AAStarNode* m_currentNode;
	bool finishedPathfinding = false;
	bool finishedPathShown = false;
	void Algorithm();



	struct PositionOffset
	{
	private:
		struct PositionAndDistance
		{
			float   m_D;
			FVector s_position;
		};
		TArray<PositionAndDistance> position_distance;
		TArray<FVector> p_constants;


	public:
		PositionOffset(const FVector _nodePosition, const FVector _endNodePosition, const float _nodeSpacing)
		{
			p_constants.SetNum(26);
			position_distance.SetNum(p_constants.Num());
			AssignConstants(_nodeSpacing);
			AssignPositions(_nodePosition, _endNodePosition);
			position_distance.Sort([](PositionAndDistance lsh, PositionAndDistance rhs) {return lsh.m_D < rhs.m_D; });

		}
		~PositionOffset()
		{
		}
		int Num() { return position_distance.Num(); }
		FVector GetPositionFVector(int i) { return(position_distance[i].s_position); }
	private:

		void AssignConstants(const float _nodeSpacing);
		void AssignPositions(const FVector _nodePosition, const FVector _endNodePosition)
		{
			for (int i = 0; i < position_distance.Num(); i++)
			{

				position_distance[i].s_position = FVector((_nodePosition.X + p_constants[i].X), _nodePosition.Y + p_constants[i].Y, _nodePosition.Z + p_constants[i].Z);
				//position_distance[i].m_D = m_currentNode->GetDistanceTo(m_endNode);


			}

		}
	};
};
