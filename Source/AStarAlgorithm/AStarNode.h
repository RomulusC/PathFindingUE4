// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Collision.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNode.generated.h"



UCLASS()
class ASTARALGORITHM_API AAStarNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAStarNode();


	USceneComponent* Scene;

	bool b_VisualNodes = false;

		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	AAStarNode* ParentNode;
	TArray<AAStarNode*> ArrayNeighbours;

	UPROPERTY(VisibleAnywhere)
		float lCost = INFINITY;
	UPROPERTY(VisibleAnywhere)
		float gCost = INFINITY;
	UPROPERTY(VisibleAnywhere)
		float fCost = INFINITY;

	bool isVisited = false;
	
	



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	
};
