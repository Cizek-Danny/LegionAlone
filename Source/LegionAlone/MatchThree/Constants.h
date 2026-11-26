#pragma once

#include "CoreMinimal.h"
#include "Constants.generated.h"

UENUM(BlueprintType)
enum class EMatchThreeFaceSize : uint8
{
	Invalid = 0 UMETA(DisplayName = "Invalid"),
	Small = 6  UMETA(DisplayName = "Small (6x6)"),
	Medium = 8 UMETA(DisplayName = "Medium (8x8)"),
	Large = 10  UMETA(DisplayName = "Large (10x10)")
};


UENUM(BlueprintType)
enum class EMatchThreeFaceType : uint8
{
	Invalid = 0 UMETA(DisplayName = "Invalid"),
	Mana = 1 UMETA(DisplayName = "Mana (1)"),
	Crafting = 2 UMETA(DisplayName = "Crafting (2)"),
	Support = 3 UMETA(DisplayName = "Support (3)"),
	Bartering = 4 UMETA(DisplayName = "Bartering (4)"),
	Inventory = 5 UMETA(DisplayName = "Inventory (5)"),
	Thievery = 6 UMETA(DisplayName = "Thievery (6)")
};


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EMatchThreeTileType : uint8
{
	Invalid = 0 UMETA(DisplayName = "Invalid"),
	Blue 	= 1 << 0 UMETA(DisplayName = "Blue (1)"),
	Green 	= 1 << 1 UMETA(DisplayName = "Green (2)"),
	Red 	= 1 << 2 UMETA(DisplayName = "Red (3)"),
	Yellow 	= 1 << 3 UMETA(DisplayName = "Yellow (4)"),
	Cyan 	= 1 << 4 UMETA(DisplayName = "Cyan (5)"),
	Purple 	= 1 << 5 UMETA(DisplayName = "Purple (6)"),
	Fuchsia = 1 << 6 UMETA(DisplayName = "Fuchsia (7)"),
	Magenta = 1 << 7 UMETA(DisplayName = "Magenta (8)")
};
ENUM_CLASS_FLAGS(EMatchThreeTileType);

