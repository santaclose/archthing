#pragma once
#include <modelTool/vl.h>
#include <vector>
#include <cstdio>
#include <string>
#include <cassert>

enum class EdgeType {
	Wall,
	Door,
	Window,
	Hole,
	StandardStairs,
	SpiralStairs,
	Toilet,
	Sink,
	Fridge,
	Shower,
	WashingMachine,
	Bed,
	Sofa,
	Desk,
	Dryer,
	TowelRack,
	Nightstand
};

inline bool IsEdgeWallKind(EdgeType edgeType)
{
	return edgeType == EdgeType::Wall || edgeType == EdgeType::Door || edgeType == EdgeType::Window;
}

inline bool IsEdgeWidthKind(EdgeType edgeType)
{
	return edgeType == EdgeType::StandardStairs;
}

inline bool IsEdgeMovableKind(EdgeType edgeType)
{
	return edgeType >= EdgeType::Toilet;
}

inline std::string EdgeTypeToString(EdgeType edgeType)
{
	switch (edgeType)
	{
		case EdgeType::Wall:
			return "wall";
		case EdgeType::Door:
			return "door";
		case EdgeType::Window:
			return "window";
		case EdgeType::Hole:
			return "hole";
		case EdgeType::StandardStairs:
			return "stairs";
		case EdgeType::SpiralStairs:
			return "spiralStairs";
		case EdgeType::Toilet:
			return "toilet";
		case EdgeType::Sink:
			return "sink";
		case EdgeType::Fridge:
			return "fridge";
		case EdgeType::Shower:
			return "shower";
		case EdgeType::WashingMachine:
			return "washingMachine";
		case EdgeType::Bed:
			return "bed";
		case EdgeType::Sofa:
			return "sofa";
		case EdgeType::Desk:
			return "desk";
		case EdgeType::Dryer:
			return "dryer";
		case EdgeType::TowelRack:
			return "towelRack";
		case EdgeType::Nightstand:
			return "nightStand";
		default:
			printf("Unknown edge type: %x\n", (unsigned int) edgeType);
			assert(false);
			return "";
	}
}

inline EdgeType EdgeTypeFromObjectName(const std::string& objName)
{
	if (objName.find("wall") != std::string::npos)
		return EdgeType::Wall;
	if (objName.find("door") != std::string::npos)
		return EdgeType::Door;
	if (objName.find("window") != std::string::npos)
		return EdgeType::Window;
	if (objName.find("hole") != std::string::npos)
		return EdgeType::Hole;
	if (objName.find("stairs") != std::string::npos)
		return EdgeType::StandardStairs;
	if (objName.find("spiralStairs") != std::string::npos)
		return EdgeType::SpiralStairs;
	if (objName.find("toilet") != std::string::npos)
		return EdgeType::Toilet;
	if (objName.find("sink") != std::string::npos)
		return EdgeType::Sink;
	if (objName.find("fridge") != std::string::npos)
		return EdgeType::Fridge;
	if (objName.find("shower") != std::string::npos)
		return EdgeType::Shower;
	if (objName.find("washingMachine") != std::string::npos)
		return EdgeType::WashingMachine;
	if (objName.find("bed") != std::string::npos)
		return EdgeType::Bed;
	if (objName.find("sofa") != std::string::npos)
		return EdgeType::Sofa;
	if (objName.find("desk") != std::string::npos)
		return EdgeType::Desk;
	if (objName.find("dryer") != std::string::npos)
		return EdgeType::Dryer;
	if (objName.find("towelRack") != std::string::npos)
		return EdgeType::TowelRack;
	if (objName.find("nightStand") != std::string::npos)
		return EdgeType::Nightstand;
	printf("Can't find edge type in object name: %s\n", objName.c_str());
	assert(false);
	return (EdgeType) -1;
}

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
	float width;
};
struct Wireframe
{
	std::vector<gv> vertices;
	std::vector<ge> edges;

	int AddVertex(const vec& pos)
	{
		int currentVertexId = vertices.size();
		gv baseVertex;
		vertices.push_back(baseVertex);
		vertices.back().id = currentVertexId;
		vertices.back().pos = pos;
		return currentVertexId;
	}
	void Connect(int a, int b, EdgeType type)
	{
		int currentEdgeId = edges.size();
		vertices[a].conn.push_back(currentEdgeId);
		vertices[b].conn.push_back(currentEdgeId);
		edges.push_back({type, a, b});
	}
	void ConnectLoop(const std::vector<int>& list, EdgeType type)
	{
		for (int i = 0; i < list.size(); i++)
		{
			Connect(list[i], list[(i + 1) % list.size()], type);
		}
	}
	void ConnectChain(const std::vector<int>& list, EdgeType type)
	{
		for (int i = 0; i < list.size() - 1; i++)
		{
			Connect(list[i], list[i + 1], type);
		}
	}
	void SetY(float newY)
	{
		for (gv& v : vertices)
			v.pos.y = newY;
	}
	void Append(const Wireframe& other)
	{
		int originalVertexCount = vertices.size();
		int originalEdgeCount = edges.size();
		int otherVertexCount = other.vertices.size();
		int otherEdgeCount = other.edges.size();

		vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
		edges.insert(edges.end(), other.edges.begin(), other.edges.end());

		for (int i = originalVertexCount; i < vertices.size(); i++)
		{
			gv& vtx = vertices[i];
			vtx.id += originalVertexCount;
			for (int& c : vtx.conn)
				c += originalEdgeCount;
		}

		for (int i = originalEdgeCount; i < edges.size(); i++)
		{
			ge& edge = edges[i];
			edge.a += originalVertexCount;
			edge.b += originalVertexCount;
		}
	}
	void Print() const
	{
		for (const gv& vtx : vertices)
		{
			printf("gv   id: %u   conn: (", vtx.id);
			for (int c : vtx.conn)
				printf("%d,", c);
			printf(")   pos: %f %f %f\n", vtx.pos.x, vtx.pos.y, vtx.pos.z);
		}
		for (const ge& edge : edges)
		{
			printf("ge   type: %u   a: %u   b: %u   width: %f\n", edge.type, edge.a, edge.b, edge.width);
		}
	}
};

Wireframe GenerateProcedural(int floorCount, float wallHeight);