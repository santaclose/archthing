#include "Geometry.h"

#include <vector>

namespace Geometry
{
    bool ccw(const vec& A, const vec& B, const vec& C)
    {
        return (C.z - A.z) * (B.x - A.x) > (B.z - A.z) * (C.x - A.x);
    }
    bool intersect(const vec& l0a, const vec& l0b, const vec& l1a, const vec& l1b)
    {
        return ccw(l0a, l1a, l1b) != ccw(l0b, l1a, l1b) && ccw(l0a, l0b, l1a) != ccw(l0a, l0b, l1b);
    }
    bool intersectIgnoreParallel(const vec& l0a, const vec& l0b, const vec& l1a, const vec& l1b, float threshold)
    {
        if (std::abs((l0b - l0a).Dot(l1a - l1b) > threshold))
            return false;
        return intersect(l0a, l0b, l1a, l1b);
    }
    bool isPointInLineSegment(vec lineA, vec lineB, vec point, float threshold)
    {
        return std::abs((lineA - point).Magnitude() + (lineB - point).Magnitude() - (lineA - lineB).Magnitude()) < threshold;
    }
    bool isPointInLineSegment2D(vec lineA, vec lineB, vec point, float threshold)
    {
        vec la = lineA, lb = lineB, p = point;
        la.y = lb.y = p.y = 0.0f;
        return std::abs((la - p).Magnitude() + (lb - p).Magnitude() - (la - lb).Magnitude()) < threshold;
    }

    float cross2d(const vec& a, const vec& b)
    {
        return a.x * b.z - a.z * b.x;
    }
    float dot2d(const vec& a, const vec& b)
    {
        return a.x * b.x + a.z * b.z;
    }
    vec intersectLinesPointDir(const vec& pointA, const vec& pointB, const vec& dirA, const vec& dirB, float threshold)
    {
        if (std::abs(dot2d(dirA.Normalized(), dirB.Normalized())) > threshold)
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

    bool isPointInsideTriangle(const vec& p, const vec& ta, const vec& tb, const vec& tc)
    {
        vec triangleNormal = (tb - ta) * (tc - ta);

        if (triangleNormal.Dot((tb - ta) * (p - ta)) < 0.0f)
            return false;
        if (triangleNormal.Dot((tc - tb) * (p - tb)) < 0.0f)
            return false;
        if (triangleNormal.Dot((ta - tc) * (p - tc)) < 0.0f)
            return false;
        return true;
    }
    bool isPointInsideTriangle2D(const vec& p, const vec& ta, const vec& tb, const vec& tc)
    {
        vec p2d = p, ta2d = ta, tb2d = tb, tc2d = tc;
        p2d.y = ta2d.y = tb2d.y = tc2d.y = 0.0f;
        return isPointInsideTriangle(p2d, ta2d, tb2d, tc2d);
    }

    bool isPointInsidePolygon2D(std::vector<vec> polygon, const vec &point, bool considerEdgesAsInside)
    {
        for (int i = 0; i < polygon.size(); i++)
        {
            int n = (i + 1) % polygon.size();

            if (isPointInLineSegment(polygon[i], polygon[n], point))
            {
                return considerEdgesAsInside;
            }
        }

    sdf:
        for (int i = 0; i < polygon.size(); i++)
        {
            int n = (i + 1) % polygon.size();
            int nn = (i + 2) % polygon.size();
            vec v =
                ((polygon[n] - polygon[i]) *
                    (polygon[nn] - polygon[i]));

            if (v.y > 0.0f) // the triangle winds in the right direction
            {
                bool thereIsAPointInside = false;
                for (const vec p : polygon) // check if the triangle contains a point inside
                {
                    if (p == polygon[i] || p == polygon[n] || p == polygon[nn])
                        continue;
                    if (isPointInsideTriangle(p, polygon[i], polygon[n], polygon[nn]))
                    {
                        thereIsAPointInside = true;
                        break;
                    }
                }

                if (!thereIsAPointInside)
                {
                    if (isPointInsideTriangle2D(point, polygon[i], polygon[n], polygon[nn]))
                        return true;
                    polygon.erase(polygon.begin() + n);
                    goto sdf;
                }
            }
        }
        return false;
    }

    bool areVectorsEqual(vec a, vec b, float threshold)
    {
        vec displacement = a - b;
        return displacement.Magnitude() < threshold; // same position or pretty close
    }

    bool areVectorsEqual2D(vec a, vec b, float threshold)
    {
        vec displacement = a - b;
        displacement.y = 0.0f;
        return displacement.Magnitude() < threshold; // same position or pretty close
    }

}