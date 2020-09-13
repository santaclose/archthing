
namespace Wall {

	void Create(vec& position, vec& direction, float length)
	{
		vec forwardVector = direction.Normalized();
		vec rightVector = (direction * vec::up).Normalized();

		vec cursor = position - rightVector * wallThickness * 0.5f;
		ml::setMaterial("wall");
		unsigned int vertices[4 * 2];
		vertices[0] = ml::vertex(cursor);
		vertices[1] = ml::vertex(cursor + rightVector * wallThickness);
		vertices[2] = ml::vertex(cursor + rightVector * wallThickness + vec::up * wallHeight);
		vertices[3] = ml::vertex(cursor + vec::up * wallHeight);
		vertices[4] = ml::vertex(cursor + forwardVector * length);
		vertices[5] = ml::vertex(cursor + rightVector * wallThickness + forwardVector * length);
		vertices[6] = ml::vertex(cursor + rightVector * wallThickness + vec::up * wallHeight + forwardVector * length);
		vertices[7] = ml::vertex(cursor + vec::up * wallHeight + forwardVector * length);

		std::vector<unsigned int> doorFaces;
		doorFaces.push_back(vertices[0]);
		doorFaces.push_back(vertices[1]);
		doorFaces.push_back(vertices[2]);
		doorFaces.push_back(vertices[3]);
		doorFaces.push_back(vertices[7]);
		doorFaces.push_back(vertices[6]);
		doorFaces.push_back(vertices[5]);
		doorFaces.push_back(vertices[4]);
		doorFaces.push_back(vertices[0]);
		doorFaces.push_back(vertices[3]);
		doorFaces.push_back(vertices[7]);
		doorFaces.push_back(vertices[4]);
		doorFaces.push_back(vertices[1]);
		doorFaces.push_back(vertices[5]);
		doorFaces.push_back(vertices[6]);
		doorFaces.push_back(vertices[2]);
		ml::faceSeq(doorFaces, 4);
	}

	void Create(unsigned int* wallVertices, const std::vector<gv>& vertices, const std::vector<ge>& edges, const ge& edge)
	{
		ml::setMaterial("wall");
		unsigned int quad[4];
		quad[0] = wallVertices[1];
		quad[1] = wallVertices[5];
		quad[2] = wallVertices[6];
		quad[3] = wallVertices[2];
		ml::face(quad, 4);
		quad[0] = wallVertices[0];
		quad[1] = wallVertices[4];
		quad[2] = wallVertices[7];
		quad[3] = wallVertices[3];
		ml::face(quad, 4, true);
		if (vertices[edge.a].conn.size() == 1)
			ml::face(&wallVertices[0], 4);
		if (vertices[edge.b].conn.size() == 1)
			ml::face(&wallVertices[4], 4, true);
	}
}