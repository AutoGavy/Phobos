#include "Body.h"

DEFINE_HOOK(547CF0, IsometricTileTypeClass_DrawTMP_SetPalette, 7)
{
	GET(IsometricTileTypeClass*, IsometricTileType, ECX);

	auto pData = IsometricTileTypeExt::ExtMap.Find(IsometricTileType);

	// nooo
	/*
	if (pData->Palette) {
		if (!pData->Convert)
			pData->SetConvert();

		if (pData->Convert) {
			R->ESI<ConvertClass*>(pData->Convert);
			return 0x547CFFu;
		}
	}
	*/

	return 0;
}
