#pragma once

struct Settings
{
	static void ReadSettings();

	// Controls
	static inline bool bUseSprintButton = true;
	static inline float fSprintHoldDuration = 0.25f;
	static inline std::uint32_t uDodgeKey = static_cast<std::uint32_t>(-1);
};
