#include <AircraftClass.h>
#include <AnimClass.h>
#include <BuildingClass.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <UnitClass.h>
#include <OverlayTypeClass.h>
#include <ScenarioClass.h>
#include <VoxelAnimClass.h>
#include <BulletClass.h>
#include <HouseClass.h>
#include <FlyLocomotionClass.h>
#include <JumpjetLocomotionClass.h>
#include <BombClass.h>
#include <WarheadTypeClass.h>
#include <Ext/Rules/Body.h>
#include <Ext/BuildingType/Body.h>
#include <Ext/Techno/Body.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>
#include <Utilities/TemplateDef.h>

//Replace: checking of HasExtras = > checking of (HasExtras && Shadow)
DEFINE_HOOK(0x423365, Phobos_BugFixes_SHPShadowCheck, 0x8)
{
	GET(AnimClass*, pAnim, ESI);

	auto const draw_shadow = pAnim->Type->Shadow && pAnim->HasExtras;

	return draw_shadow ? 0x42336Du : 0x4233EEu;
}

/*
	Allow usage of TileSet of 255 and above without making NE-SW broken bridges unrepairable

	When TileSet number crosses 255 in theater INI files, the NE-SW broken bridges
	become non-repairable. The reason is that the NonMarbleMadnessTile array of size 256
	overflows when filled and affects the variables like BridgeTopRight1 and BridgeBottomLeft2
	that come after it. This patch removes the filling of the unused NonMarbleMadnessTile array
	and its references.

	Author : E1 Elite
*/
DEFINE_JUMP(LJMP, 0x545CE2, 0x545CE9) //Phobos_BugFixes_Tileset255_RemoveNonMMArrayFill
DEFINE_JUMP(LJMP, 0x546C23, 0x546C8B) //Phobos_BugFixes_Tileset255_RefNonMMArray


// WWP's shit code! Wrong check.
// To avoid units dying when they are already dead.
DEFINE_HOOK(0x5F53AA, ObjectClass_ReceiveDamage_DyingFix, 0x6)
{
	enum { PostMortem = 0x5F583E, ContinueCheck = 0x5F53B0 };

	GET(int, health, EAX);
	GET(ObjectClass*, pThis, ESI);

	if (health <= 0 || !pThis->IsAlive)
		return PostMortem;

	return ContinueCheck;
}

DEFINE_HOOK(0x4D7431, FootClass_ReceiveDamage_DyingFix, 0x5)
{
	GET(FootClass*, pThis, ESI);
	GET(DamageState, result, EAX);

	if (result != DamageState::PostMortem && (pThis->IsSinking || (!pThis->IsAttackedByLocomotor && pThis->IsCrashing)))
		R->EAX(DamageState::PostMortem);

	return 0;
}

DEFINE_HOOK(0x737D57, UnitClass_ReceiveDamage_DyingFix, 0x7)
{
	GET(UnitClass*, pThis, ESI);
	GET(DamageState, result, EAX);

	// Immediately release locomotor warhead's hold on a crashable unit if it dies while attacked by one.
	if (result == DamageState::NowDead && pThis->IsAttackedByLocomotor && pThis->GetTechnoType()->Crashable)
		pThis->IsAttackedByLocomotor = false;

	if (result != DamageState::PostMortem && pThis->DeathFrameCounter > 0)
		R->EAX(DamageState::PostMortem);

	return 0;
}

DEFINE_HOOK(0x73B2A2, UnitClass_DrawObject_DrawerBlitterFix, 0x6)
{
	enum { SkipGameCode = 0x73B2C3 };

	GET(UnitClass* const, pThis, ESI);
	GET(BlitterFlags, blitterFlags, EDI);

	R->EAX(pThis->GetDrawer()->SelectPlainBlitter(blitterFlags));

	return SkipGameCode;
}

// BibShape checks for BuildingClass::BState which needs to not be 0 (constructing) for bib to draw.
// It is possible for BState to be 1 early during construction for frame or two which can result in BibShape being drawn during buildup, which somehow depends on length of buildup.
// Trying to fix this issue at its root is problematic and most of the time causes buildup to play twice, it is simpler to simply fix the BibShape to not draw until the buildup is done - Starkku
DEFINE_HOOK(0x43D874, BuildingClass_Draw_BuildupBibShape, 0x6)
{
	enum { DontDrawBib = 0x43D8EE };

	GET(BuildingClass*, pThis, ESI);

	if (!pThis->ActuallyPlacedOnMap)
		return DontDrawBib;

	return 0;
}

// Fix railgun target coordinates potentially differing from actual target coords.
DEFINE_HOOK(0x70C6B5, TechnoClass_Railgun_TargetCoords, 0x5)
{
	GET(AbstractClass*, pTarget, EBX);

	auto coords = pTarget->GetCenterCoords();

	if (const auto pBuilding = abstract_cast<BuildingClass*>(pTarget))
		coords = pBuilding->GetTargetCoords();
	else if (const auto pCell = abstract_cast<CellClass*>(pTarget))
		coords = pCell->GetCoordsWithBridge();

	R->EAX(&coords);
	return 0;
}

// Fix techno target coordinates (used for fire angle calculations, target lines etc) to take building target coordinate offsets into accord.
// This, for an example, fixes a vanilla bug where Destroyer has trouble targeting Naval Yards with its cannon weapon from certain angles.
DEFINE_HOOK(0x70BCE6, TechnoClass_GetTargetCoords_BuildingFix, 0x6)
{
	GET(TechnoClass*, pThis, ESI);

	if (const auto pBuilding = abstract_cast<BuildingClass*>(pThis->Target))
	{
		const auto coords = pBuilding->GetTargetCoords();
		R->EAX(&coords);
	}

	return 0;
}

DEFINE_HOOK(0x56BD8B, MapClass_PlaceRandomCrate_Sampling, 0x5)
{
	enum { SpawnCrate = 0x56BE7B, SkipSpawn = 0x56BE91 };

	int XP = 2 * MapClass::Instance->VisibleRect.X - MapClass::Instance->MapRect.Width
		+ ScenarioClass::Instance->Random.RandomRanged(0, 2 * MapClass::Instance->VisibleRect.Width);
	int YP = 2 * MapClass::Instance->VisibleRect.Y + MapClass::Instance->MapRect.Width
		+ ScenarioClass::Instance->Random.RandomRanged(0, 2 * MapClass::Instance->VisibleRect.Height + 2);
	CellStruct candidate { (short)((XP + YP) / 2),(short)((YP - XP) / 2) };

	auto pCell = MapClass::Instance->TryGetCellAt(candidate);
	if (!pCell)
		return SkipSpawn;

	if (!MapClass::Instance->IsWithinUsableArea(pCell, true))
		return SkipSpawn;

	REF_STACK(CellStruct, cell, STACK_OFFSET(0x28, -0x18));
	cell = MapClass::Instance->NearByLocation(pCell->MapCoords,
		pCell->LandType == LandType::Water ? SpeedType::Float : SpeedType::Track,
		-1, MovementZone::Normal, false, 1, 1, false, false, false, true, CellStruct::Empty, false, false);

	R->EAX(&cell);

	return SpawnCrate;
}
