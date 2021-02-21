#include "Body.h"

template<> const DWORD Extension<IsometricTileTypeClass>::Canary = 0x23434657;
IsometricTileTypeExt::ExtContainer IsometricTileTypeExt::ExtMap;

void IsometricTileTypeExt::ExtData::SetConvert()
{
	if (this->Palette) {
		this->Convert = GameCreate<ConvertClass>(this->Palette, FileSystem::TEMPERAT_PAL, DSurface::Primary, 53, false);
	}
}

// =============================
// load / save

void IsometricTileTypeExt::ExtData::LoadFromINIFile(CCINIClass* const pINI) {
	CCINIClass* pART = CCINIClass::INI_Art;

	auto pThis = this->OwnerObject();
	const char* pSection = pThis->ID;

	if (!pART->GetSection(pSection)) {
		return;
	}

	pART->ReadString(pSection, "CustomPalette", this->PaletteBuffer, this->PaletteBuffer);

	if (this->PaletteBuffer[0]) {
		this->Palette = FileSystem::AllocatePalette(PaletteBuffer);
	}
}

void IsometricTileTypeExt::ExtData::LoadFromStream(IStream* Stm)
{
	#define STM_Process(A) Stm->Read(&A, sizeof(A), 0);
	#include "Serialize.hpp"
	#undef STM_Process

	if (this->PaletteBuffer[0]) {
		this->Palette = FileSystem::AllocatePalette(PaletteBuffer);
	}
}

void IsometricTileTypeExt::ExtData::SaveToStream(IStream* Stm)
{
	#define STM_Process(A) Stm->Write(&A, sizeof(A), 0);
	#include "Serialize.hpp"
	#undef STM_Process
}

// =============================
// container

IsometricTileTypeExt::ExtContainer::ExtContainer() : Container("IsometricTileTypeClass") {
}

IsometricTileTypeExt::ExtContainer::~ExtContainer() = default;

// =============================
// container hooks

DEFINE_HOOK(5449F2, IsometricTileTypeClass_CTOR, 5)
{
	GET(IsometricTileTypeClass*, pItem, EBP);

	IsometricTileTypeExt::ExtMap.FindOrAllocate(pItem);
	return 0;
}

DEFINE_HOOK(544BC2, IsometricTileTypeClass_DTOR, 8)
{
	GET(IsometricTileTypeClass*, pItem, ESI);

	IsometricTileTypeExt::ExtMap.Remove(pItem);
	return 0;
}

DEFINE_HOOK_AGAIN(549D70, IsometricTileTypeClass_SaveLoad_Prefix, 8)
DEFINE_HOOK(549C80, IsometricTileTypeClass_SaveLoad_Prefix, 5)
{
	GET_STACK(IsometricTileTypeClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	IsometricTileTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(549D8A, IsometricTileTypeClass_Save_Suffix, 6)
{
	auto pItem = IsometricTileTypeExt::ExtMap.Find(IsometricTileTypeExt::ExtMap.SavingObject);
	IStream* pStm = IsometricTileTypeExt::ExtMap.SavingStream;

	pItem->LoadFromStream(pStm);
	return 0;
}

DEFINE_HOOK(549D5D, IsometricTileTypeClass_Load_Suffix, 5)
{
	auto pItem = IsometricTileTypeExt::ExtMap.Find(IsometricTileTypeExt::ExtMap.SavingObject);
	IStream* pStm = IsometricTileTypeExt::ExtMap.SavingStream;

	pItem->SaveToStream(pStm);
	return 0;
}

DEFINE_HOOK(54642E, IsometricTileTypeClass_LoadFromINI, 6)
{
	GET(IsometricTileTypeClass*, pItem, EBP);
	LEA_STACK(CCINIClass*, pINI, 0x38);

	IsometricTileTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	return 0;
}
