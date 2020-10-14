#pragma once
#include <vector>

template <class V>
struct IndexedTriangleList
{
	std::vector<V> vertices;
	std::vector<unsigned short> indices;
};