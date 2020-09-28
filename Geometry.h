#pragma once
#include <modelTool/vl.h>
#include <cstdlib>
#include <vector>

namespace Geometry
{
    bool ccw(const vec& A, const vec& B, const vec& C);
    bool intersect(const vec& l0a, const vec& l0b, const vec& l1a, const vec& l1b);
    bool intersectIgnoreParallel(const vec& l0a, const vec& l0b, const vec& l1a, const vec& l1b, float threshold = 0.99f);
    bool isPointInLineSegment(vec lineA, vec lineB, vec point, float threshold = 0.01f);
    bool isPointInLineSegment2D(vec lineA, vec lineB, vec point, float threshold = 0.01f);
    float cross2d(const vec& a, const vec& b);
    float dot2d(const vec& a, const vec& b);
    vec intersectLinesPointDir(const vec& pointA, const vec& pointB, const vec& dirA, const vec& dirB, float threshold = 0.99f);
    bool isPointInsideTriangle(const vec& p, const vec& ta, const vec& tb, const vec& tc);
    bool isPointInsideTriangle2D(const vec& p, const vec& ta, const vec& tb, const vec& tc);
    bool isPointInsidePolygon2D(std::vector<vec> polygon, const vec& point, bool considerEdgesAsInside = false);
    bool areVectorsEqual(vec a, vec b, float threshold = 0.001f);
    bool areVectorsEqual2D(vec a, vec b, float threshold = 0.001f);
}