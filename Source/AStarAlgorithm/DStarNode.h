// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DStarNode.generated.h"

UCLASS()
class ASTARALGORITHM_API ADStarNode : public AActor
{
	GENERATED_BODY()

private:
	USceneComponent* SceneComponent;
public:	
	// Sets default values for this actor's properties
	ADStarNode();

	bool bNodeDrawExtent = false;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxComponent;

	
	TTuple<float, float> operator==(ADStarNode) { return this->key; };
	
	
	//rhs = cost of parentNode + Distance to that node
	//rhs = rhx(x) = g(x') + c(x',x)
	// g(x) == rhs(x) - locally consistent
	// g(x) != rhs(x) - locally inconsistent | g(x)<rhx(x) - underconsistency = more expensive| g(x)>rhs(x) - overconsistency = less expensive

	UPROPERTY(VisibleAnywhere)
		float rhs=INFINITY;

	UPROPERTY(VisibleAnywhere)
		float g = INFINITY;


	/*UPROPERTY(VisibleAnywhere)
		float h = INFINITY;*/

	//Tuple = [min(g(s),rhs(s) + h(s)); min(g(s),rhs(s))];
	
	TTuple<float, float> key;

		TArray<ADStarNode*> Succ;
		TArray<ADStarNode*> Pred;
		

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
