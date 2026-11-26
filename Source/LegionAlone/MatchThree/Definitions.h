#pragma once

#include "Constants.h"

struct FMatchThreeCoordinate
{
	int32 Face = -1;
	int32 Row = -1;
	int32 Col = -1;
};

struct FMatchThreeConsumptionResult
{
	TMap<EMatchThreeTileType, int32> ConsumedCounts;
	TArray<FMatchThreeCoordinate> ConsumedCoordinates; 
	int32 TotalTilesCleared = 0;
};

struct FMatchThreeFallMove
{
	FMatchThreeCoordinate From;
	FMatchThreeCoordinate To;
	EMatchThreeTileType TileType = EMatchThreeTileType::Invalid;
	bool bIsNewSpawn = false;
};

struct FMatchThreeFallResult
{
	TArray<FMatchThreeFallMove> Moves;
};

enum class EMatchThreeBoardEventType : uint8
{
	Invalid = 0,
	Swap = 1,
	SwapReject = 2,
	Consume = 3,
	FallAndRefill = 4,
	SpawnSpecial = 5,
	BreakBoard = 6
};

struct FMatchThreeBoardEvent
{
	FMatchThreeConsumptionResult ConsumptionResult;
	FMatchThreeFallResult FallResult;
	FMatchThreeCoordinate CoordA;
	FMatchThreeCoordinate CoordB;
	EMatchThreeBoardEventType EventType = EMatchThreeBoardEventType::Invalid;
};

struct FMatchThreeFXResponse
{
	TArray<FMatchThreeCoordinate> CoordinatesToHide;
	FMatchThreeFallResult FallResult;
	FMatchThreeCoordinate CoordA;
	FMatchThreeCoordinate CoordB;
	float Duration = 0.25f;
	int32 CascadeStep = 1;
	int32 PointsAwarded = 0;
	EMatchThreeBoardEventType ActionType = EMatchThreeBoardEventType::Invalid;
};

