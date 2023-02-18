#include <Utilities/Macro.h>

// Patches TechnoClass::Kill_Cargo/KillPassengers (push ESI -> push EBP)
// Fixes recursive passenger kills not being accredited
// to proper techno but to their transports
DEFINE_PATCH(0x707CF2, 0x55);
