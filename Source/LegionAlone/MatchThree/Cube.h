#pragma once
#include "Face.h"

class FMatchThreeCube
{
public:
	FMatchThreeCube() = default;
	
	int32 GetActiveFaceNumber() const;
	int32 GetNumFaces() const;
	int32 GetFaceSize(int32 FaceIndex) const;
	EMatchThreeTileType GetTileTypeAt(const FMatchThreeCoordinate& Coord) const;

	void Initialize();
    
	void TrySwapTiles(
		const FMatchThreeCoordinate& CoordA,
		const FMatchThreeCoordinate& CoordB,
		TFunctionRef<void(const FMatchThreeBoardEvent&)> OnBoardEvent
	);

private:
	TStaticArray<FMatchThreeFace, 6> Faces;
	EMatchThreeFaceType ActiveFace = EMatchThreeFaceType::Mana;
};
