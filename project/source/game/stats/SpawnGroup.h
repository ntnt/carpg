#pragma once

//-----------------------------------------------------------------------------
enum SPAWN_GROUP
{
	SG_RANDOM,
	SG_GOBLINS,
	SG_ORCS,
	SG_BANDITS,
	SG_UNDEAD,
	SG_NECROMANCERS,
	SG_MAGES,
	SG_GOLEMS,
	SG_MAGES_AND_GOLEMS,
	SG_EVIL,
	SG_NONE,
	SG_UNKNOWN,
	SG_CHALLANGE,
	SG_MAX
};

//-----------------------------------------------------------------------------
enum KONCOWKA
{
	K_E,
	K_I
};

//-----------------------------------------------------------------------------
struct UnitData;
struct UnitGroup;

//-----------------------------------------------------------------------------
struct SpawnGroup
{
	cstring id, unit_group_id, name;
	UnitGroup* unit_group;
	KONCOWKA k;
	int food_mod;

	SpawnGroup(cstring id, cstring unit_group_id, cstring name, KONCOWKA k, int food_mod) : id(id), unit_group_id(unit_group_id),
		name(name), k(k), unit_group(nullptr), food_mod(food_mod)
	{
	}

	UnitData* GetSpawnLeader(int level) const;
};

//-----------------------------------------------------------------------------
extern SpawnGroup g_spawn_groups[];
