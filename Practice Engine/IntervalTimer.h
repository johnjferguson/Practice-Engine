#pragma once
#include <chrono>
#include <unordered_map>
#include <string>
#include "StringId.h"

class IntervalTimer
{
private:
	struct IntervalData
	{
		std::chrono::time_point<std::chrono::steady_clock> time_point;
		double time = 0.0f;
		bool closed = false;
	};
public:
	void Set(StringId id);
	double End(StringId id);
	double Peek(StringId id);
	void Print() const;
	void Clear();
	// prevent rehashing
	void Reserve(unsigned int n);
private:
	std::unordered_map<StringId, IntervalData> intervals;
};
