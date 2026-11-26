#include "MatchThreeSubsystem.h"
#include "Engine/AssetManager.h"

UMatchThreeSubsystem* UMatchThreeSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }
    
    const UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return nullptr;
    }
    
    const UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }
    
    return GameInstance->GetSubsystem<UMatchThreeSubsystem>();
}

UMatchThreeTileDefinition* UMatchThreeSubsystem::GetTileDefinition(const EMatchThreeTileType TileType) const
{
    EnsureDefinitionsLoaded();
    
    if (UMatchThreeTileDefinition* FoundDef = LoadedTileDefinitions.FindRef(TileType))
    {
        return FoundDef;
    }
    
    ensureMsgf(false, TEXT("[MatchThreeSubsystem] Failed to find tile definition for type: %s"),
        *StaticEnum<EMatchThreeTileType>()->GetNameStringByValue(static_cast<int64>(TileType)));
    
    return nullptr;
}

void UMatchThreeSubsystem::InitializeCubeData()
{
    CubeData.Initialize();
}

const TMap<EMatchThreeTileType, UMatchThreeTileDefinition*>& UMatchThreeSubsystem::GetAllTileDefinitions() const
{
    EnsureDefinitionsLoaded();
    return LoadedTileDefinitions;
}

void UMatchThreeSubsystem::EnsureDefinitionsLoaded() const
{
    if (bHasLoadedDefinitions)
    {
        return;
    }
    
    LoadedTileDefinitions.Empty();
    
    // TODO DJC: Eventually, need to move away from the hard coded path. Better to rely on the AssetRegistry voodoo.
    const FName Path = TEXT("/Game/DataAssets/MatchThree");
    TArray<FAssetData> AssetList;
    
    const auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    AssetRegistryModule.Get().GetAssetsByPath(Path, AssetList, true);
    
    for (const FAssetData& AssetData : AssetList)
    {
        if (AssetData.AssetClassPath != UMatchThreeTileDefinition::StaticClass()->GetClassPathName())
        {
            continue;
        }
        
        auto* TileDef = Cast<UMatchThreeTileDefinition>(AssetData.GetAsset());
        if (!TileDef)
        {
            continue;
        }
        
        TileDef->CachedTileMesh = TileDef->TileMesh.LoadSynchronous();
        ensureMsgf(TileDef->CachedTileMesh, TEXT("Failed to load mesh for tile definition: %s at path: %s"), 
            *TileDef->GetName(), *TileDef->TileMesh.ToString());
        
        TileDef->CachedTileMaterial = TileDef->TileMaterial.LoadSynchronous();
        ensureMsgf(TileDef->CachedTileMaterial, TEXT("Failed to load material for tile definition: %s at path: %s"), 
            *TileDef->GetName(), *TileDef->TileMaterial.ToString());
        
        LoadedTileDefinitions.Add(TileDef->TileType, TileDef);
    }
    
    bHasLoadedDefinitions = true;
}

struct UMatchThreeSubsystem::FMatchTurnScope
{
    UMatchThreeSubsystem& Subsystem;
    TArray<FMatchThreeBoardEvent> BoardEvents;
    TArray<FMatchThreeFXResponse> FXResponses;

    explicit FMatchTurnScope(UMatchThreeSubsystem& InSubsystem)
        : Subsystem(InSubsystem) {}

    auto Collect()
    {
        return [this](const FMatchThreeBoardEvent& Event) { BoardEvents.Add(Event); };
    }

    ~FMatchTurnScope()
    {
        Subsystem.PaceAndBroadcastFXResponses(FXResponses);
    }
};

void UMatchThreeSubsystem::PaceAndBroadcastFXResponses(TArray<FMatchThreeFXResponse>& VisualActions) const
{
    if (VisualActions.IsEmpty())
    {
        return;
    }

    constexpr float DefaultStepDuration = 0.25f;
    constexpr float MaxTurnDuration = 5.0f;

    const float UnscaledDuration = VisualActions.Num() * DefaultStepDuration;
    
    // TODO DJC: This is very simple time scaling, but eventually it should be an accelerating curve.
    // Game isn't complex enough to warrant that at this phase though.
    const float StepDuration = (UnscaledDuration > MaxTurnDuration)
        ? (MaxTurnDuration / VisualActions.Num())
        : DefaultStepDuration;

    for (FMatchThreeFXResponse& Action : VisualActions)
    {
        Action.Duration = StepDuration;
        OnVisualActionDispatchedEvent.Broadcast(Action);
    }
}

bool UMatchThreeSubsystem::TrySwapTiles(const FMatchThreeCoordinate& CoordA, const FMatchThreeCoordinate& CoordB)
{
    FMatchTurnScope Turn(*this);

    CubeData.TrySwapTiles(CoordA, CoordB, Turn.Collect());

    if (Turn.BoardEvents.IsEmpty())
    {
        return false;
    }

    Turn.FXResponses.Reserve(Turn.BoardEvents.Num());

    bool bValidSwap = false;
    int32 CascadeStep = 0;

    for (const auto& [ConsumptionResult, 
        FallResult,
        EventCoordA,
        EventCoordB,
        EventType] : Turn.BoardEvents)
    {
        FMatchThreeFXResponse Action;
        Action.ActionType = EventType;

        switch (EventType)
        {
        case EMatchThreeBoardEventType::Swap:
            {
                bValidSwap = true;
                Action.CoordA = EventCoordA;
                Action.CoordB = EventCoordB;
                break;
            }

        case EMatchThreeBoardEventType::SwapReject:
            {
                Action.CoordA = EventCoordA;
                Action.CoordB = EventCoordB;
                break;
            }

        case EMatchThreeBoardEventType::Consume:
            {
                CascadeStep++; // TODO DJC: This is just an example of how a rule might scale, update later with design
                const int32 Multiplier = (CascadeStep >= 2) ? CascadeStep : 1;

                // TODO DJC: Award resources to the player using Event.ConsumptionResult.ConsumedCounts
                // The below is just be a way to get the visuals on screen
                Action.CoordinatesToHide = ConsumptionResult.ConsumedCoordinates;
                Action.CascadeStep = CascadeStep;
                Action.PointsAwarded = ConsumptionResult.TotalTilesCleared * 100 * Multiplier;
                break;
            }

        case EMatchThreeBoardEventType::FallAndRefill:
            {
                Action.FallResult = FallResult;
                break;
            }

        default:
            continue;
        }

        Turn.FXResponses.Add(MoveTemp(Action));
    }

    return bValidSwap;
}

int32 UMatchThreeSubsystem::GetActiveFaceNumber() const
{
    return CubeData.GetActiveFaceNumber();
}

int32 UMatchThreeSubsystem::GetActiveFaceSize() const
{
    return CubeData.GetFaceSize(CubeData.GetActiveFaceNumber());
}

EMatchThreeTileType UMatchThreeSubsystem::GetTileTypeAt(const FMatchThreeCoordinate& Coord) const
{
    return CubeData.GetTileTypeAt(Coord);
}
