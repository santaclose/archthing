#include "Wireframe.h"

namespace Utils {

    int mod(int number,int mod);

    float maxDistanceBetweenVertices(const std::vector<gv>& vertices);

    void getAdjacentsSortedByAngle(
        const Wireframe& wf,
        int i,
        std::vector<std::pair<int, float>>& vertexAngle
    );

    void getEdgesSortedByAngle(
        const Wireframe& wf,
        int i,
        std::vector<std::pair<int, float>>& connectionAngle
    );
    
    bool castRayFromCorner(const std::vector<gv>& vertices,
        const std::vector<ge>& edges,
        float rayDistance,
        int i,
        int a,
        int b
    );

    std::pair<int, int> findFirstExternalVertices(
        const Wireframe& wf,
        float rayDistance,
        unsigned int floor, float wallHeight
    );

    void getExternal(
        const Wireframe& wf,
        std::vector<int>& outVertices,
        std::vector<int>& outCorners,
        unsigned int floor,
        float wallHeight
    );

    void getHoles(
        const Wireframe& wf,
        std::vector<std::vector<vec>>& holes,
        unsigned int floor,
        float wallHeight
    );

    void getStairs(const Wireframe& wf,
        std::vector<std::vector<vec>>& stairs,
        unsigned int floor,
        float wallHeight
    );

    void sortVertexConnectionsByAngle(Wireframe& wf);

    // adj must be already sorted by angle
    void getConnectionPoints(
        const vec& point,
        const std::vector<vec>& adj,
        float thickness,
        std::vector<vec>& connectionPoints
    );

    void getIntermediatePieces(
        const std::vector<vec>& below,
        const std::vector<vec>& above,
        std::vector<std::vector<vec>>& roofPieces,
        std::vector<std::vector<vec>>& ceilingPieces
    );
}