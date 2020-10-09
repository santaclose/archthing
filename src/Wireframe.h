#pragma once
#include <modelTool/vl.h>
#include <vector>

enum class EdgeType { Wall = 0, Door = 1, Window = 2, StandardStairs = 3, SpiralStairs = 4 };
struct ge;
struct gv
{
	int id;
	std::vector<int> conn;
	vec pos;
};
struct ge
{
	EdgeType type;
	int a;
	int b;
};
struct Wireframe
{
	std::vector<gv> vertices;
	std::vector<ge> edges;
};