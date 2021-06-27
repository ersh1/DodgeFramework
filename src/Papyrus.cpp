#include "Papyrus.h"

#include "Events.h"

namespace Papyrus
{
	uint32_t Dodge::GetKey(RE::StaticFunctionTag*)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		return inputHandler->GetKey();
	}

	void Dodge::SetKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		inputHandler->SetKey(a_key);
	}

	bool Dodge::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetKey", "DodgeFramework", GetKey);
		a_vm->RegisterFunction("SetKey", "DodgeFramework", SetKey);
		logger::info("Registered DodgeFramework class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Dodge::Register);
		logger::info("Registered papyrus functions");
	}
}
