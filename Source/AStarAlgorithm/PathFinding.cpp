// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinding.h"
#include "Runtime/Engine/Classes/Engine/World.h"



// Sets default values
APathFinding::APathFinding()
{

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_nodeVolume = 64.0f;
	m_nodeDistBetween = m_nodeVolume + 0.01f;
}

// Called when the game starts or when spawned
void APathFinding::BeginPlay()
{
	Super::BeginPlay();
	this->m_listPriorityNodes.Push(this->m_startNode);
	this->m_pathList.Push(this->m_endNode);
	this->m_currentNode = this->m_startNode;
	this->m_currentNode->lCost = 0.0f;


	this->m_currentNode->gCost = m_currentNode->GetSquaredDistanceTo(m_endNode);



}

// Called every frame
void APathFinding::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	if (!finishedPathfinding)
	{
		
		SpawnNodes();
		Algorithm();
	}
	else if (!finishedPathShown&&finishedPathfinding)
	{
		while (m_pathList.Last()!=m_startNode)
		{
			
			m_pathList.Push(m_pathList.Last()->ParentNode);
		}		
		finishedPathShown = true;
	}

}

void  APathFinding::SpawnNodes()
{//SNPAWNS AND CHECKS NEIGHBOURS INTO LIST

	PositionOffset PositionTest(m_currentNode->GetActorLocation(), m_endNode->GetActorLocation(), m_nodeDistBetween);


	UWorld* world = GetWorld();

	for (int i = 0; i < PositionTest.Num(); i++)
	{
		AAStarNode* spawnedActor = nullptr;
		FActorSpawnParameters spawnParams;

		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		spawnParams.Template = SpawnNode.GetDefaultObject();
		spawnParams.Owner = this;
		FRotator rotator(0.0f);
		FVector spawnLocation = PositionTest.GetPositionFVector(i);
		spawnedActor = world->SpawnActor<AAStarNode>(SpawnNode, spawnLocation, rotator, spawnParams);

		if (spawnedActor != nullptr)
		{
			m_currentNode->ArrayNeighbours.Push(spawnedActor);
		}
		else
		{
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnedActor = world->SpawnActor<AAStarNode>(SpawnNode, spawnLocation, rotator, spawnParams);

			TArray<AActor*> OverlappingActors;
			spawnedActor->GetOverlappingActors(OverlappingActors, SpawnNode);

			if (OverlappingActors.Num() != 0)
			{
				for (int i = 0; i < OverlappingActors.Num(); i++)
				{//maybe check for collision with area > some value

					m_currentNode->ArrayNeighbours.Push(StaticCast<AAStarNode*>(OverlappingActors[i]));

				}
			}
			spawnedActor->Destroy();
		}
	}

}

void APathFinding::Algorithm()
{
	for (int i = 0; i < m_currentNode->ArrayNeighbours.Num(); i++)
	{
		float D = m_currentNode->GetSquaredDistanceTo(m_currentNode->ArrayNeighbours[i]);

		if ((m_currentNode->lCost + D) < m_currentNode->ArrayNeighbours[i]->lCost)
		{
			m_currentNode->ArrayNeighbours[i]->ParentNode = m_currentNode;
			m_currentNode->ArrayNeighbours[i]->lCost = m_currentNode->lCost + D;
			m_currentNode->ArrayNeighbours[i]->gCost = m_currentNode->ArrayNeighbours[i]->GetSquaredDistanceTo(m_endNode);

			m_listPriorityNodes.Push(m_currentNode->ArrayNeighbours[i]);
		}
	}

	m_listPriorityNodes.Remove(m_currentNode);

	m_listPriorityNodes.Sort([](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });

	if (m_listPriorityNodes[0] == m_endNode)
	{
		finishedPathfinding = true;
	}
	else
	{
		m_currentNode = m_listPriorityNodes[0];
	}
}

void APathFinding::PositionOffset::AssignConstants(float _nodeSpacing)
{
	p_constants[0] = FVector(-_nodeSpacing, _nodeSpacing, -_nodeSpacing);
	p_constants[1] = FVector(0.0f, _nodeSpacing, -_nodeSpacing);
	p_constants[2] = FVector(_nodeSpacing, _nodeSpacing, -_nodeSpacing);
	p_constants[3] = FVector(-_nodeSpacing, 0.0f, -_nodeSpacing);
	p_constants[4] = FVector(0.0f, 0.0f, -_nodeSpacing);
	p_constants[5] = FVector(_nodeSpacing, 0.0f, -_nodeSpacing);
	p_constants[6] = FVector(-_nodeSpacing, -_nodeSpacing, -_nodeSpacing);
	p_constants[7] = FVector(0.0f, -_nodeSpacing, -_nodeSpacing);
	p_constants[8] = FVector(_nodeSpacing, -_nodeSpacing, -_nodeSpacing);

	p_constants[9] = FVector(-_nodeSpacing, _nodeSpacing, 0.0f);
	p_constants[10] = FVector(0.0f, _nodeSpacing, 0.0f);
	p_constants[11] = FVector(_nodeSpacing, _nodeSpacing, 0.0f);
	p_constants[12] = FVector(-_nodeSpacing, 0.0f, 0.0f);
	//p_constants[13] = FVector( 0.0f, 0.0f, 0.0f);
	p_constants[13] = FVector(_nodeSpacing, 0.0f, 0.0f);
	p_constants[14] = FVector(-_nodeSpacing, -_nodeSpacing, 0.0f);
	p_constants[15] = FVector(0.0f, -_nodeSpacing, 0.0f);
	p_constants[16] = FVector(_nodeSpacing, -_nodeSpacing, 0.0f);

	p_constants[17] = FVector(-_nodeSpacing, _nodeSpacing, _nodeSpacing);
	p_constants[18] = FVector(0.0f, _nodeSpacing, _nodeSpacing);
	p_constants[19] = FVector(_nodeSpacing, _nodeSpacing, _nodeSpacing);
	p_constants[20] = FVector(-_nodeSpacing, 0.0f, _nodeSpacing);
	p_constants[21] = FVector(0.0f, 0.0f, _nodeSpacing);
	p_constants[22] = FVector(_nodeSpacing, 0.0f, _nodeSpacing);
	p_constants[23] = FVector(-_nodeSpacing, -_nodeSpacing, _nodeSpacing);
	p_constants[24] = FVector(0.0f, -_nodeSpacing, _nodeSpacing);
	p_constants[25] = FVector(_nodeSpacing, -_nodeSpacing, _nodeSpacing);
}
