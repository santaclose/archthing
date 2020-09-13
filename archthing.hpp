#include "../../src/modelTool/ml.h"
#include <fstream>
#include <string>
#define MODEL_SET
#define BINDINGS

float inputScale = 1.0f;
float wallHeight = 4.26f;
float wallThickness = 0.0889f;
float doorHeight = 2.1f;
float doorSeparation = 0.003;
float doorThickness = 0.025;
float doorHandlePosX = 0.056f;
float doorHandlePosY = 1.0f;
float glassThickness = 0.003175f;
float windowOffset = 1.1f;
float windowHeight = 2.0f;
float windowFrameWidth = 0.05f;
float windowFrameThickness = 0.02f;
float roofThickness = 0.3f;
float roofBevel = 0.3f;
float roofDepth = 0.2f;
float floorThickness = 0.25f;

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

#include "archutils.hpp"

#include "basicWall.hpp"
#include "basicDoor.hpp"
#include "basicWindow.hpp"
#include "basicStairs.hpp"
#include "basicRoof.hpp"
#include "basicFloor.hpp"
//#include "spiralStairs.hpp"

void bindings()
{
	BIND(SliderFloat, "Input Scale", &inputScale, 0.01f, 20.0f);
	BIND(SliderFloat, "Wall Height", &wallHeight, 1.0f, 20.0f);
	BIND(SliderFloat, "Wall Thickness", &wallThickness, 0.01f, 0.3f);
	BIND(SliderFloat, "Door Height", &doorHeight, 1.5f, 4.0f);
	BIND(SliderFloat, "Door Separation", &doorSeparation, 0.0f, 0.3f);
	BIND(SliderFloat, "Door Thickness", &doorThickness, 0.01f, 0.05f);
	BIND(SliderFloat, "Door Handle Pos X", &doorHandlePosX, 0.0f, 0.3f);
	BIND(SliderFloat, "Door Handle Pos Y", &doorHandlePosY, 0.0f, 1.5f);
	BIND(SliderFloat, "Glass Thickness", &glassThickness, 0.001f, 0.2f);
	BIND(SliderFloat, "Window Offset", &windowOffset, 0.5f, 2.0f);
	BIND(SliderFloat, "Window Height", &windowHeight, 0.5f, 3.0f);
	BIND(SliderFloat, "Window Frame Width", &windowFrameWidth, 0.01f, 0.1f);
	BIND(SliderFloat, "Window Frame Thickness", &windowFrameThickness, 0.01f, 0.3f);
	BIND(SliderFloat, "Roof Thickness", &roofThickness, 0.01f, 0.6f);
	BIND(SliderFloat, "Roof Bevel", &roofBevel, 0.01f, 0.6f);
	BIND(SliderFloat, "Roof Depth", &roofDepth, 0.01f, 0.6f);
	BIND(SliderFloat, "Floor Thickness", &floorThickness, 0.01f, 0.6f);
}

void createWallVertices(const std::vector<gv>& vertices, const std::vector<ge>& edges, std::vector<std::vector<std::pair<unsigned int, unsigned int>>>& wallVertices)
{
	wallVertices.resize(vertices.size());

	for (int i = 0; i < vertices.size(); i++)
	{
		//std::cout << "iteration " << i << std::endl;

		std::vector<vec> adj;
		bool isStairsVertex = false;
		for (int c : vertices[i].conn)
		{
			if (edges[c].type >= EdgeType::StandardStairs)
			{
				isStairsVertex = true;
				break;
			}
			adj.push_back(edges[c].a == i ? vertices[edges[c].b].pos : vertices[edges[c].a].pos);
		}

		if (!isStairsVertex)
		{
			if (adj.size() == 1)
			{
				{
					vec corner = vertices[i].pos + (vec::up * (adj[0] - vertices[i].pos)).Normalized() * (wallThickness * 0.5f);
					unsigned int base = ml::vertex(corner);
					unsigned int top = ml::vertex(corner + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Door::Cylinder(0.01f, 3, corner, corner + vec::up * wallHeight);
				}
				{
					vec corner = vertices[i].pos + ((adj[0] - vertices[i].pos) * vec::up).Normalized() * (wallThickness * 0.5f);
					unsigned int base = ml::vertex(corner);
					unsigned int top = ml::vertex(corner + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Door::Cylinder(0.01f, 3, corner, corner + vec::up * wallHeight);
				}
			}
			else
			{
				std::vector<vec> connectionPoints;
				Utils::getConnectionPoints(vertices[i].pos, adj, wallThickness, connectionPoints);
				for (vec& cp : connectionPoints)
				{
					unsigned int base = ml::vertex(cp);
					unsigned int top = ml::vertex(cp + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Door::Cylinder(0.01f, 3, cp, cp + vec::up * wallHeight);
				}
			}
		}
	}
}

void generateModel()
{
	std::vector<gv> vertices;
	std::vector<ge> edges;
	unsigned int floorCount;
	Utils::readOBJ(vertices, edges, floorCount, "assets/testHF.obj");

	std::cout << "-- removing repeated vertices\n";
	Utils::removeDuplicated(vertices, edges);

	std::cout << "-- sorting vertex connections\n";
	Utils::sortVertexConnectionsByAngle(vertices, edges);
	std::vector<std::vector<std::pair<unsigned int, unsigned int>>> wallVertices;

	std::cout << "-- creating wall vertices\n";
	createWallVertices(vertices, edges, wallVertices);

	std::cout << "-- creating geometry\n";

	unsigned int currentEdge = 0;
	for (ge& edge : edges)
	{
		vec displacement = vertices[edge.b].pos - vertices[edge.a].pos;

		// get wall vertices for edge
		unsigned int currentEdgeWallVertices[8];
		if (edge.type < EdgeType::StandardStairs)
		{
			// find vertex connection indices
			int i = 0, j = 0;
			for (; i < vertices[edge.a].conn.size(); i++)
				if (vertices[edge.a].conn[i] == currentEdge)
					break;
			for (; j < vertices[edge.b].conn.size(); j++)
				if (vertices[edge.b].conn[j] == currentEdge)
					break;
			currentEdgeWallVertices[0] = wallVertices[edge.a][((i + 0 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].first;
			currentEdgeWallVertices[1] = wallVertices[edge.a][((i - 1 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].first;
			currentEdgeWallVertices[2] = wallVertices[edge.a][((i - 1 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].second;
			currentEdgeWallVertices[3] = wallVertices[edge.a][((i + 0 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].second;

			currentEdgeWallVertices[4] = wallVertices[edge.b][((j - 1 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].first;
			currentEdgeWallVertices[5] = wallVertices[edge.b][((j + 0 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].first;
			currentEdgeWallVertices[6] = wallVertices[edge.b][((j + 0 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].second;
			currentEdgeWallVertices[7] = wallVertices[edge.b][((j - 1 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].second;
		}

		switch (edge.type)
		{
		case EdgeType::Wall:
			Wall::Create(currentEdgeWallVertices, vertices, edges, edge);
			break;
		case EdgeType::Door:
			Door::Create(currentEdgeWallVertices, vertices, edges, edge);
			break;
		case EdgeType::Window:
			Window::Create(currentEdgeWallVertices, vertices, edges, edge);
			break;
		case EdgeType::SpiralStairs:
			break;
		case EdgeType::StandardStairs:
			break;
		}
		currentEdge++;
	}
	
	std::vector<int> externalVertices, externalCorners;
	std::vector<vec> externalCornerPositions;
	for (unsigned int i = 0; i < floorCount; i++)
	{
		externalVertices.clear();
		externalCorners.clear();
		Utils::getExternal(vertices, edges, externalVertices, externalCorners, i);

		externalCornerPositions.clear();
		for (int c : externalCorners)
			externalCornerPositions.push_back(vertices[c].pos);
		Floor::Create(externalCornerPositions, i > 0);
	}
	Roof::Create(externalCornerPositions);

	/*
	std::vector<int> externalVertices, externalCorners;
	Utils::getExternal(vertices, edges, externalVertices, externalCorners);

	std::vector<vec> externalCornerPositions;
	for (int c : externalCorners)
		externalCornerPositions.push_back(vertices[c].pos);

	Floor::Create(externalCornerPositions);
	Roof::Create(externalCornerPositions);*/
}