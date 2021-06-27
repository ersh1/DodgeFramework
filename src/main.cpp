#include "Events.h"
#include "Papyrus.h"

enum : std::uint32_t
{
	kSerializationVersion = 1,

	kDodge = 'DODG',
	kInputEventHandler = 'INPT'
};

std::string DecodeTypeCode(std::uint32_t a_typeCode)
{
	constexpr std::size_t SIZE = sizeof(std::uint32_t);

	std::string sig;
	sig.resize(SIZE);
	char* iter = reinterpret_cast<char*>(&a_typeCode);
	for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
		sig[j] = iter[i];
	}
	return sig;
}

void SaveCallback(SKSE::SerializationInterface* a_intfc)
{
	auto inputEventHandler = Events::InputEventHandler::GetSingleton();
	if (!inputEventHandler->Save(a_intfc, kInputEventHandler, kSerializationVersion)) {
		logger::error("Failed to save InputEventHandler!\n");
	}

	logger::info("Finished saving data");
}

void LoadCallback(SKSE::SerializationInterface* a_intfc)
{
	auto inputEventHandler = Events::InputEventHandler::GetSingleton();
	inputEventHandler->Clear();

	std::uint32_t type;
	std::uint32_t version;
	std::uint32_t length;
	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		if (version != kSerializationVersion) {
			logger::error("Loaded data is out of date! Read (%u), expected (%u) for type code (%s)", version, kSerializationVersion, DecodeTypeCode(type).c_str());
			continue;
		}

		switch (type) {
		case kInputEventHandler:
			if (!inputEventHandler->Load(a_intfc)) {
				inputEventHandler->Clear();
				logger::error("Failed to load InputEventHandler!\n");
			}
			break;
		default:
			logger::error("Unrecognized record type (%s)!", DecodeTypeCode(type).c_str());
			break;
		}
	}

	logger::info("Finished loading data");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::SinkEventHandlers();
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("Dodge Framework loaded");

	SKSE::Init(a_skse);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Papyrus::Register();

	auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(kDodge);
	serialization->SetSaveCallback(SaveCallback);
	serialization->SetLoadCallback(LoadCallback);

	return true;
}
