#pragma once

#include "Tile.h"

class FMatchThreeFace
{
public:
	FMatchThreeFace() = default;
	explicit FMatchThreeFace(EMatchThreeFaceType FaceType,
								EMatchThreeFaceSize FaceSize);
	
	EMatchThreeTileType GetTileTypeAt(const FMatchThreeCoordinate& Coord) const;

private:
	friend class FMatchThreeCube;

	TArray<FMatchThreeTile> Tiles;
	EMatchThreeFaceType FaceType = EMatchThreeFaceType::Invalid;
	int32 FaceIndex = -1; // TODO DJC: Assign this at face creation time; corresponds to orientation on cube.
	int32 FaceSize;
	FRandomStream FaceRandomStream;

	void GenerateFace();

	int32 CountUpToTwoConsecutiveInDirection(EMatchThreeTileType Type, const FMatchThreeCoordinate& SourceCoord,
											 const FMatchThreeCoordinate& StartCoord, int32 RowDir, int32 ColDir);
    
	bool CanSwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB);
	void SwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB);

	void FlagRunIfValid(int32 StartIdx, int32 EndIdx, int32 PerpendicularIndex, bool bIsRow, int32 MinLength = 3);
	void FlagMatchedTilesForConsumption();
	FMatchThreeConsumptionResult ConsumeFlaggedTiles();
    
	uint8 GetAvailableTileTypesMask(const FMatchThreeCoordinate& Coord);
	EMatchThreeTileType GenerateRandomTileTypeForCell(const FMatchThreeCoordinate& Coord);
	FMatchThreeFallResult ProcessGravityAndRefill();
};
