#include "Cube.h"

int32 FMatchThreeCube::GetActiveFaceNumber() const
{
	return static_cast<int32>(ActiveFace) - 1;
}

int32 FMatchThreeCube::GetNumFaces() const
{
	return Faces.Num();
}

int32 FMatchThreeCube::GetFaceSize(int32 const FaceIndex) const
{
	return Faces[FaceIndex].FaceSize;
}

EMatchThreeTileType FMatchThreeCube::GetTileTypeAt(const FMatchThreeCoordinate& Coord) const
{
	return Faces[Coord.Face].GetTileTypeAt(Coord);
}

void FMatchThreeCube::Initialize()
{
	for (int32 i = 0; i < 6; ++i)
	{
		Faces[i].FMatchThreeFace::FMatchThreeFace(static_cast<EMatchThreeFaceType>(i + 1), EMatchThreeFaceSize::Medium);
	}
}

void FMatchThreeCube::TrySwapTiles(
    const FMatchThreeCoordinate& CoordA,
    const FMatchThreeCoordinate& CoordB,
    TFunctionRef<void(const FMatchThreeBoardEvent&)> OnBoardEvent)
{
    // TODO DJC: Consider more advanced logic for when to detect and resolve matches
    // For instance, should probably consider such logic just after the board has changed, but only for the local delta
    // Knowingly implementing the naive solution just to get moving here
    // Ultimately, we'll probably have UMatchThreeSubsystem pass down some sort of rule configuration data followed here

    if (CoordA.Face != CoordB.Face) // TODO DJC: Consider cross face swapping
    {
        return;
    }

    if (CoordA.Row == CoordB.Row && CoordA.Col == CoordB.Col)
    {
        return;
    }

    if (FMath::Abs(CoordA.Row - CoordB.Row) + FMath::Abs(CoordA.Col - CoordB.Col) != 1) // Non-adjacent tiles
    {
        return;
    }

    FMatchThreeFace& TargetFace = Faces[CoordA.Face];

    if (!TargetFace.CanSwapTiles(CoordA, CoordB))
    {
        FMatchThreeBoardEvent RejectEvent;
        RejectEvent.EventType = EMatchThreeBoardEventType::SwapReject;
        RejectEvent.CoordA = CoordA;
        RejectEvent.CoordB = CoordB;
        OnBoardEvent(RejectEvent);
        return;
    }

    TargetFace.SwapTiles(CoordA, CoordB);
    FMatchThreeBoardEvent SwapEvent;
    SwapEvent.EventType = EMatchThreeBoardEventType::Swap;
    SwapEvent.CoordA = CoordA;
    SwapEvent.CoordB = CoordB;
    OnBoardEvent(SwapEvent);

    bool bDoneSettling = false;
    while (!bDoneSettling)
    {
        TargetFace.FlagMatchedTilesForConsumption();

        if (FMatchThreeConsumptionResult Consumption = TargetFace.ConsumeFlaggedTiles();
            Consumption.TotalTilesCleared > 0)
        {
            FMatchThreeBoardEvent ConsumeEvent;
            ConsumeEvent.EventType = EMatchThreeBoardEventType::Consume;
            ConsumeEvent.ConsumptionResult = MoveTemp(Consumption);
            OnBoardEvent(ConsumeEvent);

            if (FMatchThreeFallResult FallResult = TargetFace.ProcessGravityAndRefill();
                FallResult.Moves.Num() > 0)
            {
                FMatchThreeBoardEvent FallEvent;
                FallEvent.EventType = EMatchThreeBoardEventType::FallAndRefill;
                FallEvent.FallResult = MoveTemp(FallResult);
                OnBoardEvent(FallEvent);
            }
        }
        else
        {
            bDoneSettling = true;
        }
    }
}