#pragma once

struct Point2i
{
	int x;
	int y;

	Point2i operator-(Point2i other)
	{
		return { x - other.x, y - other.y };
	}
};
