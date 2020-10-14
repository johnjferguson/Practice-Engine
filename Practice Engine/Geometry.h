#pragma once
#include <assert.h>
#include "IndexedTriangleList.h"

template <class V>
IndexedTriangleList<V> MakeCube(float length, float width, float height)
{
	assert(length > 0 && width > 0 && height > 0 && "MakeCube invalid length, width or height");

	float hl = length / 2.0f;
	float hw = width / 2.0f;
	float hh = height / 2.0f;

	std::vector<V> vertices =
	{
		{-hw, -hh, -hl},
		{-hw,  hh, -hl},
		{ hw,  hh, -hl},
		{ hw, -hh, -hl},
		{-hw, -hh,  hl},
		{-hw,  hh,  hl},
		{ hw,  hh,  hl},
		{ hw, -hh,  hl}
	};

	std::vector<unsigned short> indices =
	{
		0,1,2,  0,2,3,
		7,6,5,  7,5,4,
		4,5,1,  4,1,0,
		3,2,6,  3,6,7,
		1,5,6,  1,6,2,
		4,0,3,  4,3,7
	};

	return { vertices, indices };
}
