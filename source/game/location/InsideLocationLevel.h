// dane poziomu lokacji
#pragma once

#include "Mapa2.h"
#include "Unit.h"
#include "Chest.h"
#include "Trap.h"
#include "Door.h"
#include "GroundItem.h"
#include "Useable.h"
#include "Object.h"
#include "GameCommon.h"

//-----------------------------------------------------------------------------
struct InsideLocationLevel
{
	Pole* map;
	int w, h;
	vector<Unit*> units;
	vector<Chest*> chests;
	vector<Object> objects;
	vector<Light> lights;
	vector<Room> rooms;
	vector<Trap*> traps;
	vector<Door*> doors;
	vector<GroundItem*> items;
	vector<Useable*> useables;
	vector<Blood> bloods;
	INT2 staircase_up, staircase_down;
	int staircase_up_dir, staircase_down_dir;
	bool staircase_down_in_wall;

	InsideLocationLevel() : map(NULL)
	{

	}
	~InsideLocationLevel();

	inline bool IsInside(int _x, int _y) const
	{
		return _x >= 0 && _y >= 0 && _x < w && _y < h;
	}
	inline bool IsInside(const INT2& _pt) const
	{
		return IsInside(_pt.x, _pt.y);
	}
	inline VEC3 GetRandomPos() const
	{
		return VEC3(random(2.f*w),0,random(2.f*h));
	}
	Room* GetNearestRoom(const VEC3& pos);
	Room* FindEscapeRoom(const VEC3& my_pos, const VEC3& enemy_pos);
	inline int GetRoomId(Room* room) const
	{
		assert(room);
		return (int)(room - &rooms[0]);
	}
	inline INT2 GetUpStairsFrontTile() const
	{
		return staircase_up + dir_to_pos(staircase_up_dir);
	}
	inline INT2 GetDownStairsFrontTile() const
	{
		return staircase_down + dir_to_pos(staircase_down_dir);
	}
	inline Room* GetRandomRoom()
	{
		return &rooms[rand2() % rooms.size()];
	}
	bool GetRandomNearWallTile(const Room& pokoj, INT2& tile, int& rot, bool nocol=false);
	Room& GetFarRoom(bool have_down_stairs);
	Room* GetRoom(const INT2& pt);
	inline Room* GetUpStairsRoom()
	{
		return GetRoom(staircase_up);
	}
	inline Room* GetDownStairsRoom()
	{
		return GetRoom(staircase_down);
	}

	void SaveLevel(HANDLE file, bool local);
	void LoadLevel(HANDLE file, bool local);

	void BuildRefidTable();

	inline Pole& At(const INT2& pt)
	{
		assert(IsInside(pt));
		return map[pt(w)];
	}
	int FindRoomId(int target);

	inline Door* FindDoor(const INT2& pt) const
	{
		for(vector<Door*>::const_iterator it = doors.begin(), end = doors.end(); it != end; ++it)
		{
			if((*it)->pt == pt)
				return *it;
		}
		return NULL;
	}

	inline bool IsTileVisible(const VEC3& pos) const
	{
		INT2 pt = pos_to_pt(pos);
		return IS_SET(map[pt(w)].flags, Pole::F_ODKRYTE);
	}

	// sprawdza czy pole le�y przy �cianie, nie uwzgl�dnia na ukos, nie mo�e by� na kraw�dzi mapy!
	bool IsTileNearWall(const INT2& pt) const;
	bool IsTileNearWall(const INT2& pt, int& dir) const;

	bool RemoveUnit(Unit* unit);
	bool FindUnit(Unit* unit);
};
