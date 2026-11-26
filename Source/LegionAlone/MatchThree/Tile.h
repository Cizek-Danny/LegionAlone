#pragma once

#include "Definitions.h"

class FMatchThreeTile
{
	friend class FMatchThreeFace;
    EMatchThreeTileType TileType = EMatchThreeTileType::Invalid;
	bool bAwaitingConsumption = false;
	FMatchThreeCoordinate Coordinate;
};
