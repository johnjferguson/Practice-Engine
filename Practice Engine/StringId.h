#pragma once
#include "LeanWindows.h"
#include <functional>
#include <utility>

#define SID(id) std::integral_constant<StringId,HashString(id)>().value

typedef UINT64 StringId;

constexpr StringId HashString(const char* c)
{
	uint32_t crc = 0xFFFFFFFF;
	for (size_t i = 0; c[i]; i++)
	{
		char ch = c[i];
		for (size_t j = 0; j < 8; j++) {
			uint32_t b = (ch ^ crc) & 1;
			crc >>= 1;
			if (b) crc = crc ^ 0xEDB88320;
			ch >>= 1;
		}
	}
	return ~crc;
}

