#ifndef _globalThings_HG_
#define _globalThings_HG_

// This is anything that is shared by all (or many) of the files

#include "cMeshObject.h"
#include "cLightManager.h"
#include "cVAOManager/cVAOManager.h"

// extern means the variable isn't actually here...
// ...it's somewhere else (in a .cpp file somewhere)
extern cLightManager* g_pTheLightManager;
extern std::vector< cMeshObject* > g_pMeshObjects;
extern std::vector< glm::vec3* > terrainVertices;
extern cVAOManager* pVAOManager;
extern std::map<int, cMeshObject*> g_PartialMeshObjects;
extern std::map<int, cMeshObject*>::iterator g_PartialMeshObjectsCursor;
extern PhysicsSystem m_PhysicsSystem;
extern bool Loaded; 
extern int currentLight;
extern int currentModel;
#endif
