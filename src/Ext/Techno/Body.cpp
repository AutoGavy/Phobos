#include "Body.h"
#include <Utilities/EnumFunctions.h>

template<> const DWORD Extension<TechnoClass>::Canary = 0x55555555;
TechnoExt::ExtContainer TechnoExt::ExtMap;

CoordStruct TechnoExt::PassengerKickOutLocation(TechnoClass* pThis, FootClass* pPassenger, int maxAttempts = 1)
{
	if (!pThis || !pPassenger)
		return CoordStruct::Empty;

	if (maxAttempts < 1)
		maxAttempts = 1;

	CellClass* pCell;
	CellStruct placeCoords = CellStruct::Empty;
	auto pTypePassenger = pPassenger->GetTechnoType();
	CoordStruct finalLocation = CoordStruct::Empty;
	short extraDistanceX = 1;
	short extraDistanceY = 1;
	SpeedType speedType = pTypePassenger->SpeedType;
	MovementZone movementZone = pTypePassenger->MovementZone;

	if (pTypePassenger->WhatAmI() == AbstractType::AircraftType)
	{
		speedType = SpeedType::Track;
		movementZone = MovementZone::Normal;
	}

	do
	{
		placeCoords = pThis->GetCell()->MapCoords - CellStruct { (short)(extraDistanceX / 2), (short)(extraDistanceY / 2) };
		placeCoords = MapClass::Instance->NearByLocation(placeCoords, speedType, -1, movementZone, false, extraDistanceX, extraDistanceY, true, false, false, false, CellStruct::Empty, false, false);

		pCell = MapClass::Instance->GetCellAt(placeCoords);
		extraDistanceX += 1;
		extraDistanceY += 1;
	}
	while (extraDistanceX < maxAttempts && (pThis->IsCellOccupied(pCell, -1, -1, nullptr, false) != Move::OK) && pCell->MapCoords != CellStruct::Empty);

	pCell = MapClass::Instance->TryGetCellAt(placeCoords);
	if (pCell)
		finalLocation = pCell->GetCoordsWithBridge();

	return finalLocation;
}

// =============================
// load / save

template <typename T>
void TechnoExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->LastKillWasTeamTarget)
		;
}

void TechnoExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TechnoClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TechnoExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TechnoClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

bool TechnoExt::LoadGlobals(PhobosStreamReader& Stm)
{
	return Stm
		.Success();
}

bool TechnoExt::SaveGlobals(PhobosStreamWriter& Stm)
{
	return Stm
		.Success();
}

// =============================
// container

TechnoExt::ExtContainer::ExtContainer() : Container("TechnoClass") { }

TechnoExt::ExtContainer::~ExtContainer() = default;

void TechnoExt::ExtContainer::InvalidatePointer(void* ptr, bool bRemoved) { }

// =============================
// container hooks

DEFINE_HOOK(0x6F3260, TechnoClass_CTOR, 0x5)
{
	GET(TechnoClass*, pItem, ESI);

	TechnoExt::ExtMap.FindOrAllocate(pItem);

	return 0;
}

DEFINE_HOOK(0x6F4500, TechnoClass_DTOR, 0x5)
{
	GET(TechnoClass*, pItem, ECX);

	TechnoExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x70C250, TechnoClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x70BF50, TechnoClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(TechnoClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	TechnoExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x70C249, TechnoClass_Load_Suffix, 0x5)
{
	TechnoExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x70C264, TechnoClass_Save_Suffix, 0x5)
{
	TechnoExt::ExtMap.SaveStatic();

	return 0;
}
