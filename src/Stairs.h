#pragma once

namespace Stairs {

	void CreateStraightStairs(unsigned int* stairsQuad)
	{
		vec forwardVector = (ml::getVertexPosition(stairsQuad[3]) - ml::getVertexPosition(stairsQuad[0]));
		forwardVector.y = 0.0f;
		float segmentLength = forwardVector.Magnitude();
		forwardVector.Normalize();

		float segmentHeight = (ml::getVertexPosition(stairsQuad[3]) - ml::getVertexPosition(stairsQuad[0])).y;

		unsigned int numberOfSteps = round(segmentHeight / idealStairStepHeight), tri[3];
		float stepHeight = segmentHeight / numberOfSteps;
		float stepLength = segmentLength / numberOfSteps;

		unsigned int verticalQuad[4], horizontalQuad[4];
		verticalQuad[0] = stairsQuad[0];
		verticalQuad[1] = stairsQuad[1];
		for (unsigned int cs = 0; cs < numberOfSteps; cs++)
		{
			verticalQuad[2] = ml::vertex(ml::getVertexPosition(verticalQuad[1]) + vec::up * stepHeight);
			verticalQuad[3] = ml::vertex(ml::getVertexPosition(verticalQuad[0]) + vec::up * stepHeight);
			ml::setMaterial("floor");
			ml::face(verticalQuad, 4);

			horizontalQuad[0] = verticalQuad[3];
			horizontalQuad[1] = verticalQuad[2];
			if (cs == numberOfSteps - 1)
			{
				horizontalQuad[2] = stairsQuad[2];
				horizontalQuad[3] = stairsQuad[3];
			}
			else
			{
				horizontalQuad[2] = ml::vertex(ml::getVertexPosition(horizontalQuad[1]) + forwardVector * stepLength);
				horizontalQuad[3] = ml::vertex(ml::getVertexPosition(horizontalQuad[0]) + forwardVector * stepLength);
			}
			ml::face(horizontalQuad, 4);

			// side sawtooth
			ml::setMaterial("wall");
			tri[0] = verticalQuad[0];
			tri[1] = verticalQuad[3];
			tri[2] = horizontalQuad[3];
			ml::face(tri, 3);
			tri[0] = verticalQuad[1];
			tri[1] = verticalQuad[2];
			tri[2] = horizontalQuad[2];
			ml::face(tri, 3, true);

			verticalQuad[0] = horizontalQuad[3];
			verticalQuad[1] = horizontalQuad[2];
		}
	}

	void Create(const std::vector<std::vector<vec>>& paths, float width)
	{
		float targetHeight = wallHeight;

		for (const std::vector<vec>& path : paths)
		{
			std::vector<float> segmentHeight(path.size() - 1);
			float totalLength = 0.0f;

			for (int i = 0; i < path.size() - 1; i++)
			{
				float mag = (path[i + 1] - path[i]).Magnitude();
				segmentHeight[i] = mag;
				totalLength += mag;
			}
			for (float& seg : segmentHeight)
				seg *= targetHeight / totalLength;

			std::vector<bool> turningLeft(path.size() - 2);
			std::vector<std::vector<vec>> intersectionVertPos(path.size() - 2);
		
			for (int i = 0; i < path.size() - 2; i++)
			{
				int n = i + 1;
				int nn = i + 2;

				vec a = (path[n] - path[i]).Normalized();
				vec b = (path[nn] - path[n]).Normalized();
				vec aRight = a * vec::up;
				vec bRight = b * vec::up;

				turningLeft[i] = ((path[n] - path[i]).Cross(path[nn] - path[n])).y > 0.0f;

				if (turningLeft[i])
				{
					intersectionVertPos[i].push_back(Geometry::intersect2DPointDir(path[i] - (aRight * width * 0.5f), path[nn] - (bRight * width * 0.5f), a, -b));
					intersectionVertPos[i].push_back(intersectionVertPos[i][0] + aRight * width);
					intersectionVertPos[i].push_back(Geometry::intersect2DPointDir(path[i] + (aRight * width * 0.5f), path[nn] + (bRight * width * 0.5f), a, -b));
					intersectionVertPos[i].push_back(intersectionVertPos[i][0] + bRight * width);
				}
				else
				{
					intersectionVertPos[i].push_back(Geometry::intersect2DPointDir(path[i] + (aRight * width * 0.5f), path[nn] + (bRight * width * 0.5f), a, -b));
					intersectionVertPos[i].push_back(intersectionVertPos[i][0] - aRight * width);
					intersectionVertPos[i].push_back(Geometry::intersect2DPointDir(path[i] - (aRight * width * 0.5f), path[nn] - (bRight * width * 0.5f), a, -b));
					intersectionVertPos[i].push_back(intersectionVertPos[i][0] - bRight * width);
				}
			}

			// create the geometry
			float currentHeight = floorThickness;

			unsigned int prevLeft, prevRight, quad[4], connectionQuad[4], belowQuad[4], sideQuads[8];
			unsigned int prevBelowLeft, prevBelowRight;
			for (int i = 0; i < path.size() - 1; i++)
			{
				if (i > 0)
				{
					// elbows
					quad[0] = ml::vertex(intersectionVertPos[i - 1][0] + vec::up * currentHeight);
					quad[1] = ml::vertex(intersectionVertPos[i - 1][1] + vec::up * currentHeight);
					quad[2] = ml::vertex(intersectionVertPos[i - 1][2] + vec::up * currentHeight);
					quad[3] = ml::vertex(intersectionVertPos[i - 1][3] + vec::up * currentHeight);
					ml::setMaterial("floor");
					ml::face(quad, 4, !turningLeft[i - 1]);

					belowQuad[0] = ml::vertex(ml::getVertexPosition(quad[0]) - vec::up * stairsThickness);
					belowQuad[1] = ml::vertex(ml::getVertexPosition(quad[1]) - vec::up * stairsThickness);
					belowQuad[2] = ml::vertex(ml::getVertexPosition(quad[2]) - vec::up * stairsThickness);
					belowQuad[3] = ml::vertex(ml::getVertexPosition(quad[3]) - vec::up * stairsThickness);
					ml::setMaterial("ceiling");
					ml::face(belowQuad, 4, turningLeft[i - 1]);

					sideQuads[0] = belowQuad[1];
					sideQuads[1] = belowQuad[2];
					sideQuads[2] = quad[2];
					sideQuads[3] = quad[1];
					sideQuads[4] = belowQuad[2];
					sideQuads[5] = belowQuad[3];
					sideQuads[6] = quad[3];
					sideQuads[7] = quad[2];
					ml::setMaterial("wall");
					ml::face(sideQuads, 4, !turningLeft[i - 1]);
					ml::face(&sideQuads[4], 4, !turningLeft[i - 1]);

					// straight parts
					connectionQuad[0] = prevLeft;
					connectionQuad[1] = prevRight;
					connectionQuad[2] = turningLeft[i - 1] ? quad[1] : quad[0];
					connectionQuad[3] = turningLeft[i - 1] ? quad[0] : quad[1];
					CreateStraightStairs(connectionQuad);

					connectionQuad[0] = prevBelowLeft;
					connectionQuad[1] = prevBelowRight;
					connectionQuad[2] = turningLeft[i - 1] ? belowQuad[1] : belowQuad[0];
					connectionQuad[3] = turningLeft[i - 1] ? belowQuad[0] : belowQuad[1];
					ml::setMaterial("ceiling");
					ml::face(connectionQuad, 4, true);
					
					// sides
					sideQuads[0] = turningLeft[i - 1] ? belowQuad[0] : belowQuad[1];
					sideQuads[1] = prevBelowLeft;
					sideQuads[2] = prevLeft;
					sideQuads[3] = turningLeft[i - 1] ? quad[0] : quad[1];
					sideQuads[4] = turningLeft[i - 1] ? belowQuad[1] : belowQuad[0];
					sideQuads[5] = prevBelowRight;
					sideQuads[6] = prevRight;
					sideQuads[7] = turningLeft[i - 1] ? quad[1] : quad[0];
					ml::setMaterial("wall");
					ml::face(sideQuads, 4);
					ml::face(&sideQuads[4], 4, true);

					if (turningLeft[i - 1])
					{
						prevLeft = quad[0];
						prevRight = quad[3];
						prevBelowLeft = belowQuad[0];
						prevBelowRight= belowQuad[3];
					}
					else
					{
						prevLeft = quad[3];
						prevRight = quad[0];
						prevBelowLeft = belowQuad[3];
						prevBelowRight = belowQuad[0];
					}
				}
				else
				{
					vec right = (path[1] - path[0]).Normalized().Cross(vec::up);
					prevLeft = ml::vertex(path[0] - right * width * 0.5f + vec::up * currentHeight);
					prevRight = ml::vertex(path[0] + right * width * 0.5f + vec::up * currentHeight);
					prevBelowLeft = ml::vertex(ml::getVertexPosition(prevLeft) - vec::up * stairsThickness);
					prevBelowRight = ml::vertex(ml::getVertexPosition(prevRight) - vec::up * stairsThickness);
				}

				currentHeight += segmentHeight[i];
			}

			vec right = (path[path.size() - 1] - path[path.size() - 2]).Normalized().Cross(vec::up);
			sideQuads[2] =
			connectionQuad[0] = prevLeft;
			sideQuads[6] =
			connectionQuad[1] = prevRight;
			sideQuads[7] =
			connectionQuad[2] = ml::vertex(path[path.size() - 1] + right * width * 0.5f + vec::up * currentHeight);
			sideQuads[3] =
			connectionQuad[3] = ml::vertex(path[path.size() - 1] - right * width * 0.5f + vec::up * currentHeight);
			CreateStraightStairs(connectionQuad);

			sideQuads[1] =
			connectionQuad[0] = prevBelowLeft;
			sideQuads[5] =
			connectionQuad[1] = prevBelowRight;
			sideQuads[4] =
			connectionQuad[2] = ml::vertex(path[path.size() - 1] + right * width * 0.5f + vec::up * (currentHeight - stairsThickness));
			sideQuads[0] =
			connectionQuad[3] = ml::vertex(path[path.size() - 1] - right * width * 0.5f + vec::up * (currentHeight - stairsThickness));
			ml::setMaterial("ceiling");
			ml::face(connectionQuad, 4, true);

			// sides
			ml::setMaterial("wall");
			ml::face(sideQuads, 4);
			ml::face(&sideQuads[4], 4, true);
		}
	}
}