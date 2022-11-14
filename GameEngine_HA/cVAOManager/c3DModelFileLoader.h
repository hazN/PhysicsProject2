#pragma once

#include "sModelDrawInfo.h"
#include "../PhysicsSystem.h"

class c3DModelFileLoader
{
public:
	bool LoadPLYFile_Format_XYZ_N_RGBA_UV(std::string filename, 
										  sModelDrawInfo& modelDrawInfo,
										  std::string &errorText);


};
