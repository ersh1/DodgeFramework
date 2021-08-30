#include "Settings.h"

void Settings::ReadSettings()
{
	constexpr auto path = L"Data/MCM/Settings/DodgeFramework.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	bUseSprintButton = ini.GetBoolValue("Controls", "bUseSprintButton", true);
	fSprintHoldDuration = (float)ini.GetDoubleValue("Controls", "fSprintHoldDuration", 0.25f);
	uDodgeKey = static_cast<uint32_t>(ini.GetLongValue("Controls", "uDodgeKey"));
}
