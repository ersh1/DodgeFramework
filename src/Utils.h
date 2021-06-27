#pragma once

#define PI  3.14159265f
#define PI8 0.39269908f

inline RE::NiPoint2 Vec2Rotate(const RE::NiPoint2& vec, float angle)
{
	RE::NiPoint2 ret;
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = vec.x * sin(angle) + vec.y * cos(angle);
	return ret;
}

inline float Vec2Length(const RE::NiPoint2& vec)
{
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline RE::NiPoint2 Vec2Normalize(RE::NiPoint2& vec)
{
	RE::NiPoint2 ret(0.f, 0.f);
	float vecLength = Vec2Length(vec);
	if (vecLength == 0) {
		return ret;
	}
	float invlen = 1.0f / vecLength;
	ret.x = vec.x * invlen;
	ret.y = vec.y * invlen;
	return ret;
}

inline float DotProduct(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float CrossProduct(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline float GetAngle(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return atan2(CrossProduct(a, b), DotProduct(a, b));
}
