#pragma once

#include "CoreMinimal.h"
#include "Cube.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

UCLASS()
class LEGIONALONE_API AMatchThreeCubeActor : public AActor
{
	GENERATED_BODY()
	
public:
	AMatchThreeCubeActor();
	void InitializeCube();
	
	UFUNCTION(BlueprintCallable, Category = "MatchThree")
	void HandleInteractionPlaneClick(FVector const HitLocation, UPrimitiveComponent* InteractionPlane);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	int32 SelectedRow = -1;
	int32 SelectedCol = -1;
	EMatchThreeFaceType ActiveFace = EMatchThreeFaceType::Mana;
	bool bHasSelection = false;
	
	struct FVisualIndex
	{
		EMatchThreeTileType TileType;
		int32 InstanceIndex;
	};
	struct FFaceGridData
	{
		int32 FaceSize = 0;
		TArray<FVisualIndex> Tiles;
	};
	TStaticArray<FFaceGridData, 6> VisualIndexArrays;
	
	UPROPERTY()
	TMap<EMatchThreeTileType, UHierarchicalInstancedStaticMeshComponent*> TileHISMMap;
	
	static FTransform CalculateTileTransform(const FMatchThreeCoordinate& Coord, int32 FaceSize);
	void InitializeCubeVisuals();
	
	TArray<FMatchThreeFXResponse> VisualActionQueue;
	int32 VisualQueueHeadIndex = 0;
	bool bIsPlayingVisualAction = false;
	FTimerHandle VisualActionTimerHandle;
	TMap<EMatchThreeTileType, TArray<int32>> HiddenInstancePools;
	
	void OnVisualActionDispatched(const FMatchThreeFXResponse& Action);
	void PlayNextVisualAction();
	
	void ExecuteSwapVisual(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB);
	void ExecuteConsumeVisual(const TArray<FMatchThreeCoordinate>& ConsumedCoordinates);
	void ExecuteFallAndRefillVisual(const FMatchThreeFallResult& FallResult);
};
