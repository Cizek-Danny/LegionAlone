#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Constants.h"
#include "TileDefinition.generated.h"

UCLASS(Blueprintable, BlueprintType)
class LEGIONALONE_API UMatchThreeTileDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("MatchThreeTile"), GetFName());
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Gameplay")
	EMatchThreeTileType TileType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Gameplay")
	int32 ResourceYield = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Presentation", meta = (AssetBundles = "Visuals"))
	TSoftObjectPtr<UStaticMesh> TileMesh;
	UPROPERTY(Transient)
	UStaticMesh* CachedTileMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Presentation", meta = (AssetBundles = "Visuals"))
	TSoftObjectPtr<UMaterialInterface> TileMaterial;
	UPROPERTY(Transient)
	UMaterialInterface* CachedTileMaterial = nullptr;
	
	// TODO DJC: Naturally, I'll have to come back and get these made and populated.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Presentation", meta = (AssetBundles = "Visuals"))
	// TSoftObjectPtr<UTexture2D> TileIcon;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Presentation", meta = (AssetBundles = "Visuals"))
	// TSoftObjectPtr<UAnimSequence> MatchAnimation;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MatchThree|Presentation", meta = (AssetBundles = "Visuals"))
	// TSoftObjectPtr<USoundBase> MatchSoundEffect;
};
