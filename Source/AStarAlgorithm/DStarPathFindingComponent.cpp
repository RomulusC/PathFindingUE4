// Fill out your copyright notice in the Description page of Project Settings.

#include "DStarPathFindingComponent.h"


// Sets default values for this component's properties
UDStarPathFindingComponent::UDStarPathFindingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	//Initialize();

	// ...
}


// Called when the game starts
void UDStarPathFindingComponent::BeginPlay()
{
	Super::BeginPlay();

//	Initialize();
	
	
	
}


// Called every frame
void UDStarPathFindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//void UDStarPathFindingComponent::Initialize()
//{
//	GoalNode->rhs = 0;	
//	QueueInsert(GoalNode);
//
//}
//void UDStarPathFindingComponent::QueueInsert(ADStarNode* _node)
//{
//	CalculateKey(_node);
//	U.HeapPush(GoalNode,SortingPredicate);
//}
//void UDStarPathFindingComponent::CalculateKey(ADStarNode* _inputNode)
//{
//	float g = _inputNode->g;
//	float rhs = _inputNode->rhs;
//	float h = StartNode->GetDistanceTo(_inputNode);
//	float k;
//	float four = _inputNode->rhs;
//
//	float result1;
//	float result2;
//
//	if (g < rhs)
//	{
//		result1 = g;
//	}
//	else
//	{
//		result1 = rhs;
//	}
//	if (g <= four)
//	{
//		result2 = g;
//	}
//	else
//	{
//		result2 = four;
//	}
//	_inputNode->key = TTuple<float, float>(result1, result2);
//	
//
//}
//
//
//void UDStarPathFindingComponent::ComputeShortestPath()
//{
//	
//	while (U.HeapTop()->key< CalculateKey(StartNode)||StartNode->rhs!=StartNode->g)
//	{
//		TTuple<float, float> k_old = U.HeapTop()->key;
//
//		ADStarNode* u = nullptr;
//		U.HeapPop(u,SortingPredicate);
//		if (k_old< CalculateKey(u))
//		{
//			U.HeapPush(u,SortingPredicate);
//		}
//		else if (u->g>u->rhs)
//		{
//			u->g = u->rhs;
//			for (int i = 0; i<u->Pred.Num();i++)
//			{
//				UpdateVertex(u);
//			}
//		}
//		else
//		{
//			u->g = INFINITY;
//			for (int i = 0; i < u->Pred.Num(); i++)
//			{
//				UpdateVertex(u->Pred[i]);
//			}
//			UpdateVertex(u);
//		}
//	}
//}
//void UDStarPathFindingComponent::UpdateVertex(ADStarNode* _node)
//{
//	if (_node!=GoalNode)
//	{
//		TArray<float> tempArray;
//		for (int i = 0; i<_node->Succ.Num();i++)
//		{
//			tempArray.HeapPush(_node->GetDistanceTo(_node->Succ[i]), [&](float a, float b) {return a < b; });
//			
//		}
//		_node->rhs = tempArray.HeapTop();
//
//	}
//	U.Remove(_node);
//
//	if (_node->g!=_node->rhs)
//	{
//		CalculateKey(_node);
//		U.HeapPush(_node,SortingPredicate);
//	}
//}

