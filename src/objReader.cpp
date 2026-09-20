#include "objReader.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <cassert>

namespace OBJReader {

	inline bool charIsUint(char lal)
	{
		return lal >= '0' && lal <= '9';
	}
	inline bool charIsFloat(char lal)
	{
		return (lal >= '0' && lal <= '9') || lal == '.';
	}

	unsigned int floorNumberFromObjectName(const std::string& inString)
	{
		unsigned int first, last;
		for (first = 1; first < inString.length() && (inString[first] != '=' || inString[first - 1] != 'f'); first++);
		if (first == inString.length()) // not found
			return 0;
		for (last = first; last < inString.length() && charIsUint(inString[first]); last++);
		return std::stoi(inString.substr(first, last - first));
	}

	float widthFromObjectName(const std::string& inString)
	{
		unsigned int first, last;
		for (first = 1; first < inString.length() && (inString[first] != '=' || inString[first - 1] != 'w'); first++);
		if (first == inString.length()) // not found
			return 0;
		for (last = first; last < inString.length() && charIsUint(inString[first]); last++);
		return std::stoi(inString.substr(first, last - first));
	}

	void removeDuplicated(std::vector<gv>& vertices, std::vector<ge>& edges)
	{
		std::vector<gv> originalVertices = vertices;
		std::vector<ge> originalEdges = edges;
		vertices.clear();
		edges.clear();

		std::unordered_map<int, int> vertexMap; // maps from original vertices to new ones

		int i = 0;
		for (const gv& ov : originalVertices)
		{
			bool isWallKindVertex = true;
			for (int c : ov.conn)
			{
				if (!IsEdgeWallKind(originalEdges[c].type))
				{
					isWallKindVertex = false;
					break;
				}
			}

			// see if vertices vector contains ov
			int index = 0;
			if (!isWallKindVertex)
				index = vertices.size();
			else
			{
				for (const gv& v : vertices)
				{
					if (v.pos == ov.pos)
						break;
					index++;
				}
			}

			if (!isWallKindVertex || index == vertices.size()) // not added yet
			{
				vertices.emplace_back();
				vertices.back().id = vertices.size() - 1;
				vertices.back().pos = ov.pos; // do not connect yet
			}

			vertexMap[i] = index;
			i++;
		}

		for (const ge& oe : originalEdges)
		{
			ge newEdge;
			newEdge.type = oe.type;
			newEdge.a = vertexMap[oe.a];
			newEdge.b = vertexMap[oe.b];
			edges.push_back(newEdge);
			vertices[newEdge.a].conn.push_back(edges.size() - 1);
			vertices[newEdge.b].conn.push_back(edges.size() - 1);
		}

		std::cout << "    -- " << originalVertices.size() - vertices.size() << " vertices removed\n";
	}

	bool Read(Wireframe& wf, unsigned int& floorCount, const std::string& filePath, float wallHeight, float inputScale)
	{
		EdgeType currentEdgeType = EdgeType::Wall;
		unsigned int currentFloor = 0;
		floorCount = 1;

		std::ifstream is(filePath);
		if (is.fail())
		{
			std::cout << "    -- could not read file " << filePath << std::endl;
			return false;
		}

		std::string str;
		while (std::getline(is, str))
		{
			if (str.length() == 0)
				continue;

			if (str[0] == 'o' && str[1] == ' ')
			{
				currentFloor = floorNumberFromObjectName(str);

				if (str.find("wall") != std::string::npos)
					currentEdgeType = EdgeType::Wall;
				else if (str.find("door") != std::string::npos)
					currentEdgeType = EdgeType::Door;
				else if (str.find("window") != std::string::npos)
					currentEdgeType = EdgeType::Window;
				else if (str.find("hole") != std::string::npos)
					currentEdgeType = EdgeType::Hole;
				else if (str.find("spiralStairs") != std::string::npos)
					currentEdgeType = EdgeType::SpiralStairs;
				else if (str.find("stairs") != std::string::npos)
					currentEdgeType = EdgeType::StandardStairs;
				else if (str.find("toilet") != std::string::npos)
					currentEdgeType = EdgeType::Toilet;
				else if (str.find("sink") != std::string::npos)
					currentEdgeType = EdgeType::Sink;
				else if (str.find("fridge") != std::string::npos)
					currentEdgeType = EdgeType::Fridge;
				else if (str.find("shower") != std::string::npos)
					currentEdgeType = EdgeType::Shower;
				else if (str.find("washingMachine") != std::string::npos)
					currentEdgeType = EdgeType::WashingMachine;
				else if (str.find("bed") != std::string::npos)
					currentEdgeType = EdgeType::Bed;
				else
				{
					printf("Unknown object type found in wireframe obj file: %s\n", str.c_str() + 2);
					assert(false);
				}

				floorCount = currentFloor + 1 > floorCount ? currentFloor + 1 : floorCount;
			}
			else if (str[0] == 'v' && str[1] == ' ')
			{
				int aq, ap, bq, bp, cq, cp;
				aq = 2;
				ap = aq;
				while (str[ap] != ' ')ap++;
				bq = ap + 1;
				bp = bq;
				while (str[bp] != ' ')bp++;
				cq = bp + 1;
				cp = cq;
				while (cp < str.length() && str[cp] != ' ' && str[cp] != '\n')cp++;

				wf.vertices.emplace_back();
				wf.vertices.back().id = wf.vertices.size() - 1;
				wf.vertices.back().pos.x = std::stof(str.substr(aq, ap - aq)) * inputScale;
				wf.vertices.back().pos.y = currentFloor * wallHeight;//std::stof(str.substr(bq, bp - bq)) * inputScale;
				wf.vertices.back().pos.z = std::stof(str.substr(cq, cp - cq)) * inputScale;
			}
			else if (str[0] == 'l' && str[1] == ' ')
			{
				int aq, ap, bq, bp;
				aq = 2;
				ap = aq;
				while (str[ap] != ' ')ap++;
				bq = ap + 1;
				bp = bq;
				while (bp < str.length() && str[bp] != ' ' && str[bp] != '\n')bp++;

				wf.edges.emplace_back();
				wf.edges.back().a = std::stoi(str.substr(aq, ap - aq)) - 1;
				wf.edges.back().b = std::stoi(str.substr(bq, bp - bq)) - 1;
				wf.edges.back().type = currentEdgeType;
				wf.vertices[wf.edges.back().a].conn.push_back(wf.edges.size() - 1);
				wf.vertices[wf.edges.back().b].conn.push_back(wf.edges.size() - 1);
			}
		}

		removeDuplicated(wf.vertices, wf.edges);
		return true;
	}
}