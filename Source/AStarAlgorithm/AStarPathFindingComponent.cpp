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
		if (FStartNodeExtent ==FVector(0.0f))
		{
			FStartNodeExtent = this->GetOwner()->GetComponentsBoundingBox(false).GetSize() / 2;
		}		

		SuccessorPositions = FSuccessorPositions((FStartNodeExtent)*2);
		AAStarNode* SpawnedNode = nullptr;
		FActorSpawnParameters SpawnParams;
		
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator rotator(0.0f);
		UWorld* World = GetWorld();
		FVector spawnLocation = GetOwner()->GetActorLocation();

		SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
		this->StartNode = SpawnedNode;
		
		SpawnedNode->BoxComponent->SetBoxExtent(FStartNodeExtent);
		SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;		
		ANodePriority.HeapPush(SpawnedNode, [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });

		SpawnedNode = nullptr;
		spawnLocation = AEndActor->GetActorLocation();
		SpawnedNode = World->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);
		this->EndNode = SpawnedNode;
		SpawnedNode->bNodeDrawExtent = bDrawBoxExtents;	

		if (FEndNodeExtent==FVector(0.0f))
		{
			FEndNodeExtent = AEndActor->GetComponentsBoundingBox(false).GetSize()/2;			
		}

		SpawnedNode->BoxComponent->SetBoxExtent(FEndNodeExtent);
		ANodePathList.AddHead(SpawnedNode);

		ANodePriority.HeapTop()->lCost = 0;
		ANodePriority.HeapTop()->gCost =ANodePriority.HeapTop()->GetSquaredDistanceTo(ANodePathList.GetHead()->GetValue());
		ANodePriority.HeapTop()->fCost =ANodePriority.HeapTop()->gCost +ANodePriority.HeapTop()->lCost;

		ANodePathList.GetHead()->GetValue()->gCost = 0;

		//TODO: Set up Start and End AActor's collision components to overlap NodeObjectChannel (CustomObjectCollisionChannel)
		//UBoxComponent* SpawnedBoxComponent =StaticCast<UBoxComponent*>( AEndActor->GetComponentByClass(UBoxComponent::StaticClass()));		
		//SpawnedBoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1); 

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
				ANodePathList.GetHead()->GetValue()->bNodeDrawExtent=true;
				ANodePathList.AddHead(ANodePathList.GetHead()->GetValue()->ParentNode);			
			}
			bPathfindingRequired = false;
		}
	}	
}

void  UAStarPathFindingComponent::Algorithm()
{
	 AAStarNode* CurrentNode=nullptr;
	 ANodePriority.HeapPop(CurrentNode, [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });
	
	for (int i = 0; i < CurrentNode->ArrayNeighbours.Num(); i++)
	{
		float Distance = CurrentNode->GetSquaredDistanceTo(CurrentNode->ArrayNeighbours[i]);

		if ((CurrentNode->lCost + Distance) < CurrentNode->ArrayNeighbours[i]->lCost)
		{
			CurrentNode->ArrayNeighbours[i]->ParentNode = CurrentNode;
			CurrentNode->ArrayNeighbours[i]->lCost =CurrentNode->lCost + Distance;
			CurrentNode->ArrayNeighbours[i]->gCost =CurrentNode->ArrayNeighbours[i]->GetSquaredDistanceTo(ANodePathList.GetHead()->GetValue());
			CurrentNode->ArrayNeighbours[i]->fCost =CurrentNode->ArrayNeighbours[i]->lCost + CurrentNode->ArrayNeighbours[i]->gCost;	
			
			ANodePriority.HeapPush(CurrentNode->ArrayNeighbours[i], [](AAStarNode& a, AAStarNode& b) {return a.gCost < b.gCost; });			
		}
	}		
}

void  UAStarPathFindingComponent::SpawnNodes()
{//SNPAWNS AND CHECKS NEIGHBOURS INTO LIST
	
	SuccessorPositions.AssignNewPositions(ANodePriority.HeapTop()->GetActorLocation());
	UWorld* world = GetWorld();	
	
	for (int i = 0; i< SuccessorPositions.FVectorSuccessors.Num(); i++)
	{
		AAStarNode* SpawnedNode = nullptr;
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;		
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Template = NULL;
		
		FRotator rotator(0.0f);
		FVector spawnLocation = SuccessorPositions.FVectorSuccessors[i];
		SpawnedNode = world->SpawnActor<AAStarNode>(AAStarNode::StaticClass(), spawnLocation, rotator, SpawnParams);	

		if (SpawnedNode!=nullptr)
		{			
			TArray<AActor*> OverlappingActors;
			SpawnedNode->GetOverlappingActors(OverlappingActors, AAStarNode::StaticClass());
			if (OverlappingActors.Num()==0)
			{
				SpawnedNode->BoxComponent->SetBoxExtent(FEndNodeExtent);
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
	FVectorConstants[1]  = FVector(           0.0f, _nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[2]  = FVector( _nodeSpacing.X, _nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[3]  = FVector(-_nodeSpacing.X,		 	  0.0f,-_nodeSpacing.Z);
	FVectorConstants[4]  = FVector(           0.0f,           0.0f,-_nodeSpacing.Z);
	FVectorConstants[5]  = FVector( _nodeSpacing.X,		      0.0f,-_nodeSpacing.Z);
	FVectorConstants[6]  = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[7]  = FVector(		      0.0f,-_nodeSpacing.Y,-_nodeSpacing.Z);
	FVectorConstants[8]  = FVector( _nodeSpacing.X,-_nodeSpacing.Y,-_nodeSpacing.Z);

	FVectorConstants[9]  = FVector(-_nodeSpacing.X, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[10] = FVector(		      0.0f, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[11] = FVector( _nodeSpacing.X, _nodeSpacing.Y,			  0.0f);
	FVectorConstants[12] = FVector(-_nodeSpacing.X,			  0.0f,			  0.0f);
	//MIDDLE OF FVector(0,0,0);
	FVectorConstants[13] = FVector( _nodeSpacing.X,			  0.0f,			  0.0f);
	FVectorConstants[14] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y,			  0.0f);
	FVectorConstants[15] = FVector(		      0.0f,-_nodeSpacing.Y,			  0.0f);
	FVectorConstants[16] = FVector( _nodeSpacing.X,-_nodeSpacing.Y,			  0.0f);

	FVectorConstants[17] = FVector(-_nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[18] = FVector(		     0.0f,  _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[19] = FVector( _nodeSpacing.X, _nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[20] = FVector(-_nodeSpacing.X,			  0.0f, _nodeSpacing.Z);
	FVectorConstants[21] = FVector(		      0.0f,			  0.0f, _nodeSpacing.Z);
	FVectorConstants[22] = FVector( _nodeSpacing.X,			  0.0f, _nodeSpacing.Z);
	FVectorConstants[23] = FVector(-_nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[24] = FVector(		      0.0f,-_nodeSpacing.Y, _nodeSpacing.Z);
	FVectorConstants[25] = FVector( _nodeSpacing.X,-_nodeSpacing.Y, _nodeSpacing.Z);
}

