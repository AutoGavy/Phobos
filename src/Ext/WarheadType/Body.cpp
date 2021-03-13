#include "Body.h"
#include <WarheadTypeClass.h>
#include <AnimTypeClass.h>
#include <AnimClass.h>
#include <HouseClass.h>
#include <TechnoClass.h>
#include <ScenarioClass.h>

#include "../TechnoType/Body.h"
#include "../../Utilities/Helpers.Alex.h"

template<> const DWORD Extension<WarheadTypeClass>::Canary = 0x22222222;
WarheadTypeExt::ExtContainer WarheadTypeExt::ExtMap;

void WarheadTypeExt::ExtData::ApplyCrit(const CoordStruct& coords, TechnoClass* const Owner)
{
	auto& random = ScenarioClass::Instance->Random;
	if (this->Crit_Chance >= double(random.RandomRanged(1, 10) / 10))
	{
		auto const pWH = this->OwnerObject();

		auto items = Helpers::Alex::getCellSpreadItems(coords, this->Crit_Spread, true);
		for (const auto curTechno : items) {
			if (!curTechno || curTechno->InLimbo || !curTechno->IsAlive || !curTechno->Health) {
				continue;
			}

			if (auto pTypeExt = TechnoTypeExt::ExtMap.Find(curTechno->GetTechnoType())) {
				if (pTypeExt->ImmuneToCrit)
					continue;
			}

			if (Owner && !WarheadTypeExt::CanAffectTarget(curTechno, Owner->Owner, this->OwnerObject())) {
				continue;
			}

			if (!IsCellEligible(curTechno->GetCell(), this->Crit_Affects)) {
				continue;
			}

			if (!IsTechnoEligible(curTechno, this->Crit_Affects)) {
				continue;
			}

			auto damage = this->Crit_Damage;
			curTechno->ReceiveDamage(&damage, 0, pWH, Owner, false, false, Owner->Owner);

			if (this->Crit_Anims.size()) {
				GameCreate<AnimClass>(this->Crit_Anims[
					this->Crit_Anims.size() > 1 ?
					random.RandomRanged(0, this->Crit_Anims.size() - 1) : 0],
					curTechno->GetCoords());
			}
		}
	}
}

bool WarheadTypeExt::ExtData::IsCellEligible(CellClass* const pCell, SuperWeaponTarget allowed) noexcept
{
	if (allowed & SuperWeaponTarget::AllCells) {
		if (pCell->LandType == LandType::Water) {
			// check whether it supports water
			return (allowed & SuperWeaponTarget::Water) != SuperWeaponTarget::None;
		}
		else {
			// check whether it supports non-water
			return (allowed & SuperWeaponTarget::Land) != SuperWeaponTarget::None;
		}
	}
	return true;
}

bool WarheadTypeExt::ExtData::IsTechnoEligible(TechnoClass* const pTechno, SuperWeaponTarget allowed) noexcept
{
	if (allowed & SuperWeaponTarget::AllContents) {
		if (pTechno) {
			switch (pTechno->WhatAmI()) {
			case AbstractType::Infantry:
				return (allowed & SuperWeaponTarget::Infantry) != SuperWeaponTarget::None;
			case AbstractType::Unit:
			case AbstractType::Aircraft:
				return (allowed & SuperWeaponTarget::Unit) != SuperWeaponTarget::None;
			case AbstractType::Building:
				return (allowed & SuperWeaponTarget::Building) != SuperWeaponTarget::None;
			}
		}
		else {
			// is the target cell allowed to be empty?
			return (allowed & SuperWeaponTarget::NoContent) != SuperWeaponTarget::None;
		}
	}
	return true;
}

bool WarheadTypeExt::CanAffectTarget(TechnoClass* const pTarget, HouseClass* const pSourceHouse, WarheadTypeClass* const pWarhead)
{
	if (pSourceHouse && pTarget && pWarhead) {
		// apply AffectsAllies if owner and target house are allied
		if (pSourceHouse->IsAlliedWith(pTarget->Owner)) {
			return pWarhead->AffectsAllies;
		}

		// this warhead is designed to ignore enemy units
		const auto pExt = WarheadTypeExt::ExtMap.Find(pWarhead);
		return pExt->AffectsEnemies;
	}

	return true;
}

// =============================
// load / save

void WarheadTypeExt::ExtData::LoadFromINIFile(CCINIClass* const pINI) {
	auto pThis = this->OwnerObject();
	const char* pSection = pThis->ID;

	if (!pINI->GetSection(pSection)) {
		return;
	}

	INI_EX exINI(pINI);

	this->SpySat.Read(exINI, pSection, "SpySat");
	this->BigGap.Read(exINI, pSection, "BigGap");
	this->TransactMoney.Read(exINI, pSection, "TransactMoney");
	this->SplashList.Read(exINI, pSection, "SplashList");
	this->SplashList_PickRandom.Read(exINI, pSection, "SplashList.PickRandom");
	this->RemoveDisguise.Read(exINI, pSection, "RemoveDisguise");
	this->RemoveDisguise_AffectAllies.Read(exINI, pSection, "RemoveDisguise.AffectAllies");
	this->RemoveDisguise_ApplyCellSpread.Read(exINI, pSection, "RemoveDisguise.ApplyCellSpread");
	this->AffectsEnemies.Read(exINI, pSection, "AffectsEnemies");
	this->Crit_Damage.Read(exINI, pSection, "Crit.Damage");
	this->Crit_Spread.Read(exINI, pSection, "Crit.Spread");
	this->Crit_Chance.Read(exINI, pSection, "Crit.Chance");
	this->Crit_Affects.Read(exINI, pSection, "Crit.Affects");
	this->Crit_Anims.Read(exINI, pSection, "Crit.Anims");
}

void WarheadTypeExt::ExtData::LoadFromStream(IStream* Stm) {
	this->SpySat.Load(Stm);
	this->BigGap.Load(Stm);
	this->TransactMoney.Load(Stm);
	this->SplashList.Load(Stm);
	this->SplashList_PickRandom.Load(Stm);
	this->RemoveDisguise.Load(Stm);
	this->RemoveDisguise_AffectAllies.Load(Stm);
	this->RemoveDisguise_ApplyCellSpread.Load(Stm);
	this->AffectsEnemies.Load(Stm);
	this->Crit_Damage.Load(Stm);
	this->Crit_Spread.Load(Stm);
	this->Crit_Chance.Load(Stm);
	this->Crit_Affects.Load(Stm);
	this->Crit_Anims.Load(Stm);
}

void WarheadTypeExt::ExtData::SaveToStream(IStream* Stm) const {
	this->SpySat.Save(Stm);
	this->BigGap.Save(Stm);
	this->TransactMoney.Save(Stm);
	this->SplashList.Save(Stm);
	this->SplashList_PickRandom.Save(Stm);
	this->RemoveDisguise.Save(Stm);
	this->RemoveDisguise_AffectAllies.Save(Stm);
	this->RemoveDisguise_ApplyCellSpread.Save(Stm);
	this->AffectsEnemies.Save(Stm);
	this->Crit_Damage.Save(Stm);
	this->Crit_Spread.Save(Stm);
	this->Crit_Chance.Save(Stm);
	this->Crit_Affects.Save(Stm);
	this->Crit_Anims.Save(Stm);
}

// =============================
// container

WarheadTypeExt::ExtContainer::ExtContainer() : Container("WarheadTypeClass") {
}

WarheadTypeExt::ExtContainer::~ExtContainer() = default;

// =============================
// container hooks

DEFINE_HOOK(75D1A9, WarheadTypeClass_CTOR, 7)
{
	GET(WarheadTypeClass*, pItem, EBP);

	WarheadTypeExt::ExtMap.FindOrAllocate(pItem);
	return 0;
}

DEFINE_HOOK(75E5C8, WarheadTypeClass_SDDTOR, 6)
{
	GET(WarheadTypeClass*, pItem, ESI);

	WarheadTypeExt::ExtMap.Remove(pItem);
	return 0;
}

DEFINE_HOOK_AGAIN(75E2C0, WarheadTypeClass_SaveLoad_Prefix, 5)
DEFINE_HOOK(75E0C0, WarheadTypeClass_SaveLoad_Prefix, 8)
{
	GET_STACK(WarheadTypeClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	WarheadTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(75E2AE, WarheadTypeClass_Load_Suffix, 7)
{
	auto pItem = WarheadTypeExt::ExtMap.Find(WarheadTypeExt::ExtMap.SavingObject);
	IStream* pStm = WarheadTypeExt::ExtMap.SavingStream;

	pItem->LoadFromStream(pStm);
	return 0;
}

DEFINE_HOOK(75E39C, WarheadTypeClass_Save_Suffix, 5)
{
	auto pItem = WarheadTypeExt::ExtMap.Find(WarheadTypeExt::ExtMap.SavingObject);
	IStream* pStm = WarheadTypeExt::ExtMap.SavingStream;

	pItem->SaveToStream(pStm);
	return 0;
}

DEFINE_HOOK_AGAIN(75DEAF, WarheadTypeClass_LoadFromINI, 5)
DEFINE_HOOK(75DEA0, WarheadTypeClass_LoadFromINI, 5)
{
	GET(WarheadTypeClass*, pItem, ESI);
	GET_STACK(CCINIClass*, pINI, 0x150);

	WarheadTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	return 0;
}
