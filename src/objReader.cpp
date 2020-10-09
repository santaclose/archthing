#pragma once
#include "objReader.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace OBJReader {

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
            // see if vertices vector contains ov
            int index = 0;
            for (const gv& v : vertices)
            {
                if (v.pos == ov.pos)
                    break;
                index++;
            }

            if (index == vertices.size()) // not added yet
            {
                vertices.emplace_back();
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

        std::cout << "removed " << originalVertices.size() - vertices.size() << " vertices\n";
    }

    void Read(Wireframe& wf, unsigned int& floorCount, const std::string& filePath, float wallHeight, float inputScale)
    {
        EdgeType currentEdgeType = EdgeType::Wall;
        unsigned int currentFloor = 0;
        floorCount = 1;

        std::ifstream is(filePath);
        if (is.fail())
            std::cout << "error opening file " << filePath << std::endl;

        std::string str;
        while (std::getline(is, str))
        {
            if (str.length() == 0)
                continue;

            if (str[0] == 'o')
            {
                if (str.find("wall") != std::string::npos)
                {
                    currentEdgeType = EdgeType::Wall;
                    currentFloor = str.length() > 6 ? std::stoi(str.substr(7, 3)) : 0;
                }
                else if (str.find("door") != std::string::npos)
                {
                    currentEdgeType = EdgeType::Door;
                    currentFloor = str.length() > 6 ? std::stoi(str.substr(7, 3)) : 0;
                }
                else if (str.find("window") != std::string::npos)
                {
                    currentEdgeType = EdgeType::Window;
                    currentFloor = str.length() > 8 ? std::stoi(str.substr(9, 3)) : 0;
                }
                else if (str.find("spiralStairs") != std::string::npos)
                    currentEdgeType = EdgeType::SpiralStairs;
                else
                    currentEdgeType = EdgeType::StandardStairs;

                floorCount = currentFloor + 1 > floorCount ? currentFloor + 1 : floorCount;
            }
            else if (str[0] == 'v')
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
                wf.vertices.back().pos.x = std::stof(str.substr(aq, ap - aq)) * inputScale;
                wf.vertices.back().pos.y = currentFloor * wallHeight;//std::stof(str.substr(bq, bp - bq)) * inputScale;
                wf.vertices.back().pos.z = std::stof(str.substr(cq, cp - cq)) * inputScale;
            }
            else if (str[0] == 'l')
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
    }
}