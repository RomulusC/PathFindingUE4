// Fill out your copyright notice in the Description page of Project Settings.

#include "AStarPathFindingComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values for this component's properties
UAStarPathFindingComponent::UAStarPathFindingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	
}

// Called when the game starts
void UAStarPathFindingComponent::BeginPlay()
{
	Super::BeginPlay();	
	
	if (m_EndActor != nullptr)
	{
		
		if (m_sizeOfNodeBoundingBox ==FVector(0.0f))
		{
			m_sizeOfNodeBoundingBox = this->GetOwner()->GetComponentsBoundingBox(false).GetSize() / 2;
		}		
		
		successor_struct = PositionOffset((m_sizeOfNodeBoundingBox)*2);
		AAStarNode* spawnedNode = nullptr;
		FActorSpawnParameters spawnParams;
		
		spawnParams.Owner = this->GetOwner();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator rotator(0.0f);
		UWorld* world = GetWorld();
		FVector spawnLocation = GetOwner()->GetActorLocation();

		spawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, spawnParams);
		
		spawnedNode->BoxCollision->SetBoxExtent(m_sizeOfNodeBoundingBox);
		spawnedNode->b_VisualNodes = b_visualCollisionBox;		
		m_listPriorityNodes.HeapPush(spawnedNode, [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });
			
		
		spawnedNode = nullptr;
		spawnLocation = m_EndActor->GetActorLocation();
		spawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, spawnParams);
		spawnedNode->b_VisualNodes = b_visualCollisionBox;	

		if (m_endNodeBoundingBox==FVector(0.0f))
		{
			m_endNodeBoundingBox = m_EndActor->GetComponentsBoundingBox(false).GetSize() / 2;			
		}

		spawnedNode->BoxCollision->SetBoxExtent(m_endNodeBoundingBox);
		m_pathList.Add(spawnedNode);

		m_listPriorityNodes.HeapTop()->lCost = 0;
		m_listPriorityNodes.HeapTop()->gCost =m_listPriorityNodes.HeapTop()->GetSquaredDistanceTo(m_pathList.HeapTop());
		m_listPriorityNodes.HeapTop()->fCost =m_listPriorityNodes.HeapTop()->gCost +m_listPriorityNodes.HeapTop()->lCost;

		m_pathList.HeapTop()->gCost = 0;
		check = true;
		
	}	
}

// Called every frame
void UAStarPathFindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (check)
	{
		if (m_listPriorityNodes.HeapTop() != m_pathList.HeapTop())
		{

			SpawnNodes();
			Algorithm();
		}
		else
		{
			Algorithm();

		}
	}	
}

void  UAStarPathFindingComponent::Algorithm()
{
	 AAStarNode* currentNode=nullptr;
	 m_listPriorityNodes.HeapPop(currentNode, [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });
	
	for (int i = 0; i < currentNode->ArrayNeighbours.Num(); i++)
	{
		float D = currentNode->GetSquaredDistanceTo(currentNode->ArrayNeighbours[i]);

		if ((currentNode->lCost + D) < currentNode->ArrayNeighbours[i]->lCost)
		{
			currentNode->ArrayNeighbours[i]->ParentNode = currentNode;
			currentNode->ArrayNeighbours[i]->lCost =currentNode->lCost + D;
			currentNode->ArrayNeighbours[i]->gCost =currentNode->ArrayNeighbours[i]->GetSquaredDistanceTo(m_pathList[0]);
			currentNode->ArrayNeighbours[i]->fCost =currentNode->ArrayNeighbours[i]->lCost + currentNode->ArrayNeighbours[i]->gCost;	

			
			m_listPriorityNodes.HeapPush(currentNode->ArrayNeighbours[i], [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });			
		}
	}		
}

void  UAStarPathFindingComponent::SpawnNodes()
{//SNPAWNS AND CHECKS NEIGHBOURS INTO LIST


	successor_struct.AssignNewPositions(m_listPriorityNodes.HeapTop()->GetActorLocation());
	UWorld* world = GetWorld();
	
	
	for (int i = 0; i< successor_struct.p_successor.Num(); i++)
	{
		AAStarNode* spawnedNode = nullptr;
		FActorSpawnParameters spawnParams;

		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;		
		spawnParams.Owner = this->GetOwner();
		spawnParams.Template = NULL;
		
		FRotator rotator(0.0f);
		FVector spawnLocation = successor_struct.p_successor[i];
		spawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, spawnParams);
		
		if (spawnedNode!=nullptr)
		{			
			spawnedNode->BoxCollision->SetBoxExtent(m_endNodeBoundingBox);
			spawnedNode->b_VisualNodes = b_visualCollisionBox;
			m_listPriorityNodes.HeapTop()->ArrayNeighbours.Add(spawnedNode);
					
		}
		else
		{
			//spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			//spawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, spawnParams);

			//TArray<AActor*> OverlappingActors;
			//spawnedNode->GetOverlappingActors(OverlappingActors, AAStarNode::StaticClass());

			//if (OverlappingActors.Num() != 0)
			//{
			//	for (int i = 0; i < OverlappingActors.Num(); i++)
			//	{//maybe check for collision with area > some value

			//		m_listPriorityNodes.HeapTop()->ArrayNeighbours.HeapPush(StaticCast<AAStarNode*>(OverlappingActors[i]), [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });

			//	}
			//}
		}
	}
}


void  UAStarPathFindingComponent::PositionOffset::AssignConstants(FVector _nodeSpacing)
{
	p_constants.SetNum(26);

	p_constants[0]  = FVector(-_nodeSpacing.X, _nodeSpacing.Y,-_nodeSpacing.Z);
	p_constants[1]  = FVector(           0.0f, _nodeSpacing.Y,-_nodeSpacing.Z);
	p_constants[2]  = FVector( _nodeSpacing.X, _nodeSpacing.Y,-_nodeSpacing.Z);
	p_constants[3]  = FVector(-_nodeSpacing.X,		 	 0.0f,-_nodeSpacing.Z);
	p_constants[4]  = FVector(           0.0f,           0.0f,-_nodeSpacing.Z);
	p_constants[5]  = FVector( _nodeSpacing.X,		     0.0f,-_nodeSpacing.Z);
	p_constants[6]  = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);
	p_constants[7]  = FVector(		     0.0f,-_nodeSpacing.Y,-_nodeSpacing.Z);
	p_constants[8]  = FVector( _nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);

	p_constants[9]  = FVector(-_nodeSpacing.X, _nodeSpacing.Y,			 0.0f);
	p_constants[10] = FVector(		     0.0f, _nodeSpacing.Y,			 0.0f);
	p_constants[11] = FVector( _nodeSpacing.X, _nodeSpacing.Y,			 0.0f);
	p_constants[12] = FVector(-_nodeSpacing.X,			 0.0f,			 0.0f);
	//MIDDLE OF FVector(0,0,0);
	p_constants[13] = FVector( _nodeSpacing.X,			 0.0f,			 0.0f);
	p_constants[14] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,			 0.0f);
	p_constants[15] = FVector(		     0.0f,-_nodeSpacing.Y,			 0.0f);
	p_constants[16] = FVector( _nodeSpacing.X,-_nodeSpacing.Y,			 0.0f);

	p_constants[17] = FVector(-_nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	p_constants[18] = FVector(		     0.0f, _nodeSpacing.Y, _nodeSpacing.Z);
	p_constants[19] = FVector( _nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	p_constants[20] = FVector(-_nodeSpacing.X,			 0.0f, _nodeSpacing.Z);
	p_constants[21] = FVector(		     0.0f,			 0.0f, _nodeSpacing.Z);
	p_constants[22] = FVector(_nodeSpacing.X,			 0.0f, _nodeSpacing.Z);
	p_constants[23] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
	p_constants[24] = FVector(		     0.0f,-_nodeSpacing.Y, _nodeSpacing.Z);
	p_constants[25] = FVector( _nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
}

