#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
#include <glm/vec3.hpp>
#include "../PhysicsSystem.h"
#include "sModelDrawInfo.h"

//struct sVertex
//{
//	float x, y, z;		
//	float r, g, b;
//	float nx, ny, nz;	// And normals, too!! :) 
//};



// This holds the "type" of model (mesh) we are going to draw. 
class cVAOManager
{
public:

	bool LoadModelIntoVAO(std::string fileName, 
						  sModelDrawInfo &drawInfo, 
						  unsigned int shaderProgramID);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);

	std::string getLastError(bool bAndClear = true);
	void LoadStaticModelToOurAABBEnvironment(const std::string& filepath, const glm::vec3& position, float scale);
	void createPhysicsObject(std::string meshName, glm::vec3 position, float scale);
private:

	std::map< std::string /*model name*/,
		      sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;
};

#endif	// _cVAOManager_HG_
