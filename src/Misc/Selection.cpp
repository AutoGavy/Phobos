#include <HouseClass.h>
#include <TacticalClass.h>
#include <Unsorted.h>

#include "Ext/TechnoType/Body.h"
#include "Phobos.h"

class ExtSelection
{
public:
	using callback_type = bool(__fastcall*)(ObjectClass*);

	// Reversed from Is_Selectable, w/o Select call
	static bool ObjectClass_IsSelectable(ObjectClass* pThis)
	{
		auto owner = pThis->GetOwningHouse();
		return owner && owner->IsControlledByCurrentPlayer()
			&& pThis->CanBeSelected() && pThis->CanBeSelectedNow()
			&& !pThis->InLimbo;
	}

	// Reversed from Tactical::Select
	static bool Tactical_IsInSelectionRect(TacticalClass* pThis, RECT* rect, TacticalSelectableStruct* selectable)
	{
		if (selectable->Techno && selectable->Techno->IsAlive)
		{
			LONG localX = selectable->X - pThis->TacticalPos.X;
			LONG localY = selectable->Y - pThis->TacticalPos.Y;

			if ((localX >= rect->left && localX < rect->right + rect->left) &&
				(localY >= rect->top && localY < rect->bottom + rect->top))
			{
				return true;
			}
		}
		return false;
	}

	static bool Tactical_IsHighPriorityInRect(TacticalClass* pThis, RECT* rect)
	{
		auto selected = Unsorted::TacticalSelectables();

		for (int i = 0; i < pThis->SelectableCount; ++i, ++selected)
		{
			if (Tactical_IsInSelectionRect(pThis, rect, selected) && ObjectClass_IsSelectable(selected->Techno))
			{
				auto technoTypeExt = TechnoTypeExt::ExtMap.Find(selected->Techno->GetTechnoType());

				if (!technoTypeExt->LowSelectionPriority)
					return true;
			}
		}

		return false;
	}

	// Reversed from Tactical::Select
	static void Tactical_SelectFiltered(TacticalClass* pThis, RECT* rect, callback_type check_callback, bool priorityFiltering)
	{
		Unsorted::MoveFeedback = true;

		if (rect->right <= 0 || rect->bottom <= 0 || pThis->SelectableCount <= 0)
		{
			return;
		}

		auto selected = Unsorted::TacticalSelectables();
		for (int i = 0; i < pThis->SelectableCount; ++i, ++selected)
		{
			if (Tactical_IsInSelectionRect(pThis, rect, selected))
			{
				auto techno = selected->Techno;
				auto technoType = techno->GetTechnoType();
				auto technoTypeExt = TechnoTypeExt::ExtMap.Find(technoType);

				if (priorityFiltering && technoTypeExt->LowSelectionPriority)
					continue;

				if (Game::IsTypeSelecting())
				{
					Game::UICommands_TypeSelect_7327D0(technoTypeExt->GetSelectionGroupID());
				}
				else if (check_callback)
				{
					(*check_callback)(techno);
				}
				else
				{
					bool isDeployedBuilding = false;
					if (techno->WhatAmI() == AbstractType::Building)
					{
						auto buildingType = abstract_cast<BuildingTypeClass*>(techno->GetType());

						if (buildingType->UndeploysInto && buildingType->IsUndeployable())
						{
							isDeployedBuilding = true;
						}
					}

					auto owner = techno->GetOwningHouse();
					if (owner && owner->IsControlledByCurrentPlayer() && techno->CanBeSelected()
						&& (techno->WhatAmI() != AbstractType::Building || isDeployedBuilding))
					{
						Unsorted::MoveFeedback = !techno->Select();
					}
				}
			}
		}

		Unsorted::MoveFeedback = true;
	}

	// Reversed from Tactical::MakeSelection
	static void Tactical_MakeFilteredSelection(TacticalClass* pThis, callback_type check_callback)
	{
		if (pThis->Band.Left || pThis->Band.Top)
		{
			LONG left = pThis->Band.Left;
			LONG right = pThis->Band.Right;
			LONG top = pThis->Band.Top;
			LONG bottom = pThis->Band.Bottom;

			if (left > right)
			{
				std::swap(left, right);
			}
			if (top > bottom)
			{
				std::swap(top, bottom);
			}

			RECT rect { left , top, right - left + 1, bottom - top + 1 };

			bool priorityFiltering = Tactical_IsHighPriorityInRect(pThis, &rect);
			Tactical_SelectFiltered(pThis, &rect, check_callback, priorityFiltering);

			pThis->Band.Left = 0;
			pThis->Band.Top = 0;
		}
	}
};

DEFINE_HOOK(0x6D9FF0, Tactical_MakeSelection_FilterSelection, 0)
{
	GET(TacticalClass*, pThis, ECX);
	GET_STACK(ExtSelection::callback_type, IsSelectable, 4);

	ExtSelection::Tactical_MakeFilteredSelection(pThis, IsSelectable);
	return 0x6DA075u;
}
