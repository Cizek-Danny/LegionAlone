#include "CubeActor.h"

#include "MatchThreeSubsystem.h"

AMatchThreeCubeActor::AMatchThreeCubeActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMatchThreeCubeActor::InitializeCube()
{
    auto* Subsystem = UMatchThreeSubsystem::Get(this);
    if (!Subsystem)
    {
        return;
    }
    
	Subsystem->InitializeCubeData();
	InitializeCubeVisuals();
}

void AMatchThreeCubeActor::BeginPlay()
{
    Super::BeginPlay();

    if (UMatchThreeSubsystem* MatchSubsystem = UMatchThreeSubsystem::Get(GetWorld()))
    {
        MatchSubsystem->OnVisualActionDispatchedEvent.AddUObject(this, &AMatchThreeCubeActor::OnVisualActionDispatched);
    }

    InitializeCube();
}

void AMatchThreeCubeActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UMatchThreeSubsystem* MatchSubsystem = UMatchThreeSubsystem::Get(GetWorld()))
    {
        MatchSubsystem->OnVisualActionDispatchedEvent.RemoveAll(this);
    }
    
    GetWorldTimerManager().ClearTimer(VisualActionTimerHandle);

    Super::EndPlay(EndPlayReason);
}

// Logical Arrangement
// [0,0] [0,1] [0,2] [0,3] [0,4] [0,5] [0,6] [0,7]
// [1,0] [1,1] [1,2] [1,3] [1,4] [1,5] [1,6] [1,7]
// [2,0] [2,1] [2,2] [2,3] [2,4] [2,5] [2,6] [2,7]
// [3,0] [3,1] [3,2] [3,3] [3,4] [3,5] [3,6] [3,7]
// [4,0] [4,1] [4,2] [4,3] [4,4] [4,5] [4,6] [4,7]
// [5,0] [5,1] [5,2] [5,3] [5,4] [5,5] [5,6] [5,7]
// [6,0] [6,1] [6,2] [6,3] [6,4] [6,5] [6,6] [6,7]
// [7,0] [7,1] [7,2] [7,3] [7,4] [7,5] [7,6] [7,7]

// Visual Centers
// [ 7.7,  7.7]                 [ 5.5,  7.7]            [ 3.3,  7.7]            [ 1.1,  7.7]            [-1.1,  7.7]            [-3.3,  7.7]            [-5.5,  7.7]            [-7.7,  7.7]
// [ 7.7,  5.5]                 [ 5.5,  5.5]            [ 3.3,  5.5]            [ 1.1,  5.5]            [-1.1,  5.5]            [-3.3,  5.5]            [-5.5,  5.5]            [-7.7,  5.5]
// [ 7.7,  3.3]                 [ 5.5,  3.3]            [ 3.3,  3.3]            [ 1.1,  3.3]            [-1.1,  3.3]            [-3.3,  3.3]            [-5.5,  3.3]            [-7.7,  3.3]
// [ 7.7,  1.1]                 [ 5.5,  1.1]            [ 3.3,  1.1]            [ 1.1,  1.1]            [-1.1,  1.1]            [-3.3,  1.1]            [-5.5,  1.1]            [-7.7,  1.1]
// [ 7.7, -1.1]                 [ 5.5, -1.1]            [ 3.3, -1.1]            [ 1.1, -1.1]            [-1.1, -1.1]            [-3.3, -1.1]            [-5.5, -1.1]            [-7.7, -1.1]
// [ 7.7, -3.3]                 [ 5.5, -3.3]            [ 3.3, -3.3]            [ 1.1, -3.3]            [-1.1, -3.3]            [-3.3, -3.3]            [-5.5, -3.3]            [-7.7, -3.3]
// [ 7.7, -5.5]                 [ 5.5, -5.5]            [ 3.3, -5.5]            [ 1.1, -5.5]            [-1.1, -5.5]            [-3.3, -5.5]            [-5.5, -5.5]            [-7.7, -5.5]
// [ 7.7, -7.7]                 [ 5.5, -7.7]            [ 3.3, -7.7]            [ 1.1, -7.7]            [-1.1, -7.7]            [-3.3, -7.7]            [-5.5, -7.7]            [-7.7, -7.7]

// Hit Boundaries
// [ 8.8, 8.8          ]   [ 6.6, 8.8          ]   [ 4.4, 8.8          ]   [ 2.2, 8.8          ]   [ 0.0, 8.8          ]   [-2.2, 8.8          ]   [-4.4, 8.8          ]   [-6.6, 8.8          ]
// [   -  7.7, 7.7 -   ]   [   -  5.5, 7.7 -   ]   [   -  3.3, 7.7 -   ]   [   -  1.1, 7.7 -   ]   [   - -1.1, 7.7 -   ]   [   - -3.3, 7.7 -   ]   [   - -5.5, 7.7 -   ]   [   - -7.7, 7.7 -   ]
// [           6.6, 6.6]   [           4.4, 6.6]   [           2.2, 6.6]   [           0.0, 6.6]   [          -2.2, 6.6]   [          -4.4, 6.6]   [          -6.6, 6.6]   [          -8.8, 6.6]

// [ 8.8, 6.6          ]   [ 6.6, 6.6          ]   [ 4.4, 6.6          ]   [ 2.2, 6.6          ]   [ 0.0, 6.6          ]   [-2.2, 6.6          ]   [-4.4, 6.6          ]   [-6.6, 6.6          ]
// [   -  7.7, 5.5 -   ]   [   -  5.5, 5.5 -   ]   [   -  3.3, 5.5 -   ]   [   -  1.1, 5.5 -   ]   [   - -1.1, 5.5 -   ]   [   - -3.3, 5.5 -   ]   [   - -5.5, 5.5 -   ]   [   - -7.7, 5.5 -   ]
// [           6.6, 4.4]   [           4.4, 4.4]   [           2.2, 4.4]   [           0.0, 4.4]   [          -2.2, 4.4]   [          -4.4, 4.4]   [          -6.6, 4.4]   [          -8.8, 4.4]

// [ 8.8, 4.4          ]   [ 6.6, 4.4          ]   [ 4.4, 4.4          ]   [ 2.2, 4.4          ]   [ 0.0, 4.4          ]   [-2.2, 4.4          ]   [-4.4, 4.4          ]   [-6.6, 4.4          ]
// [   -  7.7, 3.3 -   ]   [   -  5.5, 3.3 -   ]   [   -  3.3, 3.3 -   ]   [   -  1.1, 3.3 -   ]   [   - -1.1, 3.3 -   ]   [   - -3.3, 3.3 -   ]   [   - -5.5, 3.3 -   ]   [   - -7.7, 3.3 -   ]
// [           6.6, 2.2]   [           4.4, 2.2]   [           2.2, 2.2]   [           0.0, 2.2]   [          -2.2, 2.2]   [          -4.4, 2.2]   [          -6.6, 2.2]   [          -8.8, 2.2]

// [ 8.8, 2.2          ]   [ 6.6, 2.2          ]   [ 4.4, 2.2          ]   [ 2.2, 2.2          ]   [ 0.0, 2.2          ]   [-2.2, 2.2          ]   [-4.4, 2.2          ]   [-6.6, 2.2          ]
// [   -  7.7, 1.1 -   ]   [   -  5.5, 1.1 -   ]   [   -  3.3, 1.1 -   ]   [   -  1.1, 1.1 -   ]   [   - -1.1, 1.1 -   ]   [   - -3.3, 1.1 -   ]   [   - -5.5, 1.1 -   ]   [   - -7.7, 1.1 -   ]
// [           6.6, 0.0]   [           4.4, 0.0]   [           2.2, 0.0]   [           0.0, 0.0]   [          -2.2, 0.0]   [          -4.4, 0.0]   [          -6.6, 0.0]   [          -8.8, 0.0]

// [ 8.8, 0.0          ]   [ 6.6, 0.0          ]   [ 4.4, 0.0          ]   [ 2.2, 0.0          ]   [ 0.0, 0.0          ]   [-2.2, 0.0          ]   [-4.4, 0.0          ]   [-6.6, 0.0          ]
// [   -  7.7,-1.1 -   ]   [   -  5.5,-1.1 -   ]   [   -  3.3,-1.1 -   ]   [   -  1.1,-1.1 -   ]   [   - -1.1,-1.1 -   ]   [   - -3.3,-1.1 -   ]   [   - -5.5,-1.1 -   ]   [   - -7.7,-1.1 -   ]
// [           6.6,-2.2]   [           4.4,-2.2]   [           2.2,-2.2]   [           0.0,-2.2]   [          -2.2,-2.2]   [          -4.4,-2.2]   [          -6.6,-2.2]   [          -8.8,-2.2]

// [ 8.8,-2.2          ]   [ 6.6,-2.2          ]   [ 4.4,-2.2          ]   [ 2.2,-2.2          ]   [ 0.0,-2.2          ]   [-2.2,-2.2          ]   [-4.4,-2.2          ]   [-6.6,-2.2          ]
// [   -  7.7,-3.3 -   ]   [   -  5.5,-3.3 -   ]   [   -  3.3,-3.3 -   ]   [   -  1.1,-3.3 -   ]   [   - -1.1,-3.3 -   ]   [   - -3.3,-3.3 -   ]   [   - -5.5,-3.3 -   ]   [   - -7.7,-3.3 -   ]
// [           6.6,-4.4]   [           4.4,-4.4]   [           2.2,-4.4]   [           0.0,-4.4]   [          -2.2,-4.4]   [          -4.4,-4.4]   [          -6.6,-4.4]   [          -8.8,-4.4]

// [ 8.8,-4.4          ]   [ 6.6,-4.4          ]   [ 4.4,-4.4          ]   [ 2.2,-4.4          ]   [ 0.0,-4.4          ]   [-2.2,-4.4          ]   [-4.4,-4.4          ]   [-6.6,-4.4          ]
// [   -  7.7,-5.5 -   ]   [   -  5.5,-5.5 -   ]   [   -  3.3,-5.5 -   ]   [   -  1.1,-5.5 -   ]   [   - -1.1,-5.5 -   ]   [   - -3.3,-5.5 -   ]   [   - -5.5,-5.5 -   ]   [   - -7.7,-5.5 -   ]
// [           6.6,-6.6]   [           4.4,-6.6]   [           2.2,-6.6]   [           0.0,-6.6]   [          -2.2,-6.6]   [          -4.4,-6.6]   [          -6.6,-6.6]   [          -8.8,-6.6]

// [ 8.8,-6.6          ]   [ 6.6,-6.6          ]   [ 4.4,-6.6          ]   [ 2.2,-6.6          ]   [ 0.0,-6.6          ]   [-2.2,-6.6          ]   [-4.4,-6.6          ]   [-6.6,-6.6          ]
// [   -  7.7,-7.7 -   ]   [   -  5.5,-7.7 -   ]   [   -  3.3,-7.7 -   ]   [   -  1.1,-7.7 -   ]   [   - -1.1,-7.7 -   ]   [   - -3.3,-7.7 -   ]   [   - -5.5,-7.7 -   ]   [   - -7.7,-7.7 -   ]
// [           6.6,-8.8]   [           4.4,-8.8]   [           2.2,-8.8]   [           0.0,-8.8]   [          -2.2,-8.8]   [          -4.4,-8.8]   [          -6.6,-8.8]   [          -8.8,-8.8]

void AMatchThreeCubeActor::HandleInteractionPlaneClick(FVector const HitLocation, UPrimitiveComponent* const InteractionPlane)
{
    auto* Subsystem = UMatchThreeSubsystem::Get(this);
    if (!Subsystem)
    {
        return;
    }
    
    FVector const LocalHit = InteractionPlane->GetComponentTransform().InverseTransformPosition(HitLocation); // Local
    constexpr float TileSpacing = 2.2f; // TODO DJC: Make this data driven
    int32 const ActiveFaceNumber = Subsystem->GetActiveFaceNumber();
    int32 const GridSize = Subsystem->GetActiveFaceSize();
    float const AdjustedZ = 8.8f - LocalHit.Z;
    float const AdjustedY = 8.8f - LocalHit.Y;
    int32 const Row = FMath::FloorToInt(AdjustedZ / TileSpacing);
    int32 const Col = FMath::FloorToInt(AdjustedY / TileSpacing);

    if (Row >= 0 && Row < GridSize && Col >= 0 && Col < GridSize)
    {
        const auto& [FaceSize, Tiles] = VisualIndexArrays[ActiveFaceNumber];
        const int32 LocalIndex = (Row * GridSize) + Col;

        if (!bHasSelection)
        {
            SelectedRow = Row;
            SelectedCol = Col;
            bHasSelection = true;

            const auto& [TileType, InstanceIndex] = Tiles[LocalIndex];
            if (UHierarchicalInstancedStaticMeshComponent* HISMComp = TileHISMMap.FindRef(TileType))
            {
                HISMComp->SetCustomDataValue(InstanceIndex, 0, 1.0f, true);
            }
        }
        else
        {
            const int32 OldLocalIndex = (SelectedRow * GridSize) + SelectedCol;
            const auto& [TileType, InstanceIndex] = Tiles[OldLocalIndex];
        
            if (UHierarchicalInstancedStaticMeshComponent* OldHISMComp = TileHISMMap.FindRef(TileType))
            {
                OldHISMComp->SetCustomDataValue(InstanceIndex, 0, 0.0f, true);
            }

            // TODO DJC: Eventually make use of the return to signal to the player if they tried an invalid swap
            Subsystem->TrySwapTiles(
                FMatchThreeCoordinate{.Face = ActiveFaceNumber, .Row = SelectedRow, .Col = SelectedCol},
                FMatchThreeCoordinate{.Face = ActiveFaceNumber, .Row = Row, .Col = Col});

            bHasSelection = false;
            SelectedRow = -1;
            SelectedCol = -1;
        }
    }
    else
    {
        // TODO DJC: Handle clicks outside the bounds. Maybe swap cube faces, maybe leave view, etc.
    }
}

FTransform AMatchThreeCubeActor::CalculateTileTransform(const FMatchThreeCoordinate& Coord, int32 const FaceSize)
{
    constexpr double TileSpacing = 2.2f; // Tiles are radius 1cm, 2cm Diameter, so give 10% margins of space
    const double TotalWidth = TileSpacing * FaceSize;
    const double HalfWidth = TotalWidth * 0.5f;
    const double RowPlacement = HalfWidth - Coord.Row * TileSpacing - TileSpacing / 2;
    const double ColPlacement = HalfWidth - Coord.Col * TileSpacing - TileSpacing / 2;
    FVector LocalPos;
    FRotator LocalRot(0.0f, 0.0f, 0.0f);
    
    switch (Coord.Face)
    {
    case 0: // Front (+X)
        LocalPos = FVector(HalfWidth + TileSpacing / 2, ColPlacement, RowPlacement);
        break;
        
    case 1: // Back (-X)
        LocalPos = FVector(-HalfWidth - TileSpacing / 2, ColPlacement, RowPlacement);
        LocalRot = FRotator(0.0f, 180.0f, 0.0f);
        break;
        
    case 2: // Right (+Y)
        LocalPos = FVector(ColPlacement, HalfWidth + TileSpacing / 2, RowPlacement);
        LocalRot = FRotator(0.0f, 90.0f, 0.0f);
        break;
        
    case 3: // Left (-Y)
        LocalPos = FVector(ColPlacement, -HalfWidth - TileSpacing / 2, RowPlacement);
        LocalRot = FRotator(0.0f, -90.0f, 0.0f);
        break;
        
    case 4: // Top (+Z)
        LocalPos = FVector(RowPlacement, ColPlacement, HalfWidth + TileSpacing / 2);
        LocalRot = FRotator(90.0f, 0.0f, 0.0f);
        break;
        
    case 5: // Bottom (-Z)
        LocalPos = FVector(RowPlacement, ColPlacement, -HalfWidth - TileSpacing / 2);
        LocalRot = FRotator(-90.0f, 0.0f, 0.0f);
        break;
        
    default:
        break;
    }
        
    return FTransform(LocalRot, LocalPos, FVector(1.0f));
}

void AMatchThreeCubeActor::InitializeCubeVisuals()
{
    const auto* Subsystem = UMatchThreeSubsystem::Get(this);
    if (!Subsystem)
    {
        return;
    }
    
    if (!GetRootComponent())
    {
        auto* SceneRoot = NewObject<USceneComponent>(this, TEXT("SceneRoot"));
        SetRootComponent(SceneRoot);
        SceneRoot->RegisterComponent();
    }
    
    TileHISMMap.Empty();
    HiddenInstancePools.Empty();
    
    // Using Hierarchical Instanced Static Meshes to keep things lean in terms of perf. Code complexity tradeoff.
    for (const auto& Pair : Subsystem->GetAllTileDefinitions())
    {
        const EMatchThreeTileType TileType = Pair.Key;
        const UMatchThreeTileDefinition* TileDef = Pair.Value;

        if (!TileDef || !TileDef->CachedTileMesh)
        {
            continue;
        }

        if (auto* HISMComp = NewObject<UHierarchicalInstancedStaticMeshComponent>(this))
        {
            HISMComp->NumCustomDataFloats = 1;
            HISMComp->RegisterComponent();
            HISMComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            HISMComp->SetStaticMesh(TileDef->CachedTileMesh);
            HISMComp->SetMaterial(0, TileDef->CachedTileMaterial);
            HISMComp->MarkRenderStateDirty();
            
            TileHISMMap.Add(TileType, HISMComp);
        }
    }

    for (int32 FaceIdx = 0; FaceIdx < VisualIndexArrays.Num(); ++FaceIdx)
    {
        auto& [FaceSize, Tiles] = VisualIndexArrays[FaceIdx];
        FaceSize = Subsystem->GetActiveFaceSize(); // TODO DJC: This is broken logic. Accepting for current velocity
        Tiles.SetNum(FaceSize * FaceSize);

        for (int32 Row = 0; Row < FaceSize; ++Row)
        {
            for (int32 Col = 0; Col < FaceSize; ++Col)
            {
                const EMatchThreeTileType TileType =
                            Subsystem->GetTileTypeAt(FMatchThreeCoordinate{.Face = FaceIdx, .Row = Row, .Col = Col});
                if (UHierarchicalInstancedStaticMeshComponent* FoundHISM = TileHISMMap.FindRef(TileType))
                {
                    const FTransform TileTransform =
                        CalculateTileTransform(FMatchThreeCoordinate{.Face = FaceIdx, .Row = Row, .Col = Col}, FaceSize);
                    const int32 InstanceIndex = FoundHISM->AddInstance(TileTransform);
                
                    Tiles[Row * FaceSize + Col] = { .TileType = TileType, .InstanceIndex = InstanceIndex };
                }
            }
        }
    }
}

void AMatchThreeCubeActor::OnVisualActionDispatched(const FMatchThreeFXResponse& Action)
{
    VisualActionQueue.Add(Action);

    if (!bIsPlayingVisualAction)
    {
        PlayNextVisualAction();
    }
}

void AMatchThreeCubeActor::PlayNextVisualAction()
{
    if (!VisualActionQueue.IsValidIndex(VisualQueueHeadIndex))
    {
        VisualActionQueue.Reset();
        VisualQueueHeadIndex = 0;
        bIsPlayingVisualAction = false;
        return;
    }

    bIsPlayingVisualAction = true;
    const FMatchThreeFXResponse Action = VisualActionQueue[VisualQueueHeadIndex++];

    switch (Action.ActionType)
    {
    case EMatchThreeBoardEventType::Swap:
        ExecuteSwapVisual(Action.CoordA, Action.CoordB);
        break;

    case EMatchThreeBoardEventType::SwapReject:
        // TODO DJC: Play rejected swap bounce/shake between Action.CoordA and Action.CoordB
        break;

    case EMatchThreeBoardEventType::Consume:
        ExecuteConsumeVisual(Action.CoordinatesToHide);
        break;

    case EMatchThreeBoardEventType::FallAndRefill:
        ExecuteFallAndRefillVisual(Action.FallResult);
        break;

    default:
        break;
    }

    GetWorldTimerManager().SetTimer(
        VisualActionTimerHandle,
        this,
        &AMatchThreeCubeActor::PlayNextVisualAction,
        FMath::Max(Action.Duration, KINDA_SMALL_NUMBER),
        false
    );
}

void AMatchThreeCubeActor::ExecuteSwapVisual(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB)
{
    auto& [FaceSizeA, TilesA] = VisualIndexArrays[CoordA.Face];
    auto& [FaceSizeB, TilesB] = VisualIndexArrays[CoordB.Face];

    const int32 IndexA = (CoordA.Row * FaceSizeA) + CoordA.Col;
    const int32 IndexB = (CoordB.Row * FaceSizeB) + CoordB.Col;

    FVisualIndex& EntryA = TilesA[IndexA];
    FVisualIndex& EntryB = TilesB[IndexB];

    UHierarchicalInstancedStaticMeshComponent* HISM_A = TileHISMMap.FindRef(EntryA.TileType);
    UHierarchicalInstancedStaticMeshComponent* HISM_B = TileHISMMap.FindRef(EntryB.TileType);

    if (HISM_A && HISM_B)
    {
        FTransform TransformA, TransformB;
        HISM_A->GetInstanceTransform(EntryA.InstanceIndex, TransformA, /*bWorldSpace=*/false);
        HISM_B->GetInstanceTransform(EntryB.InstanceIndex, TransformB, /*bWorldSpace=*/false);

        HISM_A->UpdateInstanceTransform(EntryA.InstanceIndex, TransformB, /*bWorldSpace=*/false, /*bMarkRenderStateDirty=*/true, /*bTeleport=*/false);
        HISM_B->UpdateInstanceTransform(EntryB.InstanceIndex, TransformA, /*bWorldSpace=*/false, /*bMarkRenderStateDirty=*/true, /*bTeleport=*/false);
    }

    Swap(EntryA, EntryB);
}

void AMatchThreeCubeActor::ExecuteConsumeVisual(const TArray<FMatchThreeCoordinate>& ConsumedCoordinates)
{
    for (const auto& [Face, Row, Col] : ConsumedCoordinates)
    {
        auto& [FaceSize, Tiles] = VisualIndexArrays[Face];
        const int32 Index = (Row * FaceSize) + Col;
        const auto& [TileType, InstanceIndex] = Tiles[Index];

        if (UHierarchicalInstancedStaticMeshComponent* HISM = TileHISMMap.FindRef(TileType))
        {
            FTransform InstanceTransform;
            if (HISM->GetInstanceTransform(InstanceIndex, InstanceTransform, /*bWorldSpace=*/false))
            {
                InstanceTransform.SetScale3D(FVector::ZeroVector);
                HISM->UpdateInstanceTransform(InstanceIndex, InstanceTransform, /*bWorldSpace=*/false, /*bMarkRenderStateDirty=*/true, /*bTeleport=*/false);
                HiddenInstancePools.FindOrAdd(TileType).Add(InstanceIndex);
            }
        }
    }
}

void AMatchThreeCubeActor::ExecuteFallAndRefillVisual(const FMatchThreeFallResult& FallResult)
{
    for (const auto& [From,
                        To,
                        TileType,
                        bIsNewSpawn]
        : FallResult.Moves)
    {
        if (!bIsNewSpawn)
        {
            // Existing tile falling from Move.From -> Move.To
            auto& [FromFaceSize, FromTiles] = VisualIndexArrays[From.Face];
            auto& [ToFaceSize, ToTiles] = VisualIndexArrays[To.Face];

            const int32 FromIndex = (From.Row * FromFaceSize) + From.Col;
            const int32 ToIndex = (To.Row * ToFaceSize) + To.Col;

            FVisualIndex& SourceEntry = FromTiles[FromIndex];
            FVisualIndex& DestEntry = ToTiles[ToIndex];

            if (UHierarchicalInstancedStaticMeshComponent* HISM = TileHISMMap.FindRef(SourceEntry.TileType))
            {
                const FTransform DestTransform = CalculateTileTransform(To, ToFaceSize);
                HISM->UpdateInstanceTransform(
                    SourceEntry.InstanceIndex,
                    DestTransform,
                    false,
                    true,
                    false
                );
            }

            Swap(SourceEntry, DestEntry);
        }
        else
        {
            // Newly spawned tile entering at Move.To
            auto& [FaceSize, Tiles] = VisualIndexArrays[To.Face];
            const int32 ToIndex = (To.Row * FaceSize) + To.Col;

            UHierarchicalInstancedStaticMeshComponent* TargetHISM = TileHISMMap.FindRef(TileType);
            if (!TargetHISM)
            {
                continue;
            }

            const FTransform SpawnTransform = CalculateTileTransform(To, FaceSize);
            int32 InstanceIndex = INDEX_NONE;

            if (TArray<int32>* Pool = HiddenInstancePools.Find(TileType); Pool && !Pool->IsEmpty())
            {
                InstanceIndex = Pool->Pop(EAllowShrinking::No);
                TargetHISM->UpdateInstanceTransform(
                    InstanceIndex,
                    SpawnTransform,
                    false,
                    true,
                    true
                );
            }
            else
            {
                InstanceIndex = TargetHISM->AddInstance(SpawnTransform);
            }

            Tiles[ToIndex] = { .TileType = TileType, .InstanceIndex = InstanceIndex };
        }
    }
}


