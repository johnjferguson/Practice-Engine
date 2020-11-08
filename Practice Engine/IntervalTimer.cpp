#include "IntervalTimer.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

void IntervalTimer::Set(StringId id)
{
	
	if (intervals.find(id) == intervals.end())
	{
		intervals.insert({ id, {std::chrono::steady_clock::now(), 0.0f, false} });
	}
	else
	{
		intervals[id] = { std::chrono::steady_clock::now(), 0.0f, false };
	}
	
}

double IntervalTimer::End(StringId id)
{
	// take time before somewhat expensive string hash
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	std::unordered_map<StringId, IntervalData>::iterator
		iter = intervals.find(id);

	if (iter != intervals.end())
	{
		std::chrono::duration<float> time = now - iter->second.time_point;
		iter->second.time = time.count();
		iter->second.closed = true;
		return iter->second.time;
	}
	return 0.0f;
}

double IntervalTimer::Peek(StringId id)
{
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	std::unordered_map<StringId, IntervalData>::iterator
		iter = intervals.find(id);

	if (iter != intervals.end())
	{
		std::chrono::duration<float> time = now - iter->second.time_point;
		iter->second.time = time.count();
		return iter->second.time;
	}
	return 0.0f;
}

void IntervalTimer::Print() const
{
	/*
	unsigned int max_length = 0u;
	std::for_each(intervals.begin(), intervals.end(), [&](const std::pair<std::string, IntervalTimer::IntervalData>& data) 
	{ 
		if (data.first.length() > max_length)
			max_length = unsigned (data.first.length());
	});


	std::for_each(intervals.begin(), intervals.end(), [&](const std::pair<std::string, IntervalTimer::IntervalData>& data) 
	{
		if (data.second.closed)
		{
			std::cout << "Interval: " << std::setw(max_length) << std::left << data.first << " Time: " << data.second.time << std::endl;
		}
	}
	);
	*/
	
}
