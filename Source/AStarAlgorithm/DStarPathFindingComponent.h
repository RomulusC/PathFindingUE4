// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DStarNode.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DStarPathFindingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTARALGORITHM_API UDStarPathFindingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDStarPathFindingComponent();


	ADStarNode* StartNode;
	ADStarNode* GoalNode;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
private:
	const TFunction<bool(const ADStarNode& a, ADStarNode& b)> SortingPredicate = [&](const ADStarNode& lhs, ADStarNode& rhs) {return lhs.key < rhs.key; };

	TArray<ADStarNode*> U;

	void QueueInsert(ADStarNode* _node);
	void CalculateKey(ADStarNode* _inputNode);
	void Initialize();
	void ComputeShortestPath();
	void UpdateVertex(ADStarNode* _node);
	//Lambda that sorts nodes in the priority list with node's tuple key lexicographically
//	TFunction<bool(const ADStarNode& a, ADStarNode& b)> SortingPredicate = [&](const ADStarNode& a, ADStarNode& b) {return a.key <= b.key;};
	
};
