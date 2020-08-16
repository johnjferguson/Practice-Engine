#pragma once
#include <chrono>
#include <unordered_map>
#include <string>

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
	void Set(const std::string& interval_id);
	double End(const std::string& interval_id);
	double Peek(const std::string& interval_id);
	void Print() const;
	void Clear();
	// prevent rehashing
	void Reserve(unsigned int n);
private:
	std::unordered_map<std::string, IntervalData> intervals;
};
