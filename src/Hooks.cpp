#include "Hooks.h"

#include "Events.h"
#include "Settings.h"

namespace Hooks
{
	void Install()
	{
		logger::trace("Hooking...");

		SprintHandlerHook::Hook();

		logger::trace("...success");
	}

	static bool bStoppingSprint = false;
	//static float heldDownTimeOffset = 0.f;

	void SprintHandlerHook::ProcessButton(RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		using FlagBDD = RE::PlayerCharacter::FlagBDD;

		if (Settings::bUseSprintButton) {
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->sprint) {
				if (a_event->IsDown() && (playerCharacter->unkBDD & FlagBDD::kSprinting) != FlagBDD::kNone) { // stopping sprint
					bStoppingSprint = true;
				} else if (a_event->HeldDuration() < Settings::fSprintHoldDuration) {
					if (a_event->IsUp())
					{
						Events::Dodge();
						bStoppingSprint = false;
					}
					return;
				} else if (playerCharacter && (playerCharacter->unkBDD & FlagBDD::kSprinting) == FlagBDD::kNone && !bStoppingSprint) {
					a_event->heldDownSecs = 0.f;
				} else if (a_event->IsUp()) {
					bStoppingSprint = false;
				}
			}
		}

		/*if (Settings::bUseSprintButton) {
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->sprint) {
				if (a_event->HeldDuration() < Settings::fSprintHoldDuration) {
					if (a_event->IsUp()) {
						Events::Dodge();
						heldDownTimeOffset = 0.f;
					}
					return;
				} else if (a_event->IsUp()) {
					heldDownTimeOffset = 0.f;
				} else {
					if (heldDownTimeOffset == 0.f) {
						heldDownTimeOffset = a_event->HeldDuration();
					}
					a_event->heldDownSecs -= heldDownTimeOffset;
				}
			}
		}*/
				
		_ProcessButton(a_this, a_event, a_data);
	}
}
