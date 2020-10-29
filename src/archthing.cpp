#include <Model.h>
#include <modelTool/ml.h>
#include "Primitives.h"

#include <fstream>
#include <iostream>
#include <string>
#include <imgui.h>

char fileName[256] = "assets/input/b.obj";
float inputScale = 1.0f;
float wallHeight = 4.26f;
float wallThickness = 0.0889f;
bool  createDoors = true;
float doorHeight = 2.1f;
float doorSeparation = 0.003;
float doorThickness = 0.025;
float doorHandlePosX = 0.056f;
float doorHandlePosY = 1.0f;
bool  createWindows = true;
float glassThickness = 0.003175f;
float windowOffset = 1.1f;
float windowHeight = 2.0f;
float windowFrameWidth = 0.05f;
float windowFrameThickness = 0.02f;
float roofThickness = 0.3f;
float roofBevel = 0.3f;
float roofDepth = 0.2f;
float floorThickness = 0.25f;
float stairsThickness = 0.32f;
float stairsWidth = 1.5f;
float idealStairStepHeight = 0.18f;

#include "Wireframe.h"
#include "OBJReader.h"
#include "Utils.h"
#include "Geometry.h"

#include "Wall.h"
#include "Door.h"
#include "Window.h"
#include "Stairs.h"
#include "Roof.h"
#include "Floor.h"

void Model::Bindings(bool& haveToGenerateModel)
{
	if (ImGui::InputText("File Name", fileName, 256)) haveToGenerateModel = true;
	BIND(SliderFloat, "Input Scale", &inputScale, 0.01f, 20.0f);
	BIND(SliderFloat, "Wall Height", &wallHeight, 1.0f, 20.0f);
	BIND(SliderFloat, "Wall Thickness", &wallThickness, 0.01f, 0.3f);
	BINDCHECKBOX(Checkbox, "Create Doors", &createDoors);
	BIND(SliderFloat, "Door Height", &doorHeight, 1.5f, 4.0f);
	BIND(SliderFloat, "Door Separation", &doorSeparation, 0.0f, 0.3f);
	BIND(SliderFloat, "Door Thickness", &doorThickness, 0.01f, 0.05f);
	BIND(SliderFloat, "Door Handle Pos X", &doorHandlePosX, 0.0f, 0.3f);
	BIND(SliderFloat, "Door Handle Pos Y", &doorHandlePosY, 0.0f, 1.5f);
	BINDCHECKBOX(Checkbox, "Create Windows", &createWindows);
	BIND(SliderFloat, "Glass Thickness", &glassThickness, 0.001f, 0.2f);
	BIND(SliderFloat, "Window Offset", &windowOffset, 0.5f, 2.0f);
	BIND(SliderFloat, "Window Height", &windowHeight, 0.5f, 3.0f);
	BIND(SliderFloat, "Window Frame Width", &windowFrameWidth, 0.01f, 0.1f);
	BIND(SliderFloat, "Window Frame Thickness", &windowFrameThickness, 0.01f, 0.3f);
	BIND(SliderFloat, "Roof Thickness", &roofThickness, 0.01f, 1.0f);
	BIND(SliderFloat, "Roof Bevel", &roofBevel, 0.01f, 1.5f);
	BIND(SliderFloat, "Roof Depth", &roofDepth, 0.01f, 0.6f);
	BIND(SliderFloat, "Floor Thickness", &floorThickness, 0.01f, 0.6f);
	BIND(SliderFloat, "Stairs Thickness", &stairsThickness, 0.1f, 1.0f);
	BIND(SliderFloat, "Ideal Stair Step Height", &idealStairStepHeight, 0.01f, 0.5f);
}

void createWallVertices(const Wireframe& wf, std::vector<std::vector<std::pair<unsigned int, unsigned int>>>& wallVertices)
{
	wallVertices.resize(wf.vertices.size());

	for (int i = 0; i < wf.vertices.size(); i++)
	{
		//std::cout << "iteration " << i << std::endl;

		std::vector<vec> adj;
		bool isStairsVertex = false;
		for (int c : wf.vertices[i].conn)
		{
			if (wf.edges[c].type >= EdgeType::StandardStairs)
			{
				isStairsVertex = true;
				break;
			}
			adj.push_back(wf.edges[c].a == i ? wf.vertices[wf.edges[c].b].pos : wf.vertices[wf.edges[c].a].pos);
		}

		if (!isStairsVertex)
		{
			if (adj.size() == 1)
			{
				{
					vec corner = wf.vertices[i].pos + (vec::up * (adj[0] - wf.vertices[i].pos)).Normalized() * (wallThickness * 0.5f);
					unsigned int base = ml::vertex(corner);
					unsigned int top = ml::vertex(corner + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Primitives::Cylinder(0.01f, 3, corner, corner + vec::up * wallHeight);
				}
				{
					vec corner = wf.vertices[i].pos + ((adj[0] - wf.vertices[i].pos) * vec::up).Normalized() * (wallThickness * 0.5f);
					unsigned int base = ml::vertex(corner);
					unsigned int top = ml::vertex(corner + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Primitives::Cylinder(0.01f, 3, corner, corner + vec::up * wallHeight);
				}
			}
			else
			{
				std::vector<vec> connectionPoints;
				Utils::getConnectionPoints(wf.vertices[i].pos, adj, wallThickness, connectionPoints);
				for (vec& cp : connectionPoints)
				{
					unsigned int base = ml::vertex(cp);
					unsigned int top = ml::vertex(cp + vec::up * wallHeight);
					wallVertices[i].push_back({ base, top });
					//Primitives::Cylinder(0.01f, 3, cp, cp + vec::up * wallHeight);
				}
			}
		}
	}
}

void Model::GenerateModel()
{
	std::cout << "-- reading obj file\n";
	unsigned int floorCount;
	Wireframe wf;
	if (!OBJReader::Read(wf, floorCount, fileName, wallHeight, inputScale))
		return;

	std::cout << "-- sorting vertex connections\n";
	Utils::sortVertexConnectionsByAngle(wf);

	std::cout << "-- creating wall vertices\n";
	std::vector<std::vector<std::pair<unsigned int, unsigned int>>> wallVertices;
	createWallVertices(wf, wallVertices);

	std::cout << "-- creating geometry\n";

	unsigned int currentEdge = 0;
	for (ge& edge : wf.edges)
	{
		vec displacement = wf.vertices[edge.b].pos - wf.vertices[edge.a].pos;

		// get wall vertices for edge
		unsigned int currentEdgeWallVertices[8];
		if (edge.type < EdgeType::StandardStairs)
		{
			// find vertex connection indices
			int i = std::find(wf.vertices[edge.a].conn.begin(), wf.vertices[edge.a].conn.end(), currentEdge) - wf.vertices[edge.a].conn.begin();
			int j = std::find(wf.vertices[edge.b].conn.begin(), wf.vertices[edge.b].conn.end(), currentEdge) - wf.vertices[edge.b].conn.begin();

			// get current edge wall vertices
			currentEdgeWallVertices[0] = wallVertices[edge.a][((i + 0 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].first;
			currentEdgeWallVertices[1] = wallVertices[edge.a][((i - 1 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].first;
			currentEdgeWallVertices[2] = wallVertices[edge.a][((i - 1 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].second;
			currentEdgeWallVertices[3] = wallVertices[edge.a][((i + 0 + wallVertices[edge.a].size()) % wallVertices[edge.a].size())].second;

			currentEdgeWallVertices[4] = wallVertices[edge.b][((j - 1 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].first;
			currentEdgeWallVertices[5] = wallVertices[edge.b][((j + 0 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].first;
			currentEdgeWallVertices[6] = wallVertices[edge.b][((j + 0 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].second;
			currentEdgeWallVertices[7] = wallVertices[edge.b][((j - 1 + wallVertices[edge.b].size()) % wallVertices[edge.b].size())].second;
		}

		// create appropriate geometry
		switch (edge.type)
		{
		case EdgeType::Wall:
			Wall::Create(currentEdgeWallVertices, wf.vertices, wf.edges, edge);
			break;
		case EdgeType::Door:
			Door::Create(currentEdgeWallVertices, wf.vertices, wf.edges, edge);
			break;
		case EdgeType::Window:
			Window::Create(currentEdgeWallVertices, wf.vertices, wf.edges, edge);
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
	std::vector<vec> belowExternalCornerPositions;
	for (unsigned int i = 0; i < floorCount; i++)
	{
		externalVertices.clear();
		externalCorners.clear();
		Utils::getExternal(wf, externalVertices, externalCorners, i, wallHeight);

		externalCornerPositions.clear();
		for (int c : externalCorners)
			externalCornerPositions.push_back(wf.vertices[c].pos);

		std::vector<std::vector<vec>> holes;
		Utils::getHoles(wf, holes, i, wallHeight);
		Floor::Create(externalCornerPositions, holes, i > 0, i == 0); // no ceiling below when i == 0

		std::vector<std::vector<vec>> stairs;
		Utils::getStairs(wf, stairs, i, wallHeight);
		Stairs::Create(stairs, stairsWidth);

		if (belowExternalCornerPositions.size() > 0) // if not first floor
		{
			std::vector<std::vector<vec>> intermediateRoofPieces;
			std::vector<std::vector<vec>> intermediateCeilingPieces;
			Utils::getIntermediatePieces( // this should say what sides are next to the external walls
				belowExternalCornerPositions,
				externalCornerPositions,
				intermediateRoofPieces,
				intermediateCeilingPieces);

			for (const std::vector<vec>& piece : intermediateRoofPieces)
				Roof::CreateIntermediate(piece);

			for (const std::vector<vec>& piece : intermediateCeilingPieces)
				Roof::CreateIntermediateCeil(piece);
		}
		belowExternalCornerPositions = externalCornerPositions;
	}
	Roof::Create(externalCornerPositions);
}