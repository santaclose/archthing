#include <unordered_map>

namespace Floor {

	void Create(const std::vector<vec>& externalCornerPositions, bool bottomCap = true) // positions with y=0
	{
		ml::setMaterial("floor");
		std::vector<unsigned int> below, above;
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			above.push_back(ml::vertex(externalCornerPositions[i] + vec::up * floorThickness));
			below.push_back(ml::vertex(externalCornerPositions[i]));
		}
		ml::concaveFace(&above[0], above.size());

		unsigned int quad[4];
		for (int i = 0; i < below.size(); i++)
		{
			int nextIndex = (i + 1) % below.size();

			quad[0] = below[i];
			quad[1] = below[nextIndex];
			quad[2] = above[nextIndex];
			quad[3] = above[i];
			ml::face(quad, 4);
		}

		if (bottomCap)
		{
			ml::setMaterial("ceiling");
			ml::concaveFace(&below[0], below.size(), true);
		}
	}

	void Create(const std::vector<vec>& externalCornerPositions, const std::vector<std::vector<vec>> holes, bool bottomCap = true, bool sideFaces = false) // positions with y=0
	{
		// store what corner vertex is used by each hole
		std::vector<std::pair<vec, vec>> holeToExternalWallEdges;

		// find where to add new edges that connect holes with walls
		std::unordered_map<int, int> holePointFromCorner;
		std::unordered_map<int, int> holeIdFromCorner;

		for (int hid = 0; hid < holes.size(); hid++)
		{
			bool foundEdgeForThisHole = false;
			for (int hi = 0; hi < holes[hid].size() && !foundEdgeForThisHole; hi++)
			{
				for (int ci = 0; ci < externalCornerPositions.size() && !foundEdgeForThisHole; ci++)
				{
					bool edgeIsValid = true;
					// check for intersections with any external walls
					for (int i = 0; i < externalCornerPositions.size() && edgeIsValid; i++)
					{
						int next = (i + 1) % externalCornerPositions.size();
						if (i == ci || next == ci)
							continue;
						if (Geometry::intersect2D(holes[hid][hi], externalCornerPositions[ci], externalCornerPositions[i], externalCornerPositions[next]))
						{
							edgeIsValid = false;
							break;
						}
					}
					// check for intersections with this same hole
					for (int i = 0; i < holes[hid].size() && edgeIsValid; i++)
					{
						int next = (i + 1) % holes[hid].size();
						if (i == hi || next == hi)
							continue;
						if (Geometry::intersect2D(holes[hid][hi], externalCornerPositions[ci], holes[hid][i], holes[hid][next]))
						{
							edgeIsValid = false;
							break;
						}
					}
					// check for intersections with other holes
					for (int i = 0; i < holes.size() && edgeIsValid; i++)
					{
						if (i == hid)
							continue;

						for (int j = 0; j < holes[i].size() && edgeIsValid; j++)
						{
							int next = (j + 1) % holes[i].size();
							if (Geometry::intersect2D(holes[hid][hi], externalCornerPositions[ci], holes[i][j], holes[i][next]))
							{
								edgeIsValid = false;
								break;
							}
						}
					}
					// check if theres another hole using this wall vertex
					for (std::pair<vec, vec> edge : holeToExternalWallEdges)
					{
						if (Geometry::areVectorsEqual2D(edge.second, externalCornerPositions[ci]) ||
							Geometry::intersect2D(holes[hid][hi], externalCornerPositions[ci], edge.first, edge.second))
						{
							edgeIsValid = false;
							break;
						}
					}

					if (edgeIsValid)
					{
						holeToExternalWallEdges.push_back({ holes[hid][hi], externalCornerPositions[ci] });
						holePointFromCorner[ci] = hi;
						holeIdFromCorner[ci] = hid;
						foundEdgeForThisHole = true;
						//Primitives::Cylinder(0.2, 6, holes[hid][hi], externalCornerPositions[ci]);
					}
				}
			}
		}

		// create vertices
		ml::setMaterial("floor");
		std::vector<unsigned int> above, below;
		std::vector<std::vector<unsigned int>> holeAbove, holeBelow;

		for (const vec& point : externalCornerPositions)
		{
			above.push_back(ml::vertex(point + vec::up * floorThickness));
			below.push_back(ml::vertex(point));
		}
		for (const std::vector<vec>& hole : holes)
		{
			holeAbove.emplace_back();
			holeBelow.emplace_back();
			for (const vec& point : hole)
			{
				holeAbove.back().push_back(ml::vertex(point + vec::up * floorThickness));
				holeBelow.back().push_back(ml::vertex(point));
			}
		}

		// create faces
		std::vector<unsigned int> polygonAbove, polygonBelow;
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			polygonAbove.push_back(above[i]);
			polygonBelow.push_back(below[i]);

			if (auto edge = holePointFromCorner.find(i); edge != holePointFromCorner.end())
			{
				int currentHole = holeIdFromCorner[i];
				int currentHolePoint = holePointFromCorner[i];

				for (int j = 0; j < holes[currentHole].size() + 1; j++)
				{
					polygonAbove.push_back(holeAbove[currentHole][currentHolePoint]);
					polygonBelow.push_back(holeBelow[currentHole][currentHolePoint]);

					currentHolePoint--;
					currentHolePoint = Utils::mod(currentHolePoint, holes[currentHole].size());
				}

				polygonAbove.push_back(above[i]);
				polygonBelow.push_back(below[i]);
			}
		}

		ml::concaveFace(&polygonAbove[0], polygonAbove.size());

		unsigned int quad[4];

		if (sideFaces)
		{
			for (int i = 0; i < below.size(); i++)
			{
				int nextIndex = (i + 1) % below.size();

				quad[0] = below[i];
				quad[1] = below[nextIndex];
				quad[2] = above[nextIndex];
				quad[3] = above[i];
				ml::face(quad, 4);
			}
		}

		// edges for every hole
		ml::setMaterial("wall");
		for (int j = 0; j < holes.size(); j++)
		{
			for (int i = 0; i < holes[j].size(); i++)
			{
				int nextIndex = (i + 1) % holes[j].size();

				quad[0] = holeBelow[j][i];
				quad[1] = holeBelow[j][nextIndex];
				quad[2] = holeAbove[j][nextIndex];
				quad[3] = holeAbove[j][i];
				ml::face(quad, 4, true);
			}
		}

		if (bottomCap)
		{
			ml::setMaterial("ceiling");
			ml::concaveFace(&polygonBelow[0], polygonBelow.size(), true);
		}
	}
}
