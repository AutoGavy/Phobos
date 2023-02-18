#include "Body.h"

#include <Helpers/Macro.h>
#include <TagClass.h>

DEFINE_HOOK(0x689910, ScenarioClass_SetLocalToByID, 5)
{
	GET_STACK(const int, nIndex, 0x4);
	GET_STACK(const char, bState, 0x8);

	ScenarioExt::Global()->SetVariableToByID(false, nIndex, bState);
	return 0x689955u;
}

DEFINE_HOOK(0x689A00, ScenarioClass_GetLocalStateByID, 6)
{
	GET_STACK(const int, nIndex, 0x4);
	GET_STACK(char*, pOut, 0x8);

	ScenarioExt::Global()->GetVariableStateByID(false, nIndex, pOut);
	return 0x689A26u;
}

DEFINE_HOOK(0x689B20, ScenarioClass_ReadLocalVariables, 6)
{
	GET_STACK(CCINIClass*, pINI, 0x4);

	ScenarioExt::Global()->ReadVariables(false, pINI);
	return 0x689C4Bu;
}

DEFINE_HOOK(0x689670, ScenarioClass_SetGlobalToByID, 5)
{
	GET_STACK(const int, nIndex, 0x4);
	GET_STACK(const char, bState, 0x8);

	ScenarioExt::Global()->SetVariableToByID(true, nIndex, bState);
	return 0x6896AFu;
}

DEFINE_HOOK(0x689760, ScenarioClass_GetGlobalStateByID, 6)
{
	GET_STACK(const int, nIndex, 0x4);
	GET_STACK(char*, pOut, 0x8);

	ScenarioExt::Global()->GetVariableStateByID(true, nIndex, pOut);
	return 0x689786u;
}

// Called by MapGeneratorClass
DEFINE_HOOK(0x689880, ScenarioClass_ReadGlobalVariables, 6)
{
	GET_STACK(CCINIClass*, pINI, 0x4);

	ScenarioExt::Global()->ReadVariables(true, pINI);
	return 0x6898FFu;
}

// ScenarioClass_ReadGlobalVariables inlined in Read_Scenario_INI
DEFINE_HOOK(0x6876C2, ReadScenarioINI_Inlined_ReadGlobalVariables, 6)
{
	ScenarioExt::Global()->ReadVariables(true, CCINIClass::INI_Rules);

	R->ESI(GameMode::Campaign);
	return 0x68773Fu;
}
