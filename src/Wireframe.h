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
	Bed
};

inline bool IsEdgeWallKind(EdgeType edgeType)
{
	return edgeType == EdgeType::Wall || edgeType == EdgeType::Door || edgeType == EdgeType::Window;
}

inline bool IsEdgeWidthKind(EdgeType edgeType)
{
	return edgeType == EdgeType::StandardStairs;
}

inline bool IsEdgeObjectKind(EdgeType edgeType)
{
	return edgeType >= EdgeType::Toilet;
}

inline std::string MarkerNameFromObjectEdge(EdgeType edgeType)
{
	switch (edgeType)
	{
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
		default:
			assert(false);
			return "";
	}
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
	void Print()
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
			printf("ge   type: %u   a: %u   b: %u\n", edge.type, edge.a, edge.b);
		}
	}
};

Wireframe GenerateProcedural(int floorCount, float wallHeight);