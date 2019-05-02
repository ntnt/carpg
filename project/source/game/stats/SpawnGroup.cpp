#include "Pch.h"
#include "GameCore.h"
#include "SpawnGroup.h"
#include "UnitGroup.h"

//-----------------------------------------------------------------------------
SpawnGroup g_spawn_groups[] = {
	SpawnGroup("losowo", "", "", K_E),
	SpawnGroup("gobliny", "goblins", nullptr, K_E),
	SpawnGroup("orkowie", "orcs", nullptr, K_I),
	SpawnGroup("bandyci", "bandits", nullptr, K_I),
	SpawnGroup("nieumarli", "undead", nullptr, K_I),
	SpawnGroup("nekro", "necros", nullptr, K_I),
	SpawnGroup("magowie", "mages", nullptr, K_I),
	SpawnGroup("golemy", "golems", nullptr, K_E),
	SpawnGroup("magowie&golemy", "mages_and_golems", nullptr, K_I),
	SpawnGroup("z³o", "evils", nullptr, K_I),
	SpawnGroup("brak", "", "", K_E, -1),
	SpawnGroup("unk", "unk", nullptr, K_I),
	SpawnGroup("wyzwanie", "", "", K_E)
};

//=================================================================================================
UnitData* SpawnGroup::GetSpawnLeader(int level) const
{
	assert(unit_group->HaveLeader());
	return unit_group->GetLeader(level);
}
