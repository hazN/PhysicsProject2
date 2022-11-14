#include "globalOpenGL.h"
#include "globalThings.h"   // For the light manager, etc.
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// Extern is so the compiler knows what TYPE this thing is
// The LINKER needs the ACTUAL declaration
// These are defined in theMainFunction.cpp
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);

enum eEditMode
{
	MOVING_CAMERA,
	MOVING_LIGHT,
	MOVING_SELECTED_OBJECT  // For later, maybe??
};

eEditMode theEditMode = MOVING_CAMERA;

bool bEnableDebugLightingObjects = true;
float OBJECT_MOVE_SPEED = 0.1f;
float CAMERA_MOVE_SPEED = 1.1f;
float LIGHT_MOVE_SPEED = 0.1f;
bool wireFrame = true;
//0000 0001   1	GLFW_MOD_SHIFT
//0000 0010 	  2
//0000 0100   4
//
//0000 0110
//0000 0001 	"Mask"
//-------- -
//0000 0000
//
//// I ONLY want the shift key and nothing else
//if (mods == GLFW_MOD_SHIFT)
//
//// Shift key but I don't care if anything else is down, too
//if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)

void key_callback(GLFWwindow* window,
	int key, int scancode,
	int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		if (mods == GLFW_MOD_CONTROL)
		{
			std::ofstream saveFile;
			saveFile.open("saveData.txt");
			saveFile << ::g_cameraEye.x << " " << ::g_cameraEye.y << " " << ::g_cameraEye.z << std::endl;
			for (cLight light : ::g_pTheLightManager->vecTheLights)
			{
				saveFile << "l" << std::endl << light.position.x << " " << light.position.y << " " << light.position.z << " " << light.position.w << " "
					<< light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << " "
					<< light.specular.x << " " << light.specular.y << " " << light.specular.z << " " << light.specular.w << " "
					<< light.atten.x << " " << light.atten.y << " " << light.atten.z << " " << light.atten.w << " "
					<< light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << " "
					<< light.param1.x << " " << light.param1.y << " " << light.param1.z << " " << light.param1.w << " "
					<< light.param2.x << " " << std::endl;
			}
			for (cMeshObject* object : g_pMeshObjects)
			{
				saveFile << "o" << std::endl << object->meshName << " " << object->friendlyName << " " << object->position.x << " " << object->position.y << " " << object->position.z << " "
					<< object->rotation.x << " " << object->rotation.y << " " << object->rotation.z << " " << object->scale << " " << object->isWireframe << " "
					<< object->bUse_RGBA_colour << " " << object->RGBA_colour.x << " " << object->RGBA_colour.y << " " << object->RGBA_colour.z << " " << object->RGBA_colour.w << " "
					<< object->bDoNotLight << " " << object->bIsVisible << std::endl;
			}
			std::cout << "Save successful..." << std::endl;
			saveFile.close();
		}
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (mods == GLFW_MOD_CONTROL)
		{
			int lightIndex = 0;
			int modelIndex = 0;
			std::ifstream saveFile("saveData.txt");
			if (!saveFile.is_open())
			{
				std::cout << "Load failed..." << std::endl;
			}
			else {
				std::string line;
				std::istringstream cam(line);
				std::getline(saveFile, line);
				cam >> g_cameraEye.x >> g_cameraEye.y >> g_cameraEye.z;
				while (std::getline(saveFile, line))
				{
					std::istringstream in(line);
					std::string type;
					in >> type;
					if (type == "l")
					{
						if (lightIndex < g_pTheLightManager->vecTheLights.size())
						{
							std::getline(saveFile, line);
							std::istringstream in2(line);
							in2 >> g_pTheLightManager->vecTheLights[lightIndex].position.x >> g_pTheLightManager->vecTheLights[lightIndex].position.y >> g_pTheLightManager->vecTheLights[lightIndex].position.z >> g_pTheLightManager->vecTheLights[lightIndex].position.w
								>> g_pTheLightManager->vecTheLights[lightIndex].diffuse.x >> g_pTheLightManager->vecTheLights[lightIndex].diffuse.y >> g_pTheLightManager->vecTheLights[lightIndex].diffuse.z >> g_pTheLightManager->vecTheLights[lightIndex].diffuse.w
								>> g_pTheLightManager->vecTheLights[lightIndex].specular.x >> g_pTheLightManager->vecTheLights[lightIndex].specular.y >> g_pTheLightManager->vecTheLights[lightIndex].specular.z >> g_pTheLightManager->vecTheLights[lightIndex].specular.w
								>> g_pTheLightManager->vecTheLights[lightIndex].atten.x >> g_pTheLightManager->vecTheLights[lightIndex].atten.y >> g_pTheLightManager->vecTheLights[lightIndex].atten.z >> g_pTheLightManager->vecTheLights[lightIndex].atten.w
								>> g_pTheLightManager->vecTheLights[lightIndex].direction.x >> g_pTheLightManager->vecTheLights[lightIndex].direction.y >> g_pTheLightManager->vecTheLights[lightIndex].direction.z >> g_pTheLightManager->vecTheLights[lightIndex].direction.w
								>> g_pTheLightManager->vecTheLights[lightIndex].param1.x >> g_pTheLightManager->vecTheLights[lightIndex].param1.y >> g_pTheLightManager->vecTheLights[lightIndex].param1.z >> g_pTheLightManager->vecTheLights[lightIndex].param1.w >> g_pTheLightManager->vecTheLights[lightIndex].param2.x;
							lightIndex++;
						}
					}
					else if (type == "o")
					{
						if (modelIndex < g_pMeshObjects.size())
						{
							std::getline(saveFile, line);
							std::istringstream in2(line);
							in2 >> g_pMeshObjects[modelIndex]->meshName >> g_pMeshObjects[modelIndex]->friendlyName >> g_pMeshObjects[modelIndex]->position.x >> g_pMeshObjects[modelIndex]->position.y >> g_pMeshObjects[modelIndex]->position.z
								>> g_pMeshObjects[modelIndex]->rotation.x >> g_pMeshObjects[modelIndex]->rotation.y >> g_pMeshObjects[modelIndex]->rotation.z >> g_pMeshObjects[modelIndex]->scale >> g_pMeshObjects[modelIndex]->isWireframe
								>> g_pMeshObjects[modelIndex]->bUse_RGBA_colour >> g_pMeshObjects[modelIndex]->RGBA_colour.x >> g_pMeshObjects[modelIndex]->RGBA_colour.y >> g_pMeshObjects[modelIndex]->RGBA_colour.z >> g_pMeshObjects[modelIndex]->RGBA_colour.w
								>> g_pMeshObjects[modelIndex]->bDoNotLight >> g_pMeshObjects[modelIndex]->bIsVisible;
							modelIndex++;
						}
					}
				}
				std::cout << "Load successful!" << std::endl;
				//for (cMeshObject* g_p_mesh_object : g_pMeshObjects)
				//{
			/*		pVAOManager->LoadStaticModelToOurAABBEnvironment("assets/models/" + g_p_mesh_object->meshName + ".ply", g_p_mesh_object->position, g_p_mesh_object->scale);
				}
				pVAOManager->LoadStaticModelToOurAABBEnvironment("HelmsDeep", g_pMeshObjects[0]->position, g_pMeshObjects[0]->scale);
				const std::map<int, std::vector<Triangle*>> aabb = m_PhysicsSystem.GetAABBStructure();
				std::map<int, std::vector<Triangle*>>::const_iterator aabbIt = aabb.begin();

				for (; aabbIt != aabb.end(); aabbIt++)
				{
					int hashValue = (*aabbIt).first;
					std::vector<Triangle*> triangles = (*aabbIt).second;

					std::vector<glm::vec3> vertices;
					std::vector<int> faces;

					for (int i = 0; i < triangles.size(); i++)
					{
						Triangle* triangle = triangles[i];
						vertices.push_back(triangle->A);
						vertices.push_back(triangle->B);
						vertices.push_back(triangle->C);

						faces.push_back(i * 3);
						faces.push_back(i * 3 + 1);
						faces.push_back(i * 3 + 2);
					}
				}
				*/
				// Character sphere
				Sphere* otherSphere = new Sphere(g_pMeshObjects[2]->position, g_pMeshObjects[2]->scale);
				m_PhysicsSystem.CreatePhysicsObject(g_pMeshObjects[2]->position, otherSphere);
				m_PhysicsSystem.m_PhysicsObjects[0]->m_IsStatic = false;
				std::vector<glm::vec3> vertices;
				std::vector<int> triangles;
				sModelDrawInfo draw_info;
				// Helms deep
				pVAOManager->FindDrawInfoByModelName("HelmsDeep", draw_info);
				for (int i = 0; i < draw_info.numberOfVertices; i++)
				{
					vertices.push_back(glm::vec3(draw_info.pVertices[i].x, draw_info.pVertices[i].y, draw_info.pVertices[i].z));
				}
				// Create our mesh inside the physics system
				for (int i = 0; i < draw_info.numberOfTriangles; i += 3) {
					int indexA = i;
					int indexB = i + 1;
					int indexC = i + 2;

					// HACK to save time from fixing the vertices returned from the GDP Graphics library
					if (indexA + 2 >= vertices.size()) {
						printf("Skipping creating a triangle!\n");
						continue;
					}

					glm::vec3 vertexA = glm::vec3(vertices[indexA]) * g_pMeshObjects[0]->scale + g_pMeshObjects[0]->position;
					glm::vec3 vertexB = glm::vec3(vertices[indexB]) * g_pMeshObjects[0]->scale + g_pMeshObjects[0]->position;
					glm::vec3 vertexC = glm::vec3(vertices[indexC]) * g_pMeshObjects[0]->scale + g_pMeshObjects[0]->position;

					Triangle* triangle = new Triangle(vertexA, vertexB, vertexC);
					PhysicsObject* trianglePhysObj = m_PhysicsSystem.CreatePhysicsObject(g_pMeshObjects[0]->position, triangle);
					trianglePhysObj->SetMass(-1.f);
				}
				Loaded = true;
			}
		}
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		theEditMode = MOVING_CAMERA;
	}
	else if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theEditMode = MOVING_LIGHT;

		//        // Check for the mods to turn the spheres on or off
		//        if ( mods == GLFW_MOD_CONTROL )
		//        {
		//            bEnableDebugLightingObjects = true;
		//        }
		//        if ( mods == GLFW_MOD_ALT )
		//        {
		//            bEnableDebugLightingObjects = false;
		//        }
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		theEditMode = MOVING_SELECTED_OBJECT;
	}
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		// Check for the mods to turn the spheres on or off
		bEnableDebugLightingObjects = false;
	}
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		// Check for the mods to turn the spheres on or off
		bEnableDebugLightingObjects = true;
	}

	switch (theEditMode)
	{
	case MOVING_CAMERA:
	{
		// Move the camera
		// AWSD   AD - left and right
		//        WS - forward and back
		if (key == GLFW_KEY_A)     // Left
		{
			::g_cameraEye.x -= CAMERA_MOVE_SPEED;
		}
		if (key == GLFW_KEY_D)     // Right
		{
			::g_cameraEye.x += CAMERA_MOVE_SPEED;
		}
		if (key == GLFW_KEY_W)     // Forward
		{
			::g_cameraEye.z += CAMERA_MOVE_SPEED;
		}
		if (key == GLFW_KEY_S)     // Backwards
		{
			::g_cameraEye.z -= CAMERA_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Q)     // Down
		{
			::g_cameraEye.y -= CAMERA_MOVE_SPEED;
		}
		if (key == GLFW_KEY_E)     // Up
		{
			::g_cameraEye.y += CAMERA_MOVE_SPEED;
		}

		if (key == GLFW_KEY_1)
		{
			::g_cameraEye = glm::vec3(-5.5f, -3.4f, 15.0f);
		}
	}//case MOVING_CAMERA:
	break;

	case MOVING_LIGHT:
	{
		if (mods == GLFW_MOD_SHIFT)
		{
			if (LIGHT_MOVE_SPEED == 0.1f)
			{
				LIGHT_MOVE_SPEED = 3.f;
			}
			else LIGHT_MOVE_SPEED = 0.1f;
		}
		if (key == GLFW_KEY_A)     // Left
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.x -= LIGHT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_D)     // Right
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.x += LIGHT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_W)     // Forward
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.z += LIGHT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_S)     // Backwards
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.z -= LIGHT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Q)     // Down
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.y -= LIGHT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_E)     // Up
		{
			::g_pTheLightManager->vecTheLights[currentLight].position.y += LIGHT_MOVE_SPEED;
		}

		if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
		{
			// Select previous light
			if (currentLight > 0)
			{
				currentLight--;
			}
		}
		if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
		{
			if (currentLight < (::g_pTheLightManager->vecTheLights.size() - 1))
			{
				// Select previous light
				currentLight++;
			}
		}

		// Change attenuation
		// Linear is ==> "how bright the light is"
		// Quadratic is ==> "how far does the light go or 'throw' into the scene?"
		if (key == GLFW_KEY_1)
		{
			// Linear Decrease by 1%
			::g_pTheLightManager->vecTheLights[currentLight].atten.y *= 0.99f;
		}
		if (key == GLFW_KEY_2)
		{
			// Linear Increase by 1%
			::g_pTheLightManager->vecTheLights[currentLight].atten.y *= 1.01f;
		}
		if (key == GLFW_KEY_3)
		{
			if (mods == GLFW_MOD_SHIFT)
			{   // ONLY shift modifier is down
				// Quadratic Decrease by 0.1%
				::g_pTheLightManager->vecTheLights[currentLight].atten.z *= 0.99f;
			}
			else
			{
				// Quadratic Decrease by 0.01%
				::g_pTheLightManager->vecTheLights[currentLight].atten.z *= 0.999f;
			}
		}
		if (key == GLFW_KEY_4)
		{
			if (mods == GLFW_MOD_SHIFT)
			{   // ONLY shift modifier is down
				// Quadratic Increase by 0.1%
				::g_pTheLightManager->vecTheLights[currentLight].atten.z *= 1.01f;
			}
			else
			{
				// Quadratic Decrease by 0.01%
				::g_pTheLightManager->vecTheLights[currentLight].atten.z *= 1.001f;
			}
		}
		if (key == GLFW_KEY_5)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.x *= 0.99f;
		}
		if (key == GLFW_KEY_6)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.x *= 1.01f;
		}
		if (key == GLFW_KEY_7)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.y *= 1.01f;
		}
		if (key == GLFW_KEY_8)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.y *= 0.99f;
		}
		if (key == GLFW_KEY_T)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.z *= 0.99f;
		}
		if (key == GLFW_KEY_Y)
		{
			::g_pTheLightManager->vecTheLights[currentLight].diffuse.z *= 1.01f;
		}
	}//case MOVING_LIGHT:
	break;
	case MOVING_SELECTED_OBJECT:
	{
		if (mods == GLFW_MOD_SHIFT)
		{
			if (OBJECT_MOVE_SPEED == 0.1f)
			{
				OBJECT_MOVE_SPEED = 1.f;
			}
			else OBJECT_MOVE_SPEED = 0.1f;
		}
		if (key == GLFW_KEY_A)     // Left
		{
			::g_pMeshObjects[currentModel]->position.x += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_D)     // Right
		{
			::g_pMeshObjects[currentModel]->position.x -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_W)     // Forward
		{
			::g_pMeshObjects[currentModel]->position.z += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_S)     // Backwards
		{
			::g_pMeshObjects[currentModel]->position.z -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Q)     // Down
		{
			::g_pMeshObjects[currentModel]->position.y -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_E)     // Up
		{
			::g_pMeshObjects[currentModel]->position.y += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Z)     // Rotate x
		{
			::g_pMeshObjects[currentModel]->rotation.x += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_X)     // Rotate x
		{
			::g_pMeshObjects[currentModel]->rotation.x -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_V)     // Rotate y
		{
			::g_pMeshObjects[currentModel]->rotation.y += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_B)     // Rotate y
		{
			::g_pMeshObjects[currentModel]->rotation.y -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_F)     // Rotate z
		{
			::g_pMeshObjects[currentModel]->rotation.z += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_G)     // Rotate z
		{
			::g_pMeshObjects[currentModel]->rotation.z -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_EQUAL)     // Size UP
		{
			::g_pMeshObjects[currentModel]->scale += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_MINUS)     // Size DOWN
		{
			::g_pMeshObjects[currentModel]->scale -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
		{
			// Select previous light
			if (currentModel > 0)
			{
				g_pMeshObjects[currentModel]->isWireframe = false;
				currentModel--;
				g_pMeshObjects[currentModel]->isWireframe = wireFrame;
			}
		}
		if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
		{
			if (currentModel < (::g_pMeshObjects.size() - 1))
			{
				g_pMeshObjects[currentModel]->isWireframe = false;
				currentModel++;
				g_pMeshObjects[currentModel]->isWireframe = wireFrame;
			}
		}
		if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS)
		{
			if (wireFrame == true)
			{
				wireFrame = false;
			}
			else wireFrame = true;
		}
		g_pMeshObjects[currentModel]->isWireframe = wireFrame;
	}
	break;
	}//switch (theEditMode)

	return;
}