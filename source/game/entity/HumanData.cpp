// dane cz�owieka
#include "Pch.h"
#include "Base.h"
#include "HumanData.h"
#include "Animesh.h"
#include "SaveState.h"
#include "BitStreamFunc.h"

//-----------------------------------------------------------------------------
bool g_beard_and_mustache[MAX_BEARD-1] = {
	false,
	false,
	false,
	false,
	true
};

//-----------------------------------------------------------------------------
#define HEX(h) VEC4(1.f/256*(((h)&0xFF0000)>>16), 1.f/256*(((h)&0xFF00)>>8), 1.f/256*((h)&0xFF), 1.f)
// ze strony http://www.dandwiki.com/wiki/Random_Hair_and_Eye_Color_(DnD_Other)
const VEC4 g_hair_colors[] = {
	HEX(0x000000), // Black
	HEX(0x808080), // Gray
	HEX(0xD3D3D3), // Platinum
	HEX(0xFFFFFF), // White
	HEX(0xB8860B), // Dark Blonde
	HEX(0xDAA520), // Blonde
	HEX(0xF0E68C), // Bleach blonde
	HEX(0x800000), // Dark Redhead
	HEX(0xFF8C00), // Redhead
	HEX(0xF4A460), // Light Redhead
	HEX(0x8B4513), // Brunette
	HEX(0xA0522D)  // Auburn
};
const uint n_hair_colors = countof(g_hair_colors);
// siwy 0xDED5D0

//=================================================================================================
// Ustawienie macierzy na podstawie wysoko�ci i wagi
//=================================================================================================
void Human::ApplyScale(Animesh* ani)
{
	assert(ani);

	mat_scale.resize(ani->head.n_bones);

	float h = (height-1)*0.2f+1.f;
	float w;
	if(height > 1.f)
		w = 1.f+(height-1)*0.4f;
	else if(height < 1.f)
		w = 1.f-(1.f-height)*0.3f;
	else
		w = 1.f;

	MATRIX m;
	D3DXMatrixScaling(&m, w, h, w);

	for(int i=0; i<ani->head.n_bones; ++i)
		mat_scale[i] = m;

	w = (w+1)/2;
	D3DXMatrixScaling(&m, w, h, w);
	mat_scale[4] = m;
	mat_scale[5] = m;
}

//=================================================================================================
void Human::Save(HANDLE file)
{
	WriteFile(file, &hair, sizeof(hair), &tmp, NULL);
	WriteFile(file, &beard, sizeof(beard), &tmp, NULL);
	WriteFile(file, &mustache, sizeof(mustache), &tmp, NULL);
	WriteFile(file, &hair_color, sizeof(hair_color), &tmp, NULL);
	WriteFile(file, &height, sizeof(height), &tmp, NULL);
}

//=================================================================================================
void Human::Load(HANDLE file)
{
	ReadFile(file, &hair, sizeof(hair), &tmp, NULL);
	ReadFile(file, &beard, sizeof(beard), &tmp, NULL);
	ReadFile(file, &mustache, sizeof(mustache), &tmp, NULL);
	ReadFile(file, &hair_color, sizeof(hair_color), &tmp, NULL);
	if(LOAD_VERSION < V_0_2_10)
		ReadFile(file, &height, sizeof(height), &tmp, NULL); // old weight
	ReadFile(file, &height, sizeof(height), &tmp, NULL);
}

//=================================================================================================
void HumanData::Get(const Human& h)
{
	hair = h.hair;
	beard = h.beard;
	mustache = h.mustache;
	hair_color = h.hair_color;
	height = h.height;
}

//=================================================================================================
void HumanData::Set(Human& h) const
{
	h.hair = hair;
	h.beard = beard;
	h.mustache = mustache;
	h.hair_color = hair_color;
	h.height = height;
}

//=================================================================================================
void HumanData::CopyFrom(HumanData& hd)
{
	hair = hd.hair;
	beard = hd.beard;
	mustache = hd.mustache;
	hair_color = hd.hair_color;
	height = hd.height;
}

//=================================================================================================
void HumanData::Save(HANDLE file)
{
	WriteFile(file, &hair, sizeof(hair), &tmp, NULL);
	WriteFile(file, &beard, sizeof(beard), &tmp, NULL);
	WriteFile(file, &mustache, sizeof(mustache), &tmp, NULL);
	WriteFile(file, &hair_color, sizeof(hair_color), &tmp, NULL);
	WriteFile(file, &height, sizeof(height), &tmp, NULL);
}

//=================================================================================================
void HumanData::Load(HANDLE file)
{
	ReadFile(file, &hair, sizeof(hair), &tmp, NULL);
	ReadFile(file, &beard, sizeof(beard), &tmp, NULL);
	ReadFile(file, &mustache, sizeof(mustache), &tmp, NULL);
	ReadFile(file, &hair_color, sizeof(hair_color), &tmp, NULL);
	if(LOAD_VERSION < V_0_2_10)
		ReadFile(file, &height, sizeof(height), &tmp, NULL); // old weight
	ReadFile(file, &height, sizeof(height), &tmp, NULL);
}

//=================================================================================================
void HumanData::Write(BitStream& s) const
{
	s.WriteCasted<byte>(hair);
	s.WriteCasted<byte>(beard);
	s.WriteCasted<byte>(mustache);
	s.Write(hair_color.x);
	s.Write(hair_color.y);
	s.Write(hair_color.z);
	s.Write(height);
}

//=================================================================================================
int HumanData::Read(BitStream& s)
{
	if( !s.ReadCasted<byte>(hair) ||
		!s.ReadCasted<byte>(beard) ||
		!s.ReadCasted<byte>(mustache) ||
		!s.Read(hair_color.x) ||
		!s.Read(hair_color.y) ||
		!s.Read(hair_color.z) ||
		!s.Read(height))
		return 1;

	hair_color.w = 1.f;
	height = clamp(height, 0.9f, 1.1f);

	if(hair == 255)
		hair = -1;
	else if(hair > MAX_HAIR-2)
		return 2;

	if(beard == 255)
		beard = -1;
	else if(beard > MAX_BEARD-2)
		return 2;

	if(mustache == 255)
		mustache = -1;
	else if(mustache > MAX_MUSTACHE-2)
		return 2;

	return 0;
}
