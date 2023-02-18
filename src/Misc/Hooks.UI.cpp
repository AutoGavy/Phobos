#include <Helpers/Macro.h>

DEFINE_HOOK(0x640B8D, LoadingScreen_DisableEmptySpawnPositions, 0x6)
{
	GET(const bool, bESI, ESI);
	return bESI ? 0x640B93u : 0x640CE2u;
}

DEFINE_HOOK(0x641B41, LoadingScreen_SkipPreview, 0x8)
{
	return 0x641D4E;
}
