#include "globalThings.h"


// The variable is HERE.
cLightManager* g_pTheLightManager = NULL;
std::vector< cMeshObject* > g_pMeshObjects;
std::vector< glm::vec3* > terrainVertices;
cVAOManager* pVAOManager = new cVAOManager();
std::map<int, cMeshObject*> g_PartialMeshObjects;
std::map<int, cMeshObject*>::iterator g_PartialMeshObjectsCursor;
PhysicsSystem m_PhysicsSystem;
PhysicsObject *playerObject;
bool Loaded = false;
int currentLight = 0;
int currentModel = 0;
bool endThread = false;