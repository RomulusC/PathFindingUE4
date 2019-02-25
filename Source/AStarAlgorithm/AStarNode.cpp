// Fill out your copyright notice in the Description page of Project Settings.

#include "AStarNode.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"


// Sets default values
AAStarNode::AAStarNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	this->RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Component"));
	
	BoxCollision->SetGenerateOverlapEvents(true);	
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
	BoxCollision->bMultiBodyOverlap = true;
	BoxCollision->SetCanEverAffectNavigation(false);
	BoxCollision->SetUseCCD(true);
	BoxCollision->bMultiBodyOverlap = true;
	BoxCollision->SetCanEverAffectNavigation(false);
	BoxCollision->SetupAttachment(GetRootComponent());
	
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


// Called when the game starts or when spawned
void AAStarNode::BeginPlay()
{
	
	Super::BeginPlay();
	
	//check if wall
	
}

// Called every frame
void AAStarNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (b_VisualNodes)
	{
		DrawDebugBox(GetWorld(), this->GetActorLocation(), this->GetComponentsBoundingBox().GetExtent(), FColor::Green, true, 5);
		//DrawDebugSolidBox(GetWorld(), this->GetActorLocation(), this->GetComponentsBoundingBox().GetExtent(), FColor::Green, true, 5);
	}

}


