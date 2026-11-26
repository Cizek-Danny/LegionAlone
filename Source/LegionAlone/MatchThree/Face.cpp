#include "Face.h"

FMatchThreeFace::FMatchThreeFace( const EMatchThreeFaceType FaceType, const EMatchThreeFaceSize FaceSize )
 	: FaceType( FaceType )
 	, FaceSize( static_cast<int32>(FaceSize) )
 {
 	Tiles.SetNum( this->FaceSize * this->FaceSize );
 	FaceRandomStream.Initialize( 0 ); // TODO DJC: Update this initializer to use a save/run based master seed
 	GenerateFace();
 }

EMatchThreeTileType FMatchThreeFace::GetTileTypeAt(const FMatchThreeCoordinate& Coord) const
{
    return Tiles[Coord.Row * FaceSize + Coord.Col].TileType;
}

void FMatchThreeFace::GenerateFace()
{
    for (int32 Row = 0; Row < FaceSize; ++Row)
    {
        for (int32 Col = 0; Col < FaceSize; ++Col)
        {
            Tiles[Row * FaceSize + Col].Coordinate.Face = static_cast<int32>(FaceType); // TODO DJC: This needs fixing.
            Tiles[Row * FaceSize + Col].Coordinate.Row = Row;
            Tiles[Row * FaceSize + Col].Coordinate.Col = Col;
            
            constexpr uint8 FullMask = 0xFF;
            uint8 AvailableMask = FullMask;
            
            // Check Left 2, then Up 2, for streak and remove tile type from pool if found
            if (Col >= 2 && Tiles[Row * FaceSize + (Col - 1)].TileType == Tiles[Row * FaceSize + (Col - 2)].TileType)
            {
                AvailableMask &= ~static_cast<uint8>(Tiles[Row * FaceSize + (Col - 1)].TileType);
            }
            
            if (Row >= 2 && Tiles[(Row - 1) * FaceSize + Col].TileType == Tiles[(Row - 2) * FaceSize + Col].TileType)
            {
                AvailableMask &= ~static_cast<uint8>(Tiles[(Row - 1) * FaceSize + Col].TileType);
            }
            
            ensureMsgf(AvailableMask > 0, TEXT("Failed to find valid tile type during face generation."));
            
            // Count how many valid tile types are left in our bitmask
            const int32 TotalBits = FMath::CountBits(AvailableMask);
            const int32 RandomK = FaceRandomStream.RandHelper(TotalBits);
            
            // Iterate left, leave behind 0s, until we have the Kth valid tile type flag set and a rightward trail of 0s
            uint8 Temp = AvailableMask;
            int32 BitIndex = 0;
            for (int32 i = 0; i <= RandomK; ++i)
            {
                BitIndex = FPlatformMath::CountTrailingZeros(Temp);
                Temp &= (Temp - 1); // 1101 1010 ==> 1101 1000 
            }
            
            // Map the winning bit index back to its EMatchThreeTileType flag value
            Tiles[Row * FaceSize + Col].TileType = static_cast<EMatchThreeTileType>(1 << BitIndex);
        }
    }
}

int32 FMatchThreeFace::CountUpToTwoConsecutiveInDirection(EMatchThreeTileType const Type,
                                                            const FMatchThreeCoordinate& SourceCoord,
                                                            const FMatchThreeCoordinate& StartCoord,
                                                            int32 const RowDir, int32 const ColDir)
{
    int32 ConsecutiveTiles = 0;
    int32 RowCheck = StartCoord.Row + RowDir;
    int32 ColCheck = StartCoord.Col + ColDir;
    
    while (RowCheck >= 0 && RowCheck < FaceSize && ColCheck >= 0 && ColCheck < FaceSize)
    {
        if ( RowCheck == SourceCoord.Row && ColCheck == SourceCoord.Col)
        {
            break;
        }
        
        if (int32 const CheckIndex = RowCheck * FaceSize + ColCheck;
            Tiles[CheckIndex].TileType != Type)
        {
            return ConsecutiveTiles;
        }
        
        if (++ConsecutiveTiles > 1)
        {
            return ConsecutiveTiles;
        }
        
        RowCheck += RowDir;
        ColCheck += ColDir;
    }
    
    return ConsecutiveTiles;
}

bool FMatchThreeFace::CanSwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB)
{
    EMatchThreeTileType const TypeA = Tiles[CoordA.Row * FaceSize + CoordA.Col].TileType;
    EMatchThreeTileType const TypeB = Tiles[CoordB.Row * FaceSize + CoordB.Col].TileType;
    
    int32 Consecutive = 1 + CountUpToTwoConsecutiveInDirection( TypeA,CoordA, CoordB,0, -1);
    if (Consecutive > 2)
    {
        return true; // Left
    }
    
    Consecutive += CountUpToTwoConsecutiveInDirection(TypeA, CoordA, CoordB,  0,  1);
    if (Consecutive > 2)
    {
        return true; // Right
    }
    
    Consecutive = 1 + CountUpToTwoConsecutiveInDirection(TypeA, CoordA, CoordB, -1,  0);
    if (Consecutive > 2)
    {
        return true; // Up
    }
    
    Consecutive += CountUpToTwoConsecutiveInDirection(TypeA, CoordA, CoordB,  1,  0);
    if (Consecutive > 2)
    {
        return true; // Down
    }
    
    Consecutive = 1 + CountUpToTwoConsecutiveInDirection(TypeB, CoordB, CoordA,  0, -1);
    if (Consecutive > 2)
    {
        return true; // Left
    }
    
    Consecutive += CountUpToTwoConsecutiveInDirection(TypeB, CoordB, CoordA,  0,  1);
    if (Consecutive > 2)
    {
        return true; // Right
    }
    
    Consecutive = 1 + CountUpToTwoConsecutiveInDirection(TypeB, CoordB, CoordA, -1,  0);
    if (Consecutive > 2)
    {
        return true; // Up
    }
    
    Consecutive += CountUpToTwoConsecutiveInDirection(TypeB, CoordB, CoordA,  1,  0);
    if (Consecutive > 2)
    {
        return true; // Down
    }
    
    return false;
}

void FMatchThreeFace::SwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB)
{
    const int32 IndexA = CoordA.Row * FaceSize + CoordA.Col;
    const int32 IndexB = CoordB.Row * FaceSize + CoordB.Col;

    Tiles[IndexA].Coordinate = CoordB;
    Tiles[IndexB].Coordinate = CoordA;

    Tiles.Swap(IndexA, IndexB);
}

void FMatchThreeFace::FlagRunIfValid(int32 const StartIdx, int32 const EndIdx, int32 const PerpendicularIndex,
                                     bool const bIsRow, int32 const MinLength)
{
    if (EndIdx - StartIdx >= MinLength)
    {
        for (int32 i = StartIdx; i < EndIdx; ++i)
        {
            int32 const TargetIdx = bIsRow ? (PerpendicularIndex * FaceSize + i) : (i * FaceSize + PerpendicularIndex);
            Tiles[TargetIdx].bAwaitingConsumption = true;
        }
    }
}

void FMatchThreeFace::FlagMatchedTilesForConsumption()
{
    for (int32 Row = 0; Row < FaceSize; ++Row) // Find and flag horizontal matches
    {
        int32 const RowOffset = Row * FaceSize;
        int32 MatchStart = 0;

        for (int32 Col = 1; Col < FaceSize; ++Col)
        {
            if (Tiles[RowOffset + Col].TileType != Tiles[RowOffset + MatchStart].TileType)
            {
                FlagRunIfValid(MatchStart, Col, Row, true);
                MatchStart = Col;
            }
        }

        // Loop above misses sequences going all the way to the edge, so check for that possibility
        FlagRunIfValid(MatchStart, FaceSize, Row, true);
    }

    for (int32 Col = 0; Col < FaceSize; ++Col) // Vertical matches
    {
        int32 MatchStart = 0;

        for (int32 Row = 1; Row < FaceSize; ++Row)
        {
            if (Tiles[Row * FaceSize + Col].TileType != Tiles[MatchStart * FaceSize + Col].TileType)
            {
                FlagRunIfValid(MatchStart, Row, Col, false);
                MatchStart = Row;
            }
        }

        FlagRunIfValid(MatchStart, FaceSize, Col, false);
    }
}

FMatchThreeConsumptionResult FMatchThreeFace::ConsumeFlaggedTiles()
{
    FMatchThreeConsumptionResult Result;
    int32 const NumTiles = Tiles.Num();
    
    for (int32 i = 0; i < NumTiles; ++i)
    {
        if (Tiles[i].bAwaitingConsumption)
        {
            Result.ConsumedCounts.FindOrAdd(Tiles[i].TileType)++;
            Result.TotalTilesCleared++;

            // TODO DJC: Using default face here. Update once the cube can rotate
            Result.ConsumedCoordinates.Add(
                    FMatchThreeCoordinate{.Face = 0, .Row = Tiles[i].Coordinate.Row, .Col = Tiles[i].Coordinate.Col});
            
            Tiles[i].TileType = EMatchThreeTileType::Invalid;
            Tiles[i].bAwaitingConsumption = false;
        }
    }
    
    return Result; 
}

uint8 FMatchThreeFace::GetAvailableTileTypesMask(const FMatchThreeCoordinate& Coord)
{
    // TODO DJC: Current algorithm is attempting to never give a cascade. Switch off of this to a chance based approach.
    uint8 AvailableMask = 0xFF; // Start with all types available (assuming 8 bits for 8 types)

    // Test each possible tile type to see if placing it here would cause a streak of 3+
    for (uint8 Bit = 0; Bit < 8; ++Bit)
    {
        EMatchThreeTileType TestType = static_cast<EMatchThreeTileType>(1 << Bit);

        int32 const HorizCount = 1
            + CountUpToTwoConsecutiveInDirection(TestType, Coord, Coord, 0, -1) // Left
            + CountUpToTwoConsecutiveInDirection(TestType, Coord, Coord, 0, 1);  // Right

        int32 const VertCount = 1
            + CountUpToTwoConsecutiveInDirection(TestType, Coord, Coord, -1, 0) // Up
            + CountUpToTwoConsecutiveInDirection(TestType, Coord, Coord, 1, 0);  // Down

        // If placing this type creates 3 or more in a row anywhere, disqualify it
        if (HorizCount >= 3 || VertCount >= 3)
        {
            AvailableMask &= ~static_cast<uint8>(TestType);
        }
    }

    return AvailableMask;
}

EMatchThreeTileType FMatchThreeFace::GenerateRandomTileTypeForCell(const FMatchThreeCoordinate& Coord)
{
    uint8 AvailableMask = GetAvailableTileTypesMask(Coord);

    if (AvailableMask == 0)
    {
        AvailableMask = 0xFF;
    }

    const int32 TotalBits = FMath::CountBits(AvailableMask);
    const int32 RandomK = FaceRandomStream.RandHelper(TotalBits);

    uint8 Temp = AvailableMask;
    int32 BitIndex = 0;
    for (int32 i = 0; i <= RandomK; ++i)
    {
        BitIndex = FPlatformMath::CountTrailingZeros(Temp);
        Temp &= (Temp - 1);
    }

    return static_cast<EMatchThreeTileType>(1 << BitIndex);
}

FMatchThreeFallResult FMatchThreeFace::ProcessGravityAndRefill()
{
    // TODO DJC: Consider what you'll do to update this if/when you decide gravity is more complex than just 'go down'
    FMatchThreeFallResult FallResult;
    // int32 const FaceIndex = static_cast<int32>(FaceType); // TODO DJC: Update when you fix FaceIndex in constructor
    FaceIndex = static_cast<int32>(FaceType) - 1;

    // Process column by column
    for (int32 Col = 0; Col < FaceSize; ++Col)
    {
        // Start from the bottom row and work your way up
        for (int32 Row = FaceSize - 1; Row >= 0; --Row)
        {
            // If we found an empty space, pull the first available tile down from above
            if (int32 const CurrIdx = (Row * FaceSize) + Col;
                Tiles[CurrIdx].TileType == EMatchThreeTileType::Invalid)
            {
                int32 SourceRow = -1;
                for (int32 AboveRow = Row - 1; AboveRow >= 0; --AboveRow)
                {
                    if (int32 const AboveIdx = (AboveRow * FaceSize) + Col;
                        Tiles[AboveIdx].TileType != EMatchThreeTileType::Invalid)
                    {
                        SourceRow = AboveRow;
                        break;
                    }
                }

                if (SourceRow != -1)
                {
                    int32 const SourceIdx = (SourceRow * FaceSize) + Col;

                    Tiles[CurrIdx] = Tiles[SourceIdx];
                    Tiles[CurrIdx].Coordinate.Row = Row;
                    Tiles[CurrIdx].Coordinate.Col = Col;

                    Tiles[SourceIdx].TileType = EMatchThreeTileType::Invalid;
                    Tiles[SourceIdx].bAwaitingConsumption = false;

                    FallResult.Moves.Add(FMatchThreeFallMove{
                        .From = FMatchThreeCoordinate{.Face = FaceIndex, .Row = SourceRow, .Col = Col},
                        .To = FMatchThreeCoordinate{.Face = FaceIndex, .Row = Row, .Col = Col},
                        .TileType = Tiles[CurrIdx].TileType,
                        .bIsNewSpawn = false
                    });
                }
                else
                {
                    EMatchThreeTileType const NewType = GenerateRandomTileTypeForCell(FMatchThreeCoordinate{.Face = FaceIndex, .Row = Row, .Col = Col});

                    Tiles[CurrIdx].TileType = NewType;
                    Tiles[CurrIdx].Coordinate.Row = Row;
                    Tiles[CurrIdx].Coordinate.Col = Col;
                    Tiles[CurrIdx].bAwaitingConsumption = false;

                    FallResult.Moves.Add(FMatchThreeFallMove{
                        .From = FMatchThreeCoordinate{.Face = -1, .Row = -1, .Col = -1},
                        .To = FMatchThreeCoordinate{.Face = FaceIndex, .Row = Row, .Col = Col},
                        .TileType = NewType,
                        .bIsNewSpawn = true
                    });
                }
            }
        }
    }

    return FallResult;
}

