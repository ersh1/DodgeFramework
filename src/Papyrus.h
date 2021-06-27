#pragma once

namespace Papyrus
{
	class Dodge
	{
	public:
		static uint32_t GetKey(RE::StaticFunctionTag*);
		static void SetKey(RE::StaticFunctionTag*, uint32_t a_key);

		static bool Register(RE::BSScript::IVirtualMachine* a_vm);
	};

	void Register();
}
