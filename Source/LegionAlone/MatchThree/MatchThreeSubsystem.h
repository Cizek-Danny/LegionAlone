#pragma once

#include "CoreMinimal.h"
#include "Cube.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TileDefinition.h"
#include "MatchThreeSubsystem.generated.h"

UCLASS()
class LEGIONALONE_API UMatchThreeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnVisualActionDispatchedSignature, const FMatchThreeFXResponse&);

public:
	UFUNCTION(BlueprintPure, Category = "MatchThree", meta = (WorldContext = "WorldContextObject"))
	static UMatchThreeSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "MatchThree")
	UMatchThreeTileDefinition* GetTileDefinition(EMatchThreeTileType TileType) const;

	void InitializeCubeData();
	const TMap<EMatchThreeTileType, UMatchThreeTileDefinition*>& GetAllTileDefinitions() const;

	bool TrySwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB);

	int32 GetActiveFaceNumber() const;
	int32 GetActiveFaceSize() const;
	EMatchThreeTileType GetTileTypeAt(const FMatchThreeCoordinate& Coord) const;

	FOnVisualActionDispatchedSignature OnVisualActionDispatchedEvent;

private:
	FMatchThreeCube CubeData;
	
	UPROPERTY()
	mutable TMap<EMatchThreeTileType, UMatchThreeTileDefinition*> LoadedTileDefinitions;

	struct FMatchTurnScope;
	void PaceAndBroadcastFXResponses(TArray<FMatchThreeFXResponse>& VisualActions) const;

	// TODO DJC: Using a lazy loading pattern for now. Going to make a proper asset streaming pipeline soon.
	// proper pipeline probably involves a main menu screen, some pre-loading, etc.
	mutable bool bHasLoadedDefinitions = false;
	void EnsureDefinitionsLoaded() const;
};
