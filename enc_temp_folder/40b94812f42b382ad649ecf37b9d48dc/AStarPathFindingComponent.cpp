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

		SuccessorPositions = FSuccessorPositions(FStartNodeExtent*2);
		AAStarNode* SpawnedNode = nullptr;	
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		FRotator rotator(0.0f);
		FVector spawnLocation;
		ANodePriority.Reserve(125000); //reserve approximately 1mb array of node addresses in priority queue heap arrangement

		spawnLocation = GetOwner()->GetActorLocation();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
		SpawnedNode->BoxComponent->SetBoxExtent(FStartNodeExtent);
		SpawnedNode->bNodeDrawExtent = !bDrawBoxExtents;
		this->StartNode = SpawnedNode;		
		StartNode->gCost = 0;
		StartNode->hCost = StartNode->GetSquaredDistanceTo(EndNode);
		StartNode->fCost = StartNode->gCost + StartNode->hCost;
		ANodePriority.HeapPush(SpawnedNode, SortingPredicate);
		SpawnedNode = nullptr;

		FHitResult HitResult = PerformInitialRaycast();
		if (HitResult.IsValidBlockingHit())
		{
			spawnLocation = HitResult.ImpactPoint + (HitResult.ImpactNormal * FStartNodeExtent*1.2);
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
			SpawnedNode->BoxComponent->SetBoxExtent(FStartNodeExtent);
			SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;	

			SpawnedNode->gCost = SpawnedNode->GetSquaredDistanceTo(StartNode);
			SpawnedNode->hCost = SpawnedNode->GetSquaredDistanceTo(AEndActor);
			SpawnedNode->fCost = SpawnedNode->gCost + SpawnedNode->hCost;
			SpawnedNode->ParentNode = StartNode;
			ANodePriority.HeapPush(SpawnedNode, SortingPredicate);
			SpawnedNode = nullptr;
		}		
		
		UMeshComponent* SpawnedMeshComponent = StaticCast<UMeshComponent*>(AEndActor->GetComponentByClass(UMeshComponent::StaticClass()));
		SpawnedMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);			
		spawnLocation = AEndActor->GetActorLocation();
		SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);		
		SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;

		if (FEndNodeExtent == FVector(0.0f))
		{
			FEndNodeExtent = AEndActor->GetComponentsBoundingBox(false).GetSize() / 2;
		}

		SpawnedNode->BoxComponent->SetBoxExtent(FEndNodeExtent);
		ANodePathList.AddHead(SpawnedNode);
		this->EndNode = SpawnedNode;	

		EndNode->hCost = 0;

		SpawnedMeshComponent = StaticCast<UMeshComponent*>(AEndActor->GetComponentByClass(UMeshComponent::StaticClass()));
		SpawnedMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);		
		bPathfindingRequired = true;
		
	}
}

// Called every frame
void UAStarPathFindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		if (bPathfindingRequired)
		{		
		if (ANodePriority.HeapTop() != EndNode)
		{
			SpawnNodes();
			Algorithm();
		}
		else
		{
			while (ANodePathList.GetHead()->GetValue() != StartNode)
			{
				ANodePathList.GetHead()->GetValue()->bNodeDrawExtent = bDrawPathBoxExtents;
				ANodePathList.GetHead()->GetValue()->ExtentsColor = FColor::Cyan;
				ANodePathList.AddHead(ANodePathList.GetHead()->GetValue()->ParentNode);
			}
			bPathfindingRequired = false;
		}
	}
}
FHitResult UAStarPathFindingComponent::PerformInitialRaycast()
{
	FHitResult OutHit;
	FVector startActorLocation = this->GetOwner()->GetActorLocation();
	FVector endActorLocation = AEndActor->GetActorLocation();
	GetWorld()->SweepSingleByChannel(OutHit,startActorLocation,endActorLocation, this->GetOwner()->GetActorQuat(), ECollisionChannel::ECC_Visibility,StartNode->BoxComponent->GetCollisionShape(), ECR_Block);
	//GetWorld()->LineTraceSingleByChannel(OutHit, startActorLocation, endActorLocation, ECollisionChannel::ECC_Visibility, ECR_Block);
	
		return OutHit;
}
bool UAStarPathFindingComponent::IsEndInSight(AAStarNode* _node)
{
	FHitResult OutHit;
	FVector startActorLocation =_node->GetActorLocation();
	FVector endActorLocation = AEndActor->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(OutHit, startActorLocation, endActorLocation, ECollisionChannel::ECC_Visibility, ECR_Block);
	if (OutHit.Actor== AEndActor)
	{
		return true;
	}
	else
	{
		return false;
	}

}
void  UAStarPathFindingComponent::Algorithm()
{
	AAStarNode* CurrentNode = nullptr;
	ANodePriority.HeapPop(CurrentNode, SortingPredicate);

	if (IsEndInSight(CurrentNode))
	{
		ANodePriority.HeapPush(EndNode, SortingPredicate);
		EndNode->ParentNode = CurrentNode;
		
	}	
	for (int i = 0; i < CurrentNode->ArrayNeighbours.Num(); i++)
	{
		float Distance = CurrentNode->GetSquaredDistanceTo(CurrentNode->ArrayNeighbours[i]);

		if ((CurrentNode->gCost + Distance) < CurrentNode->ArrayNeighbours[i]->gCost)
		{
			CurrentNode->ArrayNeighbours[i]->ParentNode = CurrentNode;
			CurrentNode->ArrayNeighbours[i]->gCost = CurrentNode->gCost + (Distance);
			CurrentNode->ArrayNeighbours[i]->hCost = CurrentNode->ArrayNeighbours[i]->GetSquaredDistanceTo(EndNode);
			CurrentNode->ArrayNeighbours[i]->fCost = CurrentNode->ArrayNeighbours[i]->gCost + CurrentNode->ArrayNeighbours[i]->hCost;

			ANodePriority.HeapPush(CurrentNode->ArrayNeighbours[i], SortingPredicate);
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
				SpawnedNode->BoxComponent->SetBoxExtent(FStartNodeExtent);
				SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;				
				ANodePriority.HeapTop()->ArrayNeighbours.Add(SpawnedNode);							
			}
			else
			{
				for (int i = 0; i < OverlappingActors.Num(); i++)
				{//maybe check for collision with area > some value

					ANodePriority.HeapTop()->ArrayNeighbours.Add(StaticCast<AAStarNode*>(OverlappingActors[i]));
				}
				SpawnedNode->Destroy();
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
