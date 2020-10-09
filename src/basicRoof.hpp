
namespace Roof {

	void Create(const std::vector<vec>& externalCornerPositions) // positions with y=0
	{
		std::vector<unsigned int> inLow, outLow, outHigh, inHigh, hole;
	
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			std::vector<vec> adj;
			adj.push_back(externalCornerPositions[(i + 1) % externalCornerPositions.size()]);
			adj.push_back(externalCornerPositions[(i - 1 + externalCornerPositions.size()) % externalCornerPositions.size()]);
	
			std::vector<vec> connectionPoints;
			Utils::getConnectionPoints(externalCornerPositions[i], adj, roofBevel, connectionPoints);
	
			inLow.push_back(ml::vertex(externalCornerPositions[i] + vec::up * wallHeight));
			outLow.push_back(ml::vertex(connectionPoints[1] + vec::up * wallHeight));
			outHigh.push_back(ml::vertex(connectionPoints[1] + vec::up * (wallHeight + roofThickness)));
			inHigh.push_back(ml::vertex(connectionPoints[0] + vec::up * (wallHeight + roofThickness)));
			hole.push_back(ml::vertex(connectionPoints[0] + vec::up * (wallHeight + roofThickness - roofDepth)));
		}
	
		ml::setMaterial("ceiling");
		ml::concaveFace(&inLow[0], inLow.size(), true);
		ml::setMaterial("roof");
		ml::concaveFace(&hole[0], hole.size());
	
		unsigned int quad[4];
		for (int i = 0; i < inLow.size(); i++)
		{
			int nextIndex = (i + 1) % inLow.size();
	
			quad[0] = inLow[i];
			quad[1] = inLow[nextIndex];
			quad[2] = outLow[nextIndex];
			quad[3] = outLow[i];
			ml::face(quad, 4);
			quad[0] = outLow[i];
			quad[1] = outLow[nextIndex];
			quad[2] = outHigh[nextIndex];
			quad[3] = outHigh[i];
			ml::face(quad, 4);
			quad[0] = outHigh[i];
			quad[1] = outHigh[nextIndex];
			quad[2] = inHigh[nextIndex];
			quad[3] = inHigh[i];
			ml::face(quad, 4);
			quad[0] = inHigh[i];
			quad[1] = inHigh[nextIndex];
			quad[2] = hole[nextIndex];
			quad[3] = hole[i];
			ml::face(quad, 4);
		}
	}

	//void Create(std::vector<vec>& externalCornerPositions) // positions with y=0
	//{
	//	std::vector<unsigned int> inLow, outLow, outHigh, inHigh, hole;

	//	for (int i = 0; i < externalCornerPositions.size(); i++)
	//	{
	//		inLow.push_back(ml::vertex(externalCornerPositions[i] + vec::up * wallHeight));
	//		inHigh.push_back(ml::vertex(externalCornerPositions[i] + vec::up * (wallHeight + roofThickness)));
	//		hole.push_back(ml::vertex(externalCornerPositions[i] + vec::up * (wallHeight + roofThickness - roofDepth)));
	//		
	//		int prevIndex = i == 0 ? externalCornerPositions.size() - 1 : (i - 1);
	//		int nextIndex = (i + 1) % externalCornerPositions.size();

	//		vec dirA = vec::up * (externalCornerPositions[prevIndex] - externalCornerPositions[i]);
	//		vec dirB = (externalCornerPositions[nextIndex] - externalCornerPositions[i]) * vec::up;
	//		dirA.Normalize();
	//		dirB.Normalize();

	//		outLow.push_back(ml::vertex(externalCornerPositions[i] + (dirA + dirB).Normalized() * roofBevel + vec::up * wallHeight));
	//		outHigh.push_back(ml::vertex(externalCornerPositions[i] + (dirA + dirB).Normalized() * roofBevel + vec::up * (wallHeight + roofThickness)));
	//	}

	//	ml::concaveFace(&inLow[0], inLow.size(), true);
	//	ml::concaveFace(&hole[0], hole.size());

	//	unsigned int quad[4];
	//	for (int i = 0; i < inLow.size(); i++)
	//	{
	//		int nextIndex = (i + 1) % inLow.size();

	//		quad[0] = inLow[i];
	//		quad[1] = inLow[nextIndex];
	//		quad[2] = outLow[nextIndex];
	//		quad[3] = outLow[i];
	//		ml::face(quad, 4);
	//		quad[0] = outLow[i];
	//		quad[1] = outLow[nextIndex];
	//		quad[2] = outHigh[nextIndex];
	//		quad[3] = outHigh[i];
	//		ml::face(quad, 4);
	//		quad[0] = outHigh[i];
	//		quad[1] = outHigh[nextIndex];
	//		quad[2] = inHigh[nextIndex];
	//		quad[3] = inHigh[i];
	//		ml::face(quad, 4);
	//		quad[0] = inHigh[i];
	//		quad[1] = inHigh[nextIndex];
	//		quad[2] = hole[nextIndex];
	//		quad[3] = hole[i];
	//		ml::face(quad, 4);
	//	}

	//}
		/*std::vector<std::pair<int, float>> cornerArea; divide by area test
		for (int i = 0; i < externalCornerPositions.size(); i++) // testing
		{
			float area =
				((externalCornerPositions[i] - externalCornerPositions[(i + 1) % externalCornerPositions.size()]) *
				(externalCornerPositions[i] - externalCornerPositions[(i - 1) % externalCornerPositions.size()])).Magnitude();
			cornerArea.push_back({ i, area });
		}
		std::cout << "printing areas\n";
		for (std::pair<int, float>& lal : cornerArea)
		{
			std::cout << lal.first << ", " << lal.second << std::endl;
		}*/
}
