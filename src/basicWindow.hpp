
namespace Window {

	void Create(unsigned int* wallVertices, const std::vector<gv>& vertices, const std::vector<ge>& edges, const ge& edge)
	{
		ml::setMaterial("wall");
		ml::face(&wallVertices[0], 4, true);
		ml::face(&wallVertices[4], 4);
		unsigned int newWallVertices[8];
		newWallVertices[0] = ml::vertex(ml::getVertexPosition(wallVertices[0]) + vec::up * (floorThickness + windowOffset));
		newWallVertices[1] = ml::vertex(ml::getVertexPosition(wallVertices[1]) + vec::up * (floorThickness + windowOffset));
		newWallVertices[2] = ml::vertex(ml::getVertexPosition(wallVertices[5]) + vec::up * (floorThickness + windowOffset));
		newWallVertices[3] = ml::vertex(ml::getVertexPosition(wallVertices[4]) + vec::up * (floorThickness + windowOffset));
		newWallVertices[4] = ml::vertex(ml::getVertexPosition(wallVertices[0]) + vec::up * (floorThickness + windowOffset + windowHeight));
		newWallVertices[5] = ml::vertex(ml::getVertexPosition(wallVertices[1]) + vec::up * (floorThickness + windowOffset + windowHeight));
		newWallVertices[6] = ml::vertex(ml::getVertexPosition(wallVertices[5]) + vec::up * (floorThickness + windowOffset + windowHeight));
		newWallVertices[7] = ml::vertex(ml::getVertexPosition(wallVertices[4]) + vec::up * (floorThickness + windowOffset + windowHeight));
		ml::face(&newWallVertices[0], 4);
		ml::face(&newWallVertices[4], 4, true);
		unsigned int quad[4];
		quad[0] = wallVertices[1];
		quad[1] = wallVertices[5];
		quad[2] = newWallVertices[2];
		quad[3] = newWallVertices[1];
		ml::face(quad, 4);
		quad[0] = wallVertices[0];
		quad[1] = wallVertices[4];
		quad[2] = newWallVertices[3];
		quad[3] = newWallVertices[0];
		ml::face(quad, 4, true);
		quad[0] = newWallVertices[5];
		quad[1] = newWallVertices[6];
		quad[2] = wallVertices[6];
		quad[3] = wallVertices[2];
		ml::face(quad, 4);
		quad[0] = newWallVertices[4];
		quad[1] = newWallVertices[7];
		quad[2] = wallVertices[7];
		quad[3] = wallVertices[3];
		ml::face(quad, 4, true);

		ml::setMaterial("windowFrame");
		vec forwardVector = (vertices[edge.b].pos - vertices[edge.a].pos).Normalized();
		vec rightVector = (forwardVector * vec::up); // cross product of two unit vectors is a unit vector
		float length = (vertices[edge.a].pos - vertices[edge.b].pos).Magnitude();
		unsigned int frameVertices[4 * 4];
		vec cursor = vertices[edge.a].pos + vec::up * (floorThickness + windowOffset) + rightVector * (-windowFrameThickness * 0.5f);
		frameVertices[0] = ml::vertex(cursor);
		frameVertices[1] = ml::vertex(cursor + rightVector * windowFrameThickness);
		frameVertices[2] = ml::vertex(cursor + rightVector * windowFrameThickness + forwardVector * windowFrameWidth + vec::up * windowFrameWidth);
		frameVertices[3] = ml::vertex(cursor + forwardVector * windowFrameWidth + vec::up * windowFrameWidth);
		frameVertices[4] = ml::vertex(cursor + vec::up * windowHeight);
		frameVertices[5] = ml::vertex(cursor + vec::up * windowHeight + rightVector * windowFrameThickness);
		frameVertices[6] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * windowFrameWidth + rightVector * windowFrameThickness);
		frameVertices[7] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * windowFrameWidth);
		frameVertices[8] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		frameVertices[9] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length + rightVector * windowFrameThickness);
		frameVertices[10] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * (length - windowFrameWidth) + rightVector * windowFrameThickness);
		frameVertices[11] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * (length - windowFrameWidth));
		frameVertices[12] = ml::vertex(cursor + forwardVector * length);
		frameVertices[13] = ml::vertex(cursor + forwardVector * length + rightVector * windowFrameThickness);
		frameVertices[14] = ml::vertex(cursor + forwardVector * (length - windowFrameWidth) + rightVector * windowFrameThickness + vec::up * windowFrameWidth);
		frameVertices[15] = ml::vertex(cursor + forwardVector * (length - windowFrameWidth) + vec::up * windowFrameWidth);
		std::vector<unsigned int> frameFaces;
		frameFaces.push_back(frameVertices[12]);
		frameFaces.push_back(frameVertices[0]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[0]);
		frameFaces.push_back(frameVertices[4]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[4]);
		frameFaces.push_back(frameVertices[8]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[8]);
		frameFaces.push_back(frameVertices[12]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[11]);
		ml::faceSeq(frameFaces, 4);
		frameFaces.clear();
		frameFaces.push_back(frameVertices[13]);
		frameFaces.push_back(frameVertices[1]);
		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[1]);
		frameFaces.push_back(frameVertices[5]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[5]);
		frameFaces.push_back(frameVertices[9]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[9]);
		frameFaces.push_back(frameVertices[13]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[10]);
		ml::faceSeq(frameFaces, 4, true);
		frameFaces.clear();
		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[2]);
		ml::faceSeq(frameFaces, 4, true);

		ml::setMaterial("windowGlass");
		cursor = vertices[edge.a].pos + vec::up * (floorThickness + windowOffset) + rightVector * (-glassThickness * 0.5f);
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + vec::up * windowHeight);
		quad[2] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		quad[3] = ml::vertex(cursor + forwardVector * length);
		ml::face(quad, 4);
		cursor += rightVector * glassThickness;
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + vec::up * windowHeight);
		quad[2] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		quad[3] = ml::vertex(cursor + forwardVector * length);
		ml::face(quad, 4, true);
	}

	void Create(vec& position, vec& direction, float length)
	{
		vec forwardVector = direction.Normalized();
		vec rightVector = (direction * vec::up).Normalized();

		// wall
		ml::setMaterial("wall");
		vec cursor = position - rightVector * wallThickness * 0.5f;
		unsigned int quad[4];
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + forwardVector * length);
		quad[2] = ml::vertex(cursor + forwardVector * length + vec::up * windowOffset);
		quad[3] = ml::vertex(cursor + vec::up * windowOffset);
		ml::face(quad, 4, true);
		quad[0] = ml::vertex(cursor + vec::up * windowOffset + rightVector * wallThickness);
		quad[1] = ml::vertex(cursor + vec::up * windowOffset + rightVector * wallThickness + forwardVector * length);
		ml::face(quad, 4);
		quad[2] = ml::vertex(cursor + rightVector * wallThickness + forwardVector * length);
		quad[3] = ml::vertex(cursor + rightVector * wallThickness);
		ml::face(quad, 4, true);

		cursor += vec::up * wallHeight;
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + forwardVector * length);
		quad[2] = ml::vertex(cursor + forwardVector * length - vec::up * (wallHeight - windowHeight - windowOffset));
		quad[3] = ml::vertex(cursor - vec::up * (wallHeight - windowHeight - windowOffset));
		ml::face(quad, 4);
		quad[0] = ml::vertex(cursor - vec::up * (wallHeight - windowHeight - windowOffset) + rightVector * wallThickness);
		quad[1] = ml::vertex(cursor + forwardVector * length - vec::up * (wallHeight - windowHeight - windowOffset) + rightVector * wallThickness);
		ml::face(quad, 4, true);
		quad[2] = ml::vertex(cursor + rightVector * wallThickness + forwardVector * length);
		quad[3] = ml::vertex(cursor + rightVector * wallThickness);
		ml::face(quad, 4);

		// window frame
		ml::setMaterial("windowFrame");
		unsigned int frameVertices[4 * 4];
		cursor = position + vec::up * windowOffset + rightVector * (- windowFrameThickness * 0.5f);

		frameVertices[0] = ml::vertex(cursor);
		frameVertices[1] = ml::vertex(cursor + rightVector * windowFrameThickness);
		frameVertices[2] = ml::vertex(cursor + rightVector * windowFrameThickness + forwardVector * windowFrameWidth + vec::up * windowFrameWidth);
		frameVertices[3] = ml::vertex(cursor + forwardVector * windowFrameWidth + vec::up * windowFrameWidth);

		frameVertices[4] = ml::vertex(cursor + vec::up * windowHeight);
		frameVertices[5] = ml::vertex(cursor + vec::up * windowHeight + rightVector * windowFrameThickness);
		frameVertices[6] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * windowFrameWidth + rightVector * windowFrameThickness);
		frameVertices[7] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * windowFrameWidth);

		frameVertices[8] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		frameVertices[9] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length + rightVector * windowFrameThickness);
		frameVertices[10] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * (length - windowFrameWidth) + rightVector * windowFrameThickness);
		frameVertices[11] = ml::vertex(cursor + vec::up * (windowHeight - windowFrameWidth) + forwardVector * (length - windowFrameWidth));

		frameVertices[12] = ml::vertex(cursor + forwardVector * length);
		frameVertices[13] = ml::vertex(cursor + forwardVector * length + rightVector * windowFrameThickness);
		frameVertices[14] = ml::vertex(cursor + forwardVector * (length - windowFrameWidth) + rightVector * windowFrameThickness + vec::up * windowFrameWidth);
		frameVertices[15] = ml::vertex(cursor + forwardVector * (length - windowFrameWidth) + vec::up * windowFrameWidth);

		std::vector<unsigned int> frameFaces;
		frameFaces.push_back(frameVertices[12]);
		frameFaces.push_back(frameVertices[0]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[0]);
		frameFaces.push_back(frameVertices[4]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[4]);
		frameFaces.push_back(frameVertices[8]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[8]);
		frameFaces.push_back(frameVertices[12]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[11]);
		ml::faceSeq(frameFaces, 4);
		frameFaces.clear();

		frameFaces.push_back(frameVertices[13]);
		frameFaces.push_back(frameVertices[1]);
		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[1]);
		frameFaces.push_back(frameVertices[5]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[5]);
		frameFaces.push_back(frameVertices[9]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[9]);
		frameFaces.push_back(frameVertices[13]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[10]);
		ml::faceSeq(frameFaces, 4, true);
		frameFaces.clear();

		frameFaces.push_back(frameVertices[2]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[14]);
		frameFaces.push_back(frameVertices[15]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[10]);
		frameFaces.push_back(frameVertices[11]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[6]);
		frameFaces.push_back(frameVertices[7]);
		frameFaces.push_back(frameVertices[3]);
		frameFaces.push_back(frameVertices[2]);
		ml::faceSeq(frameFaces, 4, true);

		// glass
		ml::setMaterial("windowGlass");
		cursor = position + vec::up * windowOffset + rightVector * (- glassThickness * 0.5f);
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + vec::up * windowHeight);
		quad[2] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		quad[3] = ml::vertex(cursor + forwardVector * length);
		ml::face(quad, 4);
		cursor += rightVector * glassThickness;
		quad[0] = ml::vertex(cursor);
		quad[1] = ml::vertex(cursor + vec::up * windowHeight);
		quad[2] = ml::vertex(cursor + vec::up * windowHeight + forwardVector * length);
		quad[3] = ml::vertex(cursor + forwardVector * length);
		ml::face(quad, 4, true);
	}
}