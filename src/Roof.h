
namespace Roof {

	void Create(const std::vector<vec>& externalCornerPositions) // positions with y=0
	{
		std::vector<unsigned int> inBelow, outBelow, outAbove, inAbove, hole;
	
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			std::vector<vec> adj;
			adj.push_back(externalCornerPositions[Utils::mod(i + 1, externalCornerPositions.size())]);
			adj.push_back(externalCornerPositions[Utils::mod(i - 1, externalCornerPositions.size())]);
	
			std::vector<vec> connectionPoints;
			Utils::getConnectionPoints(externalCornerPositions[i], adj, roofBevel, connectionPoints);
	
			inBelow.push_back(ml::vertex(externalCornerPositions[i] + vec::up * wallHeight));
			outBelow.push_back(ml::vertex(connectionPoints[1] + vec::up * wallHeight));
			outAbove.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight + roofThickness)));
			inAbove.push_back(ml::vertex(connectionPoints[0] + vec::up * (wallHeight + roofThickness)));
			hole.push_back(ml::vertex(connectionPoints[0] + vec::up * (wallHeight + roofThickness - roofDepth)));
		}
	
		ml::setMaterial("ceiling");
		ml::concaveFace(&inBelow[0], inBelow.size(), true);
		ml::setMaterial("roof");
		ml::concaveFace(&hole[0], hole.size());
	
		unsigned int quad[4];
		for (int i = 0; i < inBelow.size(); i++)
		{
			int nextIndex = (i + 1) % inBelow.size();
	
			quad[0] = inBelow[i];
			quad[1] = inBelow[nextIndex];
			quad[2] = outBelow[nextIndex];
			quad[3] = outBelow[i];
			ml::face(quad, 4);
			quad[0] = outBelow[i];
			quad[1] = outBelow[nextIndex];
			quad[2] = outAbove[nextIndex];
			quad[3] = outAbove[i];
			ml::face(quad, 4);
			quad[0] = outAbove[i];
			quad[1] = outAbove[nextIndex];
			quad[2] = inAbove[nextIndex];
			quad[3] = inAbove[i];
			ml::face(quad, 4);
			quad[0] = inAbove[i];
			quad[1] = inAbove[nextIndex];
			quad[2] = hole[nextIndex];
			quad[3] = hole[i];
			ml::face(quad, 4);
		}
	}

	void CreateIntermediate(const std::vector<vec>& externalCornerPositions) // positions with y=0
	{
		ml::setMaterial("roof");
		std::vector<unsigned int> below, above;
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			std::vector<vec> adj;
			adj.push_back(externalCornerPositions[Utils::mod(i + 1, externalCornerPositions.size())]);
			adj.push_back(externalCornerPositions[Utils::mod(i - 1, externalCornerPositions.size())]);

			std::vector<vec> connectionPoints;
			Utils::getConnectionPoints(externalCornerPositions[i], adj, wallThickness, connectionPoints);

			above.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight + floorThickness)));
			below.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight)));
		}

		ml::concaveFace(&above[0], above.size());

		ml::setMaterial("wall");
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

		ml::setMaterial("ceiling");
		ml::concaveFace(&below[0], below.size(), true);
	}

	void CreateIntermediateCeil(const std::vector<vec>& externalCornerPositions) // positions with y=0
	{
		ml::setMaterial("wall");
		std::vector<unsigned int> below, above;
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			std::vector<vec> adj;
			adj.push_back(externalCornerPositions[Utils::mod(i + 1, externalCornerPositions.size())]);
			adj.push_back(externalCornerPositions[Utils::mod(i - 1, externalCornerPositions.size())]);

			std::vector<vec> connectionPoints;
			Utils::getConnectionPoints(externalCornerPositions[i], adj, wallThickness, connectionPoints);

			above.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight)));
			below.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight - floorThickness)));
		}

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

		ml::concaveFace(&below[0], below.size(), true);
	}

}
