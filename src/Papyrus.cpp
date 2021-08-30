#include "Papyrus.h"

#include "Events.h"
#include "Settings.h"

namespace Papyrus
{
	void DodgeFramework_MCM::OnConfigClose(RE::TESQuest*)
	{
		Settings::ReadSettings();
	}

	bool DodgeFramework_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OnConfigClose", "DodgeFramework_MCM", OnConfigClose);

		logger::info("Registered DodgeFramework_MCM class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(DodgeFramework_MCM::Register);
		logger::info("Registered papyrus functions");
	}
}
