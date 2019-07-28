// Rommulluss Caraiman 

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
	
	if (AEndActor != nullptr)
	{
		if (FStartNodeExtent == FVector(0.0f))
		{
			FStartNodeExtent = this->GetOwner()->GetComponentsBoundingBox(false).GetSize() / 2;
		}

		SuccessorPositions = FSuccessorPositions(FStartNodeExtent*2+.01f);
		AAStarNode* SpawnedNode = nullptr;	
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		FRotator rotator(0.0f);
		FVector spawnLocation;
		ANodePriority.Reserve(125000); //reserve approximately 1mb array of node addresses for priority queue heap

		spawnLocation = GetOwner()->GetActorLocation();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
		//SpawnedNode->BoxComponent->SetBoxExtent(FStartNodeExtent);
		SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;
		this->StartNode = SpawnedNode;	
		StartNode->depth = 0;
		StartNode->gCost = 0;
		StartNode->hCost = StartNode->GetDistanceTo(EndNode);
		StartNode->fCost = StartNode->gCost + StartNode->hCost;
		ANodePriority.HeapPush(SpawnedNode, SORTING_PREDICATE_LAMBDA);
		SpawnedNode = nullptr;
		
		UMeshComponent* SpawnedMeshComponentEnd = dynamic_cast<UMeshComponent*>(AEndActor->GetComponentByClass(UMeshComponent::StaticClass()));
		UMeshComponent* SpawnedMeshComponentStart = dynamic_cast<UMeshComponent*>(AEndActor->GetComponentByClass(UMeshComponent::StaticClass()));
		if (SpawnedMeshComponentEnd !=nullptr)
		{
			SpawnedMeshComponentEnd->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
			SpawnedMeshComponentStart->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

			spawnLocation = AEndActor->GetActorLocation();
			SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
			SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;

			if (FEndNodeExtent == FVector(0.0f))
			{
				FEndNodeExtent = AEndActor->GetComponentsBoundingBox(false).GetSize() / 2;
			}

			SpawnedNode->BoxComponent->SetBoxExtent(FEndNodeExtent);			
			this->EndNode = SpawnedNode;
			EndNode->hCost = 0;			
			ANodePathList.Add(EndNode);
			bPathfindingRequired = true;
		}
		else
		{
			bPathfindingRequired = false;
		}		
	}
}

void UAStarPathFindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
		if (bPathfindingRequired&&ANodePriority.Num() != 0&&fin ==false)
		{		
			if (ANodePriority.HeapTop() != EndNode)
			{
				SpawnNodes();
				Algorithm();
			}
			else
			{
				while (ANodePathList.Last() != StartNode)
				{
					ANodePathList.Last()->bNodeDrawExtent = bDrawPathBoxExtents;
					ANodePathList.Last()->ExtentsColor = FColor::Cyan;
					ANodePathList.Add(ANodePathList.Last()->ParentNode);
				}
			StartNode->bNodeDrawExtent = bDrawPathBoxExtents;
			StartNode->ExtentsColor = FColor::Cyan;
			bPathfindingRequired = false;	
			
			travelToNextNode = ANodePathList.Pop();
			}			
		}
		
		if (!bPathfindingRequired&&fin==false)
		{
			if (this->GetOwner()->GetDistanceTo(travelToNextNode) >= 10.0f )
			{
				this->GetOwner()->SetActorLocation(this->GetOwner()->GetActorLocation() + moveVector);

			}
			else if (travelToNextNode != EndNode)
			{
				travelToNextNode = ANodePathList.Pop();
				FVector vec = travelToNextNode->GetActorLocation() - this->GetOwner()->GetActorLocation();				
				moveVector = 10.0f * vec/ vec.Size();
			}			
			else
			{
				this->GetOwner()->SetActorLocation(EndNode->GetActorLocation());
				fin = true;

			}

		}
}
/*
bool UAStarPathFindingComponent::IsEndInSight(AAStarNode* _node)
{
	FHitResult OutHit;
	FVector startActorLocation =_node->GetActorLocation();
	FVector endActorLocation = AEndActor->GetActorLocation();
	FCollisionQueryParams QueryParam;
	GetWorld()->SweepSingleByChannel(OutHit, startActorLocation, endActorLocation, FQuat::Identity, ECollisionChannel::ECC_Pawn, StartNode->BoxComponent->GetCollisionShape(), QueryParam);
	if (OutHit.Actor == AEndActor) 
		return true;	
	else 
		return false;	
}
*/
void  UAStarPathFindingComponent::Algorithm()
{
	AAStarNode* CurrentNode = nullptr;
	ANodePriority.HeapPop(CurrentNode, SORTING_PREDICATE_LAMBDA);
	/*
	if (IsEndInSight(CurrentNode))
	{
		EndNode->gCost = CurrentNode->gCost + CurrentNode->GetDistanceTo(EndNode);
		EndNode->fCost = EndNode->gCost + EndNode->hCost;
		ANodePriority.HeapPush(EndNode, SORTING_PREDICATE_LAMBDA);
		EndNode->ParentNode = CurrentNode;			
	}	
	*/
	for (int i = 0; i < CurrentNode->ArrayNeighbours.Num(); i++)
	{
		float Distance = CurrentNode->GetDistanceTo(CurrentNode->ArrayNeighbours[i]);

		if ((CurrentNode->gCost + Distance) < CurrentNode->ArrayNeighbours[i]->gCost)
		{
			CurrentNode->ArrayNeighbours[i]->depth = CurrentNode->depth + 1;
			CurrentNode->ArrayNeighbours[i]->ParentNode = CurrentNode;
			CurrentNode->ArrayNeighbours[i]->gCost = CurrentNode->gCost + (Distance);
			CurrentNode->ArrayNeighbours[i]->hCost = CurrentNode->ArrayNeighbours[i]->GetDistanceTo(EndNode);
			//CurrentNode->ArrayNeighbours[i]->fCost = CurrentNode->ArrayNeighbours[i]->gCost + (CurrentNode->ArrayNeighbours[i]->hCost); //Admissable
			CurrentNode->ArrayNeighbours[i]->fCost = CurrentNode->ArrayNeighbours[i]->gCost + (3*CurrentNode->ArrayNeighbours[i]->hCost); //A*/Static Weighting
			ANodePriority.HeapPush(CurrentNode->ArrayNeighbours[i], SORTING_PREDICATE_LAMBDA);
		}		
	}
}
void  UAStarPathFindingComponent::SpawnNodes()
{//SNPAWNS AND CHECKS NEIGHBOURS INTO LIST

	SuccessorPositions.AssignNewPositions(ANodePriority.HeapTop()->GetActorLocation());
	UWorld* world = GetWorld();

	for (int i = 0; i < SuccessorPositions.FVectorSuccessors.Num(); i++)
	{
		AAStarNode* SpawnedNode = nullptr;
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Template = NULL;

		FRotator rotator(0.0f);
		FVector spawnLocation = SuccessorPositions.FVectorSuccessors[i];
		SpawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
		
		if (SpawnedNode != nullptr)
		{			
			TArray<AActor*> OverlappingActors;			
			SpawnedNode->GetOverlappingActors(OverlappingActors, AAStarNode::StaticClass());
			if (OverlappingActors.Num() == 0)
			{						
				SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;				
				ANodePriority.HeapTop()->ArrayNeighbours.Add(SpawnedNode);							
			}
			else
			{
				for (int i = 0; i < OverlappingActors.Num(); i++)
				{
					if(OverlappingActors[i]->GetClass() == AAStarNode::StaticClass())
						ANodePriority.HeapTop()->ArrayNeighbours.AddUnique(StaticCast<AAStarNode*>(OverlappingActors[i]));					
				}
				
				this->GetWorld()->DestroyActor(SpawnedNode);
			}
		}
	}
}

void  UAStarPathFindingComponent::FSuccessorPositions::AssignConstants(FVector _nodeSpacing)
{
	FVectorConstants.SetNum(26);	

	FVectorConstants[0]  = FVector(-_nodeSpacing.X, _nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[1]  = FVector(			  0.0f, _nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[2]  = FVector( _nodeSpacing.X, _nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[3]  = FVector(-_nodeSpacing.X,			  0.0f,-_nodeSpacing.Z);
	FVectorConstants[4]  = FVector(			  0.0f,			  0.0f,-_nodeSpacing.Z);
	FVectorConstants[5]  = FVector( _nodeSpacing.X,			  0.0f,-_nodeSpacing.Z);
	FVectorConstants[6]  = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[7]  = FVector(			  0.0f,-_nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[8]  = FVector( _nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);

	FVectorConstants[9]  = FVector(-_nodeSpacing.X, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[10] = FVector(			  0.0f, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[11] = FVector( _nodeSpacing.X, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[12] = FVector(-_nodeSpacing.X,			  0.0f,			  0.0f);
	//Position of actor    FVector(				 0,				0,			     0);
	FVectorConstants[13] = FVector( _nodeSpacing.X,			  0.0f,			  0.0f);
	FVectorConstants[14] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,			  0.0f);
	FVectorConstants[15] = FVector(			  0.0f,-_nodeSpacing.Y,			  0.0f);
	FVectorConstants[16] = FVector( _nodeSpacing.X,-_nodeSpacing.Y,			  0.0f);

	FVectorConstants[17] = FVector(-_nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[18] = FVector(			  0.0f, _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[19] = FVector( _nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[20] = FVector(-_nodeSpacing.X,			  0.0f, _nodeSpacing.Z);
	FVectorConstants[21] = FVector(			  0.0f,			  0.0f, _nodeSpacing.Z);
	FVectorConstants[22] = FVector( _nodeSpacing.X,		      0.0f, _nodeSpacing.Z);
	FVectorConstants[23] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[24] = FVector(			  0.0f,-_nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[25] = FVector( _nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
}
