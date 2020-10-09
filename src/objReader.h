#pragma once
#include <vector>
#include <string>
#include "Wireframe.h"

namespace OBJReader
{
	void Read(Wireframe& wf, unsigned int& floorCount, const std::string& filePath, float wallHeight, float inputScale);
}