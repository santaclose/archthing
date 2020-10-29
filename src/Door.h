
namespace Door {

	void Create(unsigned int* wallVertices, const std::vector<gv>& vertices, const std::vector<ge>& edges, const ge& edge)
	{
		ml::setMaterial("wall");
		unsigned int newWallVertices[4];
		newWallVertices[0] = ml::vertex(ml::getVertexPosition(wallVertices[3]) - vec::up * (wallHeight - (doorHeight + doorSeparation * 2 + floorThickness)));
		newWallVertices[1] = ml::vertex(ml::getVertexPosition(wallVertices[2]) - vec::up * (wallHeight - (doorHeight + doorSeparation * 2 + floorThickness)));
		newWallVertices[2] = ml::vertex(ml::getVertexPosition(wallVertices[6]) - vec::up * (wallHeight - (doorHeight + doorSeparation * 2 + floorThickness)));
		newWallVertices[3] = ml::vertex(ml::getVertexPosition(wallVertices[7]) - vec::up * (wallHeight - (doorHeight + doorSeparation * 2 + floorThickness)));
		ml::face(newWallVertices, 4, true);
		unsigned int quad[4];
		quad[0] = newWallVertices[1];
		quad[1] = newWallVertices[2];
		quad[2] = wallVertices[6];
		quad[3] = wallVertices[2];
		ml::face(quad, 4);
		quad[0] = newWallVertices[0];
		quad[1] = newWallVertices[3];
		quad[2] = wallVertices[7];
		quad[3] = wallVertices[3];
		ml::face(quad, 4, true);
		ml::face(&wallVertices[0], 4, true);
		ml::face(&wallVertices[4], 4);


		if (!createDoors)
			return;

		ml::setMaterial("door");
		vec forwardVector = (vertices[edge.b].pos - vertices[edge.a].pos).Normalized();
		vec rightVector = (forwardVector * vec::up); // cross product of two unit vectors is a unit vector
		float length = (vertices[edge.a].pos - vertices[edge.b].pos).Magnitude();
		unsigned int doorVertices[8];
		vec cursor = vertices[edge.a].pos + rightVector * (-doorThickness * 0.5f) + forwardVector * doorSeparation;
		doorVertices[0] = ml::vertex(cursor + vec::up * (doorSeparation + floorThickness));
		doorVertices[1] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + floorThickness));
		doorVertices[2] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + doorHeight + floorThickness));
		doorVertices[3] = ml::vertex(cursor + vec::up * (doorSeparation + doorHeight + floorThickness));
		cursor += forwardVector * (length - doorSeparation * 2);
		doorVertices[4] = ml::vertex(cursor + vec::up * (doorSeparation + floorThickness));
		doorVertices[5] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + floorThickness));
		doorVertices[6] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + doorHeight + floorThickness));
		doorVertices[7] = ml::vertex(cursor + vec::up * (doorSeparation + doorHeight + floorThickness));
		std::vector<unsigned int> doorFaces;
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[5]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[5]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[5]);
		ml::faceSeq(doorFaces, 4);

		ml::setMaterial("doorHandle");
		cursor = vertices[edge.a].pos + vec::up * (doorSeparation + doorHandlePosY + floorThickness) + forwardVector * (doorSeparation + doorHandlePosX);
		Primitives::Cylinder(0.005, 12, cursor - rightVector * 0.03f, cursor + rightVector * 0.03f, false);
		Primitives::Cylinder(0.02, 24, cursor - rightVector * 0.03f, cursor - rightVector * 0.043f, true);
		Primitives::Cylinder(0.02, 24, cursor + rightVector * 0.03f, cursor + rightVector * 0.043f, true);
	}

	void Create(vec& position, vec& direction, float length)
	{
		vec forwardVector = direction.Normalized();
		vec rightVector = (direction * vec::up).Normalized();

		// wall
		ml::setMaterial("wall");
		//vec cursor = position;
		vec cursor = position - rightVector * wallThickness * 0.5f;
		unsigned int quad[4];
		quad[0] = ml::vertex(cursor + vec::up * wallHeight);
		quad[1] = ml::vertex(cursor + forwardVector * length + vec::up * wallHeight);
		quad[2] = ml::vertex(cursor + forwardVector * length + vec::up * (doorHeight + doorSeparation * 2 + floorThickness));
		quad[3] = ml::vertex(cursor + vec::up * (doorHeight + doorSeparation * 2 + floorThickness));
		ml::face(quad, 4, false);
		quad[0] = ml::vertex(cursor + vec::up * (doorHeight + doorSeparation * 2 + floorThickness) + rightVector * wallThickness);
		quad[1] = ml::vertex(cursor + forwardVector * length + vec::up * (doorHeight + doorSeparation * 2 + floorThickness) + rightVector * wallThickness);
		ml::face(quad, 4, true);
		quad[2] = ml::vertex(cursor + rightVector * wallThickness + forwardVector * length + vec::up * wallHeight);
		quad[3] = ml::vertex(cursor + rightVector * wallThickness + vec::up * wallHeight);
		ml::face(quad, 4, false);

		// door
		ml::setMaterial("door");
		unsigned int doorVertices[4 * 2];
		cursor = position + rightVector * (- doorThickness * 0.5f) + forwardVector * doorSeparation;

		doorVertices[0] = ml::vertex(cursor + vec::up * (doorSeparation + floorThickness));
		doorVertices[1] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + floorThickness));
		doorVertices[2] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + doorHeight + floorThickness));
		doorVertices[3] = ml::vertex(cursor + vec::up * (doorSeparation + doorHeight + floorThickness));

		cursor += forwardVector * (length - doorSeparation * 2);

		doorVertices[4] = ml::vertex(cursor + vec::up * (doorSeparation + floorThickness));
		doorVertices[5] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + floorThickness));
		doorVertices[6] = ml::vertex(cursor + rightVector * doorThickness + vec::up * (doorSeparation + doorHeight + floorThickness));
		doorVertices[7] = ml::vertex(cursor + vec::up * (doorSeparation + doorHeight + floorThickness));

		std::vector<unsigned int> doorFaces;
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[5]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[5]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[2]);
		doorFaces.push_back(doorVertices[6]);
		doorFaces.push_back(doorVertices[7]);
		doorFaces.push_back(doorVertices[3]);
		doorFaces.push_back(doorVertices[1]);
		doorFaces.push_back(doorVertices[0]);
		doorFaces.push_back(doorVertices[4]);
		doorFaces.push_back(doorVertices[5]);
		ml::faceSeq(doorFaces, 4);

		// door handle
		ml::setMaterial("doorHandle");
		//cursor = position + rightVector * (wallThickness * 0.5f) + vec::up * (doorSeparation + doorHandlePosY) + forwardVector * (doorSeparation + doorHandlePosX);
		cursor = position + vec::up * (doorSeparation + doorHandlePosY) + forwardVector * (doorSeparation + doorHandlePosX);
		Primitives::Cylinder(0.005, 12, cursor - rightVector * 0.03f, cursor + rightVector * 0.03f, false);
		Primitives::Cylinder(0.02, 24, cursor - rightVector * 0.03f, cursor - rightVector * 0.043f, true);
		Primitives::Cylinder(0.02, 24, cursor + rightVector * 0.03f, cursor + rightVector * 0.043f, true);
	}
}
