
namespace Floor {

	void Create(std::vector<vec>& externalCornerPositions, bool bottomCap = true) // positions with y=0
	{
		ml::setMaterial("floor");
		std::vector<unsigned int> low, high;
		for (int i = 0; i < externalCornerPositions.size(); i++)
		{
			low.push_back(ml::vertex(externalCornerPositions[i]));
			high.push_back(ml::vertex(externalCornerPositions[i] + vec::up * floorThickness));
		}
		ml::concaveFace(&high[0], high.size());

		unsigned int quad[4];
		for (int i = 0; i < low.size(); i++)
		{
			int nextIndex = (i + 1) % low.size();

			quad[0] = low[i];
			quad[1] = low[nextIndex];
			quad[2] = high[nextIndex];
			quad[3] = high[i];
			ml::face(quad, 4);
		}

		if (bottomCap)
		{
			ml::setMaterial("ceiling");
			ml::concaveFace(&low[0], low.size(), true);
		}
	}
}
