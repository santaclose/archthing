#include <algorithm>

namespace Utils {

    bool ccw(const vec& A, const vec& B, const vec& C)
    {
        return (C.z - A.z) * (B.x - A.x) > (B.z - A.z) * (C.x - A.x);
    }
    // Return true if line segments AB and CD intersect
    bool intersect(const vec& A, const vec& B, const vec& C, const vec& D)
    {
        return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
    }
    inline bool isPointInLineSegment(vec lineA, vec lineB, vec point, float threshold = 0.01f)
    {
        return std::abs((lineA - point).Magnitude() + (lineB - point).Magnitude() - (lineA - lineB).Magnitude()) < threshold;
    }

    template <typename T>
    void addWithoutRepeating(std::vector<T>& list, T item)
    {
        for (const T& i : list)
        {
            if (i == item)
                return;
        }
        list.push_back(item);
    }

    inline float cross2d(const vec& a, const vec& b)
    {
        return a.x * b.z - a.z * b.x;
    }
    inline float dot2d(const vec& a, const vec& b)
    {
        return a.x * b.x + a.z * b.z;
    }
    vec intersectLines(const vec& pointA, const vec& pointB, const vec& dirA, const vec& dirB)
    {
        if (abs(dot2d(dirA.Normalized(), dirB.Normalized())) > 0.95)
        {
            return (pointA + pointB) * 0.5;
        }

    #define p pointA
    #define q pointB
    #define r dirA
    #define s dirB
        vec qp = q - p;
        float rs = cross2d(r, s);

        vec result;
        if (rs != 0.0 && cross2d(qp, r) != 0.0)
        {
            float t = cross2d(qp, s) / rs;
            float u = cross2d(qp, r) / rs;
            result = (p + (r * t) + q + (s * u)) * 0.5;
        }
        else
        {
            result = (p + q) * 0.5;
        }
        return result;
    #undef p
    #undef q
    #undef r
    #undef s
    }

    inline float maxDistanceBetweenVertices(const std::vector<gv>& vertices)
    {
        float maxDistance = 0.0f;
        for (const gv& i : vertices)
        {
            for (const gv& j : vertices)
            {
                float mag = (i.pos - j.pos).Magnitude();
                if (mag > maxDistance)
                    maxDistance = mag;
            }
        }
        return maxDistance;
    }

    inline void getAdjacentsSortedByAngle(
        const std::vector<gv>& vertices,
        const std::vector<ge>& edges,
        int i,
        std::vector<std::pair<int, float>>& vertexAngle
        )
    {
        for (int conn : vertices[i].conn)
        {
            int adjVertex = edges[conn].a == i ? edges[conn].b : edges[conn].a;
            vertexAngle.emplace_back();
            vertexAngle.back().first = adjVertex;
            vertexAngle.back().second = atan2((vertices[adjVertex].pos.z - vertices[i].pos.z), (vertices[adjVertex].pos.x - vertices[i].pos.x));
        }

        // sort pair vector
        std::sort(
            vertexAngle.begin(),
            vertexAngle.end(),
            [](auto& left, auto& right)
            {
                return left.second < right.second;
            });
    }

    inline void getEdgesSortedByAngle(
        const std::vector<gv>& vertices,
        const std::vector<ge>& edges,
        int i,
        std::vector<std::pair<int, float>>& connectionAngle
    )
    {
        for (int conn : vertices[i].conn)
        {
            int adjVertex = edges[conn].a == i ? edges[conn].b : edges[conn].a;
            connectionAngle.emplace_back();
            connectionAngle.back().first = conn;
            connectionAngle.back().second = atan2((vertices[adjVertex].pos.z - vertices[i].pos.z), (vertices[adjVertex].pos.x - vertices[i].pos.x));
        }

        // sort pair vector
        std::sort(
            connectionAngle.begin(),
            connectionAngle.end(),
            [](auto& left, auto& right)
            {
                return left.second < right.second;
            });
    }
    
    inline bool castRayFromCorner(const std::vector<gv>& vertices, const std::vector<ge>& edges, float rayDistance, int i, int a, int b)
    {
        vec wallDirA = ((vertices[a].pos - vertices[i].pos) * vec::up).Normalized();
        vec wallDirB = (vec::up * (vertices[b].pos - vertices[i].pos)).Normalized();

        vec rayDir = (wallDirA + wallDirB).Normalized();

        for (const ge& edge : edges)
        {
            // dont consider connected edges
            if (edge.a == i || edge.b == i)
                continue;

            if (intersect(vertices[edge.a].pos, vertices[edge.b].pos, vertices[i].pos, vertices[i].pos + rayDir * rayDistance))
                return true;
        }
        return false;
    }

    std::pair<int,int> findFirstExternalVertices(const std::vector<gv>& vertices, const std::vector<ge>& edges, float rayDistance, unsigned int floor)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            std::vector<std::pair<int, float>> vertexAngle;
            getAdjacentsSortedByAngle(vertices, edges, i, vertexAngle);

            for (int a = 0; a < vertexAngle.size(); a++)
            {
                int b = (a + 1) % vertexAngle.size();

                if (!castRayFromCorner(vertices, edges, rayDistance, i, vertexAngle[a].first, vertexAngle[b].first) && vertices[i].pos.y == wallHeight * floor)
                    return { i, vertexAngle[b].first };
            }
        }
        // should never happen
        return { -1, -1 };
    }

    void getExternal(
        const std::vector<gv>& vertices,
        const std::vector<ge>& edges,
        std::vector<int>& outVertices,
        std::vector<int>& outCorners,
        unsigned int floor)
    {
        float rayDistance = maxDistanceBetweenVertices(vertices) + 1.0f;
        auto firstVertices = findFirstExternalVertices(vertices, edges, rayDistance, floor);
        outVertices.insert(outVertices.begin(), firstVertices.first);
        outVertices.insert(outVertices.begin(), firstVertices.second);

        while (true)
        {
            std::vector<std::pair<int, float>> vertexAngle;
            getAdjacentsSortedByAngle(vertices, edges, firstVertices.second, vertexAngle);

            for (int a = 0; a < vertexAngle.size(); a++)
            {
                if (vertexAngle[a].first == firstVertices.first)
                {
                    int b = (a + 1) % vertexAngle.size();
                    if (vertexAngle[b].first == outVertices.back())
                        goto tag;
                    outVertices.insert(outVertices.begin(), vertexAngle[b].first);
                    firstVertices.first = firstVertices.second;
                    firstVertices.second = vertexAngle[b].first;
                    break;
                }
            }
        }

    tag:
        // get corners
        outCorners = outVertices;
    lal:
        for (unsigned int i = 0; i < outCorners.size(); i++)
        {
            if (isPointInLineSegment(
                vertices[outCorners[(i) % outCorners.size()]].pos,
                vertices[outCorners[(i + 2) % outCorners.size()]].pos,
                vertices[outCorners[(i + 1) % outCorners.size()]].pos
            ))
            {
                outCorners.erase(outCorners.begin() + ((i + 1) % outCorners.size()));
                goto lal;
            }
        }

    }

    /*void getExternal(
        const std::vector<gv>& vertices,
        const std::vector<ge>& edges,
        std::vector<int>& outVertices,
        std::vector<int>& outEdges,
        std::vector<int>& outCorners)
    {
        // get max distance between vertices
        float maxDistance = 0.0f;
        for (const gv& i : vertices)
        {
            for (const gv& j : vertices)
            {
                float mag = (i.pos - j.pos).Magnitude();
                if (mag > maxDistance)
                    maxDistance = mag;
            }
        }
        // add one to make sure they are going to intersect
        maxDistance += 1.0f;

        int currentEdge;
        l:
        currentEdge = 0;
        for (const ge& i : edges)
        {
            if (std::find(outEdges.begin(), outEdges.end(), currentEdge) != outEdges.end())
            {
                currentEdge++;
                continue;
            }

            vec mid = (vertices[i.a].pos + vertices[i.b].pos) * 0.5f;
            vec midRight = mid + (((vertices[i.b].pos - vertices[i.a].pos) * vec::up).Normalized() * maxDistance);
            vec midLeft = mid + ((vec::up * (vertices[i.b].pos - vertices[i.a].pos)).Normalized() * maxDistance);

            bool somethingToTheRight = false;
            bool somethingToTheLeft = false;
            for (const ge& j : edges)
            {
                if (&j == &i)
                    continue;

                somethingToTheRight |= intersect(mid, midRight, vertices[j.a].pos, vertices[j.b].pos);
                somethingToTheLeft |= intersect(mid, midLeft, vertices[j.a].pos, vertices[j.b].pos);
            }

            if (!somethingToTheRight || !somethingToTheLeft) // is external wall
            {
                if (outVertices.size() == 0)
                {
                    outEdges.push_back(currentEdge);
                    // make sure we go ccw
                    if (!somethingToTheRight)
                    {
                        outVertices.push_back(i.a);
                        outVertices.push_back(i.b);
                    }
                    else
                    {
                        outVertices.push_back(i.b);
                        outVertices.push_back(i.a);
                    }
                    goto l;
                }
                else
                {
                    if (vertices[i.a].pos == vertices[outVertices.back()].pos)
                    {
                        outEdges.push_back(currentEdge);
                        outVertices.push_back(i.b);
                        goto l;
                    }
                    else if (vertices[i.b].pos == vertices[outVertices.back()].pos)
                    {
                        outEdges.push_back(currentEdge);
                        outVertices.push_back(i.a);
                        goto l;
                    }
                }
            }
            currentEdge++;
        }

        // get corners
        outCorners = outVertices;
        tag:
        for (unsigned int i = 0; i < outCorners.size(); i++)
        {
            if (isPointInLineSegment(
                vertices[outCorners[(i) % outCorners.size()]].pos,
                vertices[outCorners[(i + 2) % outCorners.size()]].pos,
                vertices[outCorners[(i + 1) % outCorners.size()]].pos
            ))
            {
                outCorners.erase(outCorners.begin() + ((i + 1) % outCorners.size()));
                goto tag;
            }
        }
    }*/

    void sortVertexConnectionsByAngle(
        std::vector<gv>& vertices,
        const std::vector<ge>& edges)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            std::vector<std::pair<int, float>> edgeAngle;
            getEdgesSortedByAngle(vertices, edges, i, edgeAngle);
            vertices[i].conn.clear();
            for (std::pair<int, float>& p : edgeAngle)
                vertices[i].conn.insert(vertices[i].conn.begin(), p.first);
        }
    }

    //void getConnectionPointsForVertex(
    //    const std::vector<gv>& vertices,
    //    const std::vector<ge>& edges,
    //    int vertexIndex,
    //    float thickness,
    //    std::vector<std::pair<int, float>>& vertexAngle,
    //    std::vector<vec>& connectionPoints
    //    )
    //{
    //    getAdjacentsSortedByAngle(vertices, edges, vertexIndex, vertexAngle);
    //    for (int i = 0; i < vertexAngle.size(); i++)
    //    {
    //        int n = (i + 1) % vertexAngle.size();

    //        vec diri = vertices[vertexAngle[i].first].pos - vertices[vertexIndex].pos;
    //        vec dirn = vertices[vertexAngle[n].first].pos - vertices[vertexIndex].pos;
    //        diri.y = dirn.y = 0.0f;
    //        diri.Normalize();
    //        dirn.Normalize();

    //        vec posi = vertices[vertexIndex].pos - (diri * vec::up).Normalized() * (thickness * 0.5f);
    //        vec posn = vertices[vertexIndex].pos + (dirn * vec::up).Normalized() * (thickness * 0.5f);

    //        connectionPoints.push_back(intersectLines(posi, posn, diri, dirn));
    //    }
    //}

    // adj must be already sorted by angle
    void getConnectionPoints(const vec& point, const std::vector<vec>& adj, float thickness, std::vector<vec>& connectionPoints)
    {
        for (int i = 0; i < adj.size(); i++)
        {
            int n = (i + 1) % adj.size();

            vec diri = adj[i] - point;
            vec dirn = adj[n] - point;
            diri.y = dirn.y = 0.0f;
            diri.Normalize();
            dirn.Normalize();

            vec posi = point - (diri * vec::up).Normalized() * (thickness * 0.5f);
            vec posn = point + (dirn * vec::up).Normalized() * (thickness * 0.5f);

            vec res = intersectLines(posi, posn, diri, dirn);
            //std::cout << "res: " << res.x << ", " << res.y << ", " << res.z << std::endl;
            connectionPoints.push_back(res);
        }
    }

    void readOBJ(std::vector<gv>& vertices, std::vector<ge>& edges, unsigned int& floorCount, const std::string& filePath)
    {
        EdgeType currentEdgeType = EdgeType::Wall;
        unsigned int currentFloor = 0;
        floorCount = 1;

        std::ifstream is(filePath);
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

                vertices.emplace_back();
                vertices.back().pos.x = std::stof(str.substr(aq, ap - aq)) * inputScale;
                vertices.back().pos.y = currentFloor * wallHeight;//std::stof(str.substr(bq, bp - bq)) * inputScale;
                vertices.back().pos.z = std::stof(str.substr(cq, cp - cq)) * inputScale;
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

                edges.emplace_back();
                edges.back().a = std::stoi(str.substr(aq, ap - aq)) - 1;
                edges.back().b = std::stoi(str.substr(bq, bp - bq)) - 1;
                edges.back().type = currentEdgeType;
                vertices[edges.back().a].conn.push_back(edges.size() - 1);
                vertices[edges.back().b].conn.push_back(edges.size() - 1);
            }
        }
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
}