#include <Helpers/Macro.h>
#include <PCX.h>
#include <FileFormats/SHP.h>
#include <LoadProgressManager.h>
#include <GameStrings.h>

DEFINE_HOOK(0x6B9D9C, RGB_PCX_Loader, 7)
{
	GET(BSurface*, pSurf, EDI);

	return pSurf->BytesPerPixel == 2 ?
		0x6B9EE7u : 0u;
}

DEFINE_HOOK(0x5535D0, PCX_LoadScreen_Skirmish, 6)
{
	LEA_STACK(char*, name, 0x84);

	char pFilename[0x20];
	strcpy_s(pFilename, name);
	_strlwr_s(pFilename);

	if (strstr(pFilename, ".pcx") || strstr(pFilename, "obs.shp")) {
		GET_BASE(DSurface* const, pSurf, 0x60);
		PCX::Instance->LoadFile(pFilename);

		auto pcx = PCX::Instance->GetSurface(pFilename);
		if (pcx) {
			RectangleStruct pSurfBounds = { 0, 0, pSurf->Width, pSurf->Height };
			RectangleStruct pcxBounds = { 0, 0, pcx->Width, pcx->Height };

			RectangleStruct destClip = { 0, 0, pcx->Width, pcx->Height };
			destClip.X = (pSurf->Width - pcx->Width) / 2;
			destClip.Y = (pSurf->Height - pcx->Height) / 2;

			pSurf->CopyFrom(&pSurfBounds, &destClip, pcx, &pcxBounds, &pcxBounds, true, true);
		}
		return 0x553603u;
	}

	return 0;
}

DEFINE_HOOK(0x552FC9, PCX_LoadScreen_Campaign, 6)
{
	char pFilename[0x20];
	strcpy_s(pFilename, ScenarioClass::Instance->LS800BkgdName);
	_strlwr_s(pFilename);

	if (strstr(pFilename, ".pcx")) {
		GET_BASE(DSurface* const, pSurf, 0x60);
		PCX::Instance->LoadFile(pFilename);

		auto pcx = PCX::Instance->GetSurface(pFilename);
		if (pcx) {
			RectangleStruct pSurfBounds = { 0, 0, pSurf->Width, pSurf->Height };
			RectangleStruct pcxBounds = { 0, 0, pcx->Width, pcx->Height };

			RectangleStruct destClip = { 0, 0, pcx->Width, pcx->Height };
			destClip.X = (pSurf->Width - pcx->Width) / 2;
			destClip.Y = (pSurf->Height - pcx->Height) / 2;

			pSurf->CopyFrom(&pSurfBounds, &destClip, pcx, &pcxBounds, &pcxBounds, true, true);
		}
		return 0x552FFFu;
	}

	return 0;
}
