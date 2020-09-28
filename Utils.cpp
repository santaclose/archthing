#include "Utils.hpp"
#include "Geometry.h"

#include <iostream>

namespace Utils {

    int mod(int number, int mod)
    {
        if (number < 0)
            number += mod * (-number / mod + 1);
        return number % mod;
    }

    float maxDistanceBetweenVertices(const std::vector<gv>& vertices)
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

    void getAdjacentsSortedByAngle(
        const Wireframe& wf,
        int i,
        std::vector<std::pair<int, float>>& vertexAngle
    )
    {
        for (int conn : wf.vertices[i].conn)
        {
            int adjVertex = wf.edges[conn].a == i ? wf.edges[conn].b : wf.edges[conn].a;
            vertexAngle.emplace_back();
            vertexAngle.back().first = adjVertex;
            vertexAngle.back().second = atan2((wf.vertices[adjVertex].pos.z - wf.vertices[i].pos.z), (wf.vertices[adjVertex].pos.x - wf.vertices[i].pos.x));
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

    void getEdgesSortedByAngle(
        const Wireframe& wf,
        int i,
        std::vector<std::pair<int, float>>& connectionAngle
    )
    {
        for (int conn : wf.vertices[i].conn)
        {
            int adjVertex = wf.edges[conn].a == i ? wf.edges[conn].b : wf.edges[conn].a;
            connectionAngle.emplace_back();
            connectionAngle.back().first = conn;
            connectionAngle.back().second = atan2((wf.vertices[adjVertex].pos.z - wf.vertices[i].pos.z), (wf.vertices[adjVertex].pos.x - wf.vertices[i].pos.x));
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

    bool castRayFromCorner(const std::vector<gv>& vertices, const std::vector<ge>& edges, float rayDistance, int i, int a, int b)
    {
        vec wallDirA = ((vertices[a].pos - vertices[i].pos) * vec::up).Normalized();
        vec wallDirB = (vec::up * (vertices[b].pos - vertices[i].pos)).Normalized();

        vec rayDir = (wallDirA + wallDirB).Normalized();

        for (const ge& edge : edges)
        {
            // dont consider connected edges
            if (edge.a == i || edge.b == i)
                continue;
            // dont consider edges in other floors
            if (std::abs(vertices[edge.a].pos.y - vertices[i].pos.y) > 0.001f)
                continue;

            if (Geometry::intersect(vertices[edge.a].pos, vertices[edge.b].pos, vertices[i].pos, vertices[i].pos + rayDir * rayDistance))
                return true;
        }
        return false;
    }

    std::pair<int, int> findFirstExternalVertices(
        const Wireframe& wf,
        float rayDistance,
        unsigned int floor,
        float wallHeight)
    {
        for (int i = 0; i < wf.vertices.size(); i++)
        {
            if (std::abs(wf.vertices[i].pos.y - wallHeight * floor) > 0.001f) // vertex not in the requested floor
                continue;

            std::vector<std::pair<int, float>> vertexAngle;
            getAdjacentsSortedByAngle(wf, i, vertexAngle);

            for (int a = 0; a < vertexAngle.size(); a++)
            {
                int b = (a + 1) % vertexAngle.size();

                if (!castRayFromCorner(wf.vertices, wf.edges, rayDistance, i, vertexAngle[a].first, vertexAngle[b].first))
                    return { i, vertexAngle[b].first };
            }
        }
        // should never happen
        return { -1, -1 };
    }

    void getExternal(
        const Wireframe& wf,
        std::vector<int>& outVertices,
        std::vector<int>& outCorners,
        unsigned int floor,
        float wallHeight)
    {
        float rayDistance = maxDistanceBetweenVertices(wf.vertices) + 1.0f;
        auto firstVertices = findFirstExternalVertices(wf, rayDistance, floor, wallHeight);
        outVertices.insert(outVertices.begin(), firstVertices.first);
        outVertices.insert(outVertices.begin(), firstVertices.second);

        while (true)
        {
            std::vector<std::pair<int, float>> vertexAngle;
            getAdjacentsSortedByAngle(wf, firstVertices.second, vertexAngle);

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
            if (Geometry::isPointInLineSegment(
                wf.vertices[outCorners[(i) % outCorners.size()]].pos,
                wf.vertices[outCorners[(i + 2) % outCorners.size()]].pos,
                wf.vertices[outCorners[(i + 1) % outCorners.size()]].pos
            ))
            {
                outCorners.erase(outCorners.begin() + ((i + 1) % outCorners.size()));
                goto lal;
            }
        }

    }

    void sortVertexConnectionsByAngle(Wireframe& wf)
    {
        for (int i = 0; i < wf.vertices.size(); i++)
        {
            std::vector<std::pair<int, float>> edgeAngle;
            getEdgesSortedByAngle(wf, i, edgeAngle);
            wf.vertices[i].conn.clear();
            for (std::pair<int, float>& p : edgeAngle)
                wf.vertices[i].conn.insert(wf.vertices[i].conn.begin(), p.first);
        }
    }

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

            vec res = Geometry::intersectLinesPointDir(posi, posn, diri, dirn);
            //std::cout << "res: " << res.x << ", " << res.y << ", " << res.z << std::endl;
            connectionPoints.push_back(res);
        }
    }

    void getIntermediatePieces(
        const std::vector<vec>& below,
        const std::vector<vec>& above,
        std::vector<std::vector<vec>>& roofPieces,
        std::vector<std::vector<vec>>& ceilingPieces
        )
    {
        // find first common vertex
        unsigned int fcva = 0;
        unsigned int fcvb;
        for (; fcva < above.size(); fcva++)
        {
            fcvb = 0;
            for (; fcvb < below.size(); fcvb++)
            {
                if (Geometry::areVectorsEqual2D(below[fcvb], above[fcva]))
                {
                    std::cout << "found first common vertex: " << above[fcva].x << ", " << above[fcva].z << std::endl;
                    goto tag;
                }
            }
        }
        // no common vertices
        return;

    tag:
        // create above and below polygons with intersection vertices
        std::vector<vec> aboveWithIntersections;
        std::vector<vec> belowWithIntersections;
        for (int i = 0; i < above.size(); i++)
        {
            int ca = mod(fcva + i, above.size());
            int na = mod(fcva + i + 1, above.size());

            aboveWithIntersections.push_back(above[ca]);
            aboveWithIntersections.back().y = below[0].y;

            for (int j = 0; j < below.size(); j++)
            {
                int cb = mod(fcvb + j, below.size());

                if (!Geometry::areVectorsEqual2D(above[ca], below[cb]) &&
                    !Geometry::areVectorsEqual2D(above[na], below[cb]) &&
                    Geometry::isPointInLineSegment2D(above[ca], above[na], below[cb]))
                {
                    aboveWithIntersections.push_back(below[cb]);
                }
            }
        }
        for (int i = 0; i < below.size(); i++)
        {
            int cb = mod(fcvb + i, below.size());
            int nb = mod(fcvb + i + 1, below.size());

            belowWithIntersections.push_back(below[cb]);

            for (int j = 0; j < above.size(); j++)
            {
                int ca = mod(fcva + j, above.size());

                if (!Geometry::areVectorsEqual2D(below[cb], above[ca]) &&
                    !Geometry::areVectorsEqual2D(below[nb], above[ca]) &&
                    Geometry::isPointInLineSegment2D(below[cb], below[nb], above[ca]))
                {
                    belowWithIntersections.push_back(above[ca]);
                    belowWithIntersections.back().y = below[0].y;
                }
            }
        }

        // get loose parts
        for (int i = 0, j = 0;
            i < aboveWithIntersections.size() && j < belowWithIntersections.size();
            i++, j++)
        {
            if (!Geometry::areVectorsEqual2D(aboveWithIntersections[i], belowWithIntersections[j]))
            {
                if (!Geometry::isPointInsidePolygon2D(aboveWithIntersections, belowWithIntersections[j], true)) // below outside
                {
                    roofPieces.emplace_back();
                    roofPieces.back().push_back(aboveWithIntersections[i - 1]);

                    int p = 0, q = 0;
                    for (; p < aboveWithIntersections.size(); p++)
                    {
                        q = 0;
                        for (; q < belowWithIntersections.size(); q++)
                        {
                            if (Geometry::areVectorsEqual2D(
                                aboveWithIntersections[mod(i + p, aboveWithIntersections.size())],
                                belowWithIntersections[mod(j + q, belowWithIntersections.size())])
                                )
                                goto baOut;
                        }
                        roofPieces.back().insert(roofPieces.back().begin(), aboveWithIntersections[i + p]);
                    }
                baOut:
                    i = mod(i + p, aboveWithIntersections.size());
                    int t = j + q;
                    while (j < t)
                    {
                        roofPieces.back().push_back(belowWithIntersections[mod(j, belowWithIntersections.size())]);
                        j++;
                    }
                    roofPieces.back().push_back(belowWithIntersections[mod(j, belowWithIntersections.size())]);
                    std::cout << "below outside\n";
                }
                else if (Geometry::isPointInsidePolygon2D(aboveWithIntersections, belowWithIntersections[j], false)) // below inside
                {
                    ceilingPieces.emplace_back();
                    ceilingPieces.back().push_back(aboveWithIntersections[i - 1]);

                    int p = 0, q = 0;
                    for (; p < aboveWithIntersections.size(); p++)
                    {
                        q = 0;
                        for (; q < belowWithIntersections.size(); q++)
                        {
                            if (Geometry::areVectorsEqual2D(
                                aboveWithIntersections[mod(i + p, aboveWithIntersections.size())],
                                belowWithIntersections[mod(j + q, belowWithIntersections.size())])
                                )
                                goto biOut;
                        }
                        ceilingPieces.back().push_back(aboveWithIntersections[i + p]);
                    }
                biOut:
                    i = mod(i + p, aboveWithIntersections.size());
                    int t = j + q;
                    while (j < t)
                    {
                        ceilingPieces.back().insert(ceilingPieces.back().begin(), belowWithIntersections[mod(j, belowWithIntersections.size())]);
                        j++;
                    }
                    ceilingPieces.back().insert(ceilingPieces.back().begin(), belowWithIntersections[mod(j, belowWithIntersections.size())]);
                    std::cout << "below inside\n";
                }
                else if (!Geometry::isPointInsidePolygon2D(belowWithIntersections, aboveWithIntersections[i], true)) // above outside
                {
                    ceilingPieces.emplace_back();
                    ceilingPieces.back().push_back(belowWithIntersections[j - 1]);

                    int q = 0, p = 0;
                    for (; q < belowWithIntersections.size(); q++)
                    {
                        p = 0;
                        for (; p < aboveWithIntersections.size(); p++)
                        {
                            if (Geometry::areVectorsEqual2D(
                                belowWithIntersections[mod(j + q, belowWithIntersections.size())],
                                aboveWithIntersections[mod(i + p, aboveWithIntersections.size())])
                                )
                                goto aoOut;
                        }
                        ceilingPieces.back().insert(ceilingPieces.back().begin(), belowWithIntersections[j + q]);
                    }
                aoOut:
                    j = mod(j + q, belowWithIntersections.size());
                    int t = i + p;
                    while (i < t)
                    {
                        ceilingPieces.back().push_back(aboveWithIntersections[mod(i, aboveWithIntersections.size())]);
                        i++;
                    }
                    ceilingPieces.back().push_back(aboveWithIntersections[mod(i, aboveWithIntersections.size())]);
                    std::cout << "above outside\n";
                }
                else if (Geometry::isPointInsidePolygon2D(belowWithIntersections, aboveWithIntersections[i], false)) // above inside
                {
                    roofPieces.emplace_back();
                    roofPieces.back().push_back(belowWithIntersections[j - 1]);

                    int q = 0, p = 0;
                    for (; q < belowWithIntersections.size(); q++)
                    {
                        p = 0;
                        for (; p < aboveWithIntersections.size(); p++)
                        {
                            if (Geometry::areVectorsEqual2D(
                                belowWithIntersections[mod(j + q, belowWithIntersections.size())],
                                aboveWithIntersections[mod(i + p, aboveWithIntersections.size())])
                                )
                                goto aiOut;
                        }
                        roofPieces.back().push_back(belowWithIntersections[j + q]);
                    }
                aiOut:
                    j = mod(j + q, belowWithIntersections.size());
                    int t = i + p;
                    while (i < t)
                    {
                        roofPieces.back().insert(roofPieces.back().begin(), aboveWithIntersections[mod(i, aboveWithIntersections.size())]);
                        i++;
                    }
                    roofPieces.back().insert(roofPieces.back().begin(), aboveWithIntersections[mod(i, aboveWithIntersections.size())]);
                    std::cout << "above inside\n";
                }
            }
        }
    }
}