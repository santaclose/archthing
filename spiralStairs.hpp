#include "../src/modelTool/ml.h"
#define MODEL_SET
#define BINDINGS

vec position = { 0.0f, 0.0f, 0.0f };
float height = 5.0f;
float innerRadius = 1.0f;
float outerRadius = 2.8f;
float thickness = 0.2f;
float startAngle = 0.0f;
float totalAngle = PI;

int stepCount = 50;
float stepHeight = 0.07;
float stepAngle = 2 * PI / 30;

float idealStepHeight = 0.1905f;

void bindings()
{
	BIND(SliderFloat3, "Position", &position.x, -5.0f, 5.0f);
	BIND(SliderFloat, "Height", &height, 1.0f, 20.0f);
	BIND(SliderFloat, "Inner Radius", &innerRadius, 0.1f, 20.0f);
	BIND(SliderFloat, "Outer Radius", &outerRadius, 0.1f, 20.0f);
	BIND(SliderFloat, "Thickness", &thickness, 0.01f, 2.0f);
	BIND(SliderFloat, "Start Angle", &startAngle, 0.0f, 2.0f * PI);
	BIND(SliderFloat, "Total Angle", &totalAngle, PI / 6.0f, 4 * PI);
}

void spiral(vec origin)
{
	stepCount = (unsigned int)round(height / idealStepHeight);
	stepHeight = height / stepCount;
	stepAngle = totalAngle / stepCount;

	using namespace ml;
	float currentAngle = startAngle;

	unsigned int outerSide[7]; //tris
	unsigned int innerSide[7]; //tris
	unsigned int bottom[4]; // quads
	unsigned int vertices[4]; //quads

	vec pos(cos(currentAngle) * innerRadius,
		0,
		-sin(currentAngle) * innerRadius);

	vertices[0] = vertex(origin + pos);
	innerSide[1] = innerSide[3] = innerSide[5] = vertices[0];
	bottom[0] = vertex(origin + pos - vec::up * thickness);
	innerSide[0] = bottom[0];
	pos.x = cos(currentAngle) * outerRadius;
	pos.z = -sin(currentAngle) * outerRadius;
	vertices[1] = vertex(origin + pos);
	outerSide[6] = outerSide[2] = vertices[1];
	bottom[3] = vertex(origin + pos - vec::up * thickness);
	outerSide[0] = bottom[3];

	for (int i = 0; i < stepCount; i++)
	{
		pos += vec::up * stepHeight;
		vertices[2] = vertex(origin + pos);
		outerSide[5] = vertices[2];
		pos.x = cos(currentAngle) * innerRadius;
		pos.z = -sin(currentAngle) * innerRadius;
		vertices[3] = vertex(origin + pos);
		innerSide[6] = vertices[3];
		setMaterial("wall");
		face(vertices, 4);

		currentAngle += stepAngle;
		vertices[0] = vertices[3];
		vertices[1] = vertices[2];
		pos.x = cos(currentAngle) * outerRadius;
		pos.z = -sin(currentAngle) * outerRadius;
		vertices[2] = vertex(origin + pos);
		bottom[2] = vertex(origin + pos - vec::up * thickness);
		outerSide[1] = outerSide[3] = bottom[2];
		outerSide[4] = vertices[2];
		pos.x = cos(currentAngle) * innerRadius;
		pos.z = -sin(currentAngle) * innerRadius;
		vertices[3] = vertex(origin + pos);
		innerSide[4] = vertices[3];
		bottom[1] = vertex(origin + pos - vec::up * thickness);
		innerSide[2] = bottom[1];
		setMaterial("floor");
		face(vertices, 4);
		setMaterial("ceiling");
		face(bottom, 4);
		setMaterial("wall");
		face(outerSide, 3);
		face(outerSide, 3, 2);
		face(outerSide, 3, 4);
		face(innerSide, 3);
		face(innerSide, 3, 2);
		face(innerSide, 3, 4);

		vertices[0] = vertices[3];
		vertices[1] = vertices[2];

		bottom[0] = bottom[1];
		bottom[3] = bottom[2];

		outerSide[6] = outerSide[2] = outerSide[4];
		outerSide[0] = outerSide[1];

		innerSide[0] = innerSide[2];
		innerSide[1] = innerSide[3] = innerSide[5] = innerSide[4];

		pos.x = cos(currentAngle) * outerRadius;
		pos.z = -sin(currentAngle) * outerRadius;
	}
}


void generateModel()
{
	spiral(position);
}