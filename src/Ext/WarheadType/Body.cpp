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
	if (random.RandomRanged(1, 10) / 10 >= this->CritChance)
	{
		auto const pWH = this->OwnerObject();

		auto items = Helpers::Alex::getCellSpreadItems(coords, this->CritSpread, true);
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

			if (!IsCellEligible(curTechno->GetCell(), this->CritAffects)) {
				continue;
			}

			auto damage = this->CritDamage;
			curTechno->ReceiveDamage(&damage, 0, pWH, Owner, false, false, Owner->Owner);

			if (this->CritAnims.size()) {
				GameCreate<AnimClass>(this->CritAnims[
					this->CritAnims.size() > 1 ?
					random.RandomRanged(0, this->CritAnims.size() - 1) : 0],
					curTechno->GetCoords());
			}
		}
	}
}

bool WarheadTypeExt::ExtData::IsCellEligible(CellClass* const pCell, WarheadTarget allowed) noexcept
{
	if (allowed & WarheadTarget::AllCells) {
		if (pCell->LandType == LandType::Water) {
			// check whether it supports water
			return (allowed & WarheadTarget::Water) != WarheadTarget::None;
		}
		else {
			// check whether it supports non-water
			return (allowed & WarheadTarget::Land) != WarheadTarget::None;
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
	this->CritDamage.Read(exINI, pSection, "CritDamage");
	this->CritSpread.Read(exINI, pSection, "CritSpread");
	this->CritChance.Read(exINI, pSection, "CritChance");
	this->CritAffects.Read(exINI, pSection, "CritAffects");
	this->CritAnims.Read(exINI, pSection, "CritAnims");
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
	this->CritDamage.Load(Stm);
	this->CritSpread.Load(Stm);
	this->CritChance.Load(Stm);
	this->CritAffects.Load(Stm);
	this->CritAnims.Load(Stm);
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
	this->CritDamage.Save(Stm);
	this->CritSpread.Save(Stm);
	this->CritChance.Save(Stm);
	this->CritAffects.Save(Stm);
	this->CritAnims.Save(Stm);
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
