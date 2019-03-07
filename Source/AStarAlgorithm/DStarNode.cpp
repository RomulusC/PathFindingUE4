// Fill out your copyright notice in the Description page of Project Settings.

#include "DStarNode.h"


// Sets default values
ADStarNode::ADStarNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADStarNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADStarNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


