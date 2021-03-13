#pragma once

#include <CCINIClass.h>
#include <WarheadTypeClass.h>
#include <CellClass.h>

#include "../_Container.hpp"
#include "../../Phobos.h"

#include "../../Utilities/Debug.h"
#include "../../Utilities/TemplateDef.h"

class WarheadTypeExt
{
public:
	using base_type = WarheadTypeClass;

	class ExtData final : public Extension<WarheadTypeClass>
	{
	public:

		Valueable<bool> SpySat;
		Valueable<bool> BigGap;
		Valueable<int> TransactMoney;
		ValueableVector<AnimTypeClass*> SplashList;
		Valueable<bool> SplashList_PickRandom;
		Valueable<bool> RemoveDisguise;
		Valueable<bool> RemoveDisguise_AffectAllies;
		Valueable<bool> RemoveDisguise_ApplyCellSpread;
		Valueable<bool> AffectsEnemies;
		Valueable<int> Crit_Damage;
		Valueable<double> Crit_Spread;
		Valueable<double> Crit_Chance;
		Valueable<SuperWeaponTarget> Crit_Affects;
		ValueableVector<AnimTypeClass*> Crit_Anims;

		ExtData(WarheadTypeClass* OwnerObject) : Extension<WarheadTypeClass>(OwnerObject),
			SpySat(false),
			BigGap(false),
			TransactMoney(0),
			SplashList(),
			SplashList_PickRandom(false),
			RemoveDisguise(false),
			RemoveDisguise_AffectAllies(false),
			RemoveDisguise_ApplyCellSpread(true),
			AffectsEnemies(true),
			Crit_Damage(0),
			Crit_Spread(0.0),
			Crit_Chance(0.0),
			Crit_Affects(SuperWeaponTarget::None),
			Crit_Anims()
		{ }

		void ApplyCrit(const CoordStruct& coords, TechnoClass* const Owner);
		bool IsCellEligible(CellClass* const pCell, SuperWeaponTarget allowed) noexcept;
		bool IsTechnoEligible(TechnoClass* const pCell, SuperWeaponTarget allowed) noexcept;

		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual ~ExtData() = default;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override {}

		virtual void LoadFromStream(IStream* Stm);

		virtual void SaveToStream(IStream* Stm) const;
	};

	static void ReshroudMapForOpponents(HouseClass* pThisHouse);

	class ExtContainer final : public Container<WarheadTypeExt> {
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;
	static bool CanAffectTarget(TechnoClass* const pTarget, HouseClass* const pSourceHouse, WarheadTypeClass* const pWarhead);
};
