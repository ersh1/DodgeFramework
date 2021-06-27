#include "Events.h"
#include "Utils.h"

namespace Events
{
	enum Direction : std::uint32_t
	{
		kNeutral = 0,
		kForward = 1,
		kRightForward = 2,
		kRight = 3,
		kRightBackward = 4,
		kBackward = 5,
		kLeftBackward = 6,
		kLeft = 7,
		kLeftForward = 8
	};

	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	auto InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
		-> EventResult
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;

		if (_key == kInvalid) {
			return EventResult::kContinue;
		}

		if (!a_event) {
			return EventResult::kContinue;
		}

		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType != EventType::kButton) {
				continue;
			}

			auto button = static_cast<RE::ButtonEvent*>(event);
			if (!button->IsDown()) {
				continue;
			}

			auto key = button->idCode;
			switch (button->device.get()) {
			case DeviceType::kMouse:
				key += kMouseOffset;
				break;
			case DeviceType::kKeyboard:
				key += kKeyboardOffset;
				break;
			case DeviceType::kGamepad:
			default:
				continue;
			}

			auto ui = RE::UI::GetSingleton();
			auto controlMap = RE::ControlMap::GetSingleton();
			if (ui->GameIsPaused() || !controlMap->IsMovementControlsEnabled()) {
				continue;
			}

			if (key == _key) {
				Dodge();
				break;
			}
		}

		return EventResult::kContinue;
	}

	void InputEventHandler::Dodge()
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerControls = RE::PlayerControls::GetSingleton();
		
		if (!playerCharacter || !playerControls)
		{
			return;
		}

		RE::BShkbAnimationGraph* animationGraph = nullptr;
		RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		if (playerCharacter->GetAnimationGraphManager(animationGraphManagerPtr)) {
			animationGraph = animationGraphManagerPtr->graphs[animationGraphManagerPtr->activeGraph].get();
		}

		if (!animationGraph)
		{
			return;
		}

		auto normalizedInputDirection = Vec2Normalize(playerControls->data.moveInputVec);
		if (normalizedInputDirection.x == 0.f && normalizedInputDirection.y == 0.f)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", PI);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kNeutral);
			playerCharacter->NotifyAnimationGraph("Dodge_N");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("neutral");
			return;
		}

		RE::NiPoint2 forwardVector(0.f, 1.f);
		float dodgeAngle = GetAngle(normalizedInputDirection, forwardVector);

		if (dodgeAngle >= -PI8 && dodgeAngle < PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kForward);
			playerCharacter->NotifyAnimationGraph("Dodge_F");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("forward");
		}
		else if (dodgeAngle >= PI8 && dodgeAngle < 3 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRightForward);
			playerCharacter->NotifyAnimationGraph("Dodge_RF");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right-forward");
		}
		else if (dodgeAngle >= 3 * PI8 && dodgeAngle < 5 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRight);
			playerCharacter->NotifyAnimationGraph("Dodge_R");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right");
		}
		else if (dodgeAngle >= 5 * PI8 && dodgeAngle < 7 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRightBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_RB");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right-backward");
		}
		else if (dodgeAngle >= 7 * PI8 || dodgeAngle < 7 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_B");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("backward");
		}
		else if (dodgeAngle >= 7 * -PI8 && dodgeAngle < 5 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeftBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_LB");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left-backward");
		}
		else if (dodgeAngle >= 5 * -PI8 && dodgeAngle < 3 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeft);
			playerCharacter->NotifyAnimationGraph("Dodge_L");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left");
		}
		else if (dodgeAngle >= 3 * -PI8 && dodgeAngle < -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeftForward);
			playerCharacter->NotifyAnimationGraph("Dodge_LF");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left-forward");
		}
	}

	bool InputEventHandler::Save(const SKSE::SerializationInterface* a_intfc, uint32_t a_typeCode, uint32_t a_version)
	{
		Locker locker(_lock);

		if (!a_intfc->OpenRecord(a_typeCode, a_version)) {
			return false;
		}

		if (!a_intfc->WriteRecordData(_key)) {
			return false;
		}

		return true;
	}

	bool InputEventHandler::Load(const SKSE::SerializationInterface* a_intfc)
	{
		Locker locker(_lock);

		if (!a_intfc->ReadRecordData(_key)) {
			return false;
		}

		return true;
	}

	void InputEventHandler::Clear()
	{
		Locker locker(_lock);
		_key = 47; // temp
	}

	uint32_t InputEventHandler::GetKey() const
	{
		Locker locker(_lock);
		return _key;
	}

	void InputEventHandler::SetKey(uint32_t a_key)
	{
		Locker locker(_lock);
		_key = a_key;
	}

	InputEventHandler::InputEventHandler() :
		_lock(),
		_key(47) // temp
	{}

	void SinkEventHandlers()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Added input event sink");
	}
}
