#include "Pch.h"
#include "GameCore.h"
#include "Buff.h"
#include "Language.h"
#include "ResourceManager.h"

BuffInfo BuffInfo::info[] = {
	BuffInfo("buff_regeneration"),
	BuffInfo("buff_natural"),
	BuffInfo("buff_food"),
	BuffInfo("buff_alcohol"),
	BuffInfo("buff_poison"),
	BuffInfo("buff_antimagic"),
	BuffInfo("buff_stamina"),
	BuffInfo("buff_stun"),
	BuffInfo("buff_poison_res")
};

void BuffInfo::LoadImages()
{
	auto& tex_mgr = ResourceManager::Get<Texture>();
	for(int i=0; i<BUFF_COUNT; ++i)
	{
		auto& buff = info[i];
		cstring path = Format("%s.png", buff.id);
		tex_mgr.AddLoadTask(path, buff.img);
	}
}

void BuffInfo::LoadText()
{
	Language::Section& s = Language::GetSection("Buffs");
	for(int i=0; i<BUFF_COUNT; ++i)
	{
		auto& buff = info[i];
		buff.text = s.Get(buff.id);
	}
}
