#include "globalOpenGL.h"
#include "globalThings.h"   // For the light manager, etc.
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <glm/ext/matrix_transform.hpp>
// Extern is so the compiler knows what TYPE this thing is
// The LINKER needs the ACTUAL declaration
// These are defined in theMainFunction.cpp
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);

enum eEditMode
{
	MOVING_CAMERA,
	MOVING_LIGHT,
	MOVING_SELECTED_OBJECT  
};

eEditMode theEditMode = MOVING_SELECTED_OBJECT;

bool bEnableDebugLightingObjects = true;
float OBJECT_MOVE_SPEED = 0.1f;
float CAMERA_MOVE_SPEED = 1.1f;
float LIGHT_MOVE_SPEED = 0.1f;
bool wireFrame = false;

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
		/*	if (mods == GLFW_MOD_SHIFT)
			{
				if (OBJECT_MOVE_SPEED == 0.1f)
				{
					OBJECT_MOVE_SPEED = 1.f;
				}
				else OBJECT_MOVE_SPEED = 0.1f;
			}*/
		OBJECT_MOVE_SPEED = 0.0001;
		if (key == GLFW_KEY_A)     // Left
		{
			playerObject->ApplyForce(glm::vec3(-OBJECT_MOVE_SPEED, 0, 0));
			//::g_pMeshObjects[currentModel]->position.x += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_D)     // Right
		{
			playerObject->ApplyForce(glm::vec3(OBJECT_MOVE_SPEED, 0, 0));
			//m_PhysicsSystem.m_PhysicsObjects[0]->ApplyForce(glm::vec3(OBJECT_MOVE_SPEED, 0, 0));
			//::g_pMeshObjects[currentModel]->position.x -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_W)     // Forward
		{
			playerObject->ApplyForce(glm::vec3(0, 0, -OBJECT_MOVE_SPEED));
			//m_PhysicsSystem.m_PhysicsObjects[0]->ApplyForce(glm::vec3(0, 0, -OBJECT_MOVE_SPEED));
			//::g_pMeshObjects[currentModel]->position.z += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_S)     // Backwards
		{
			playerObject->ApplyForce(glm::vec3(0, 0, OBJECT_MOVE_SPEED));
			//m_PhysicsSystem.m_PhysicsObjects[0]->ApplyForce(glm::vec3(0, 0, OBJECT_MOVE_SPEED));
			//::g_pMeshObjects[currentModel]->position.z -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Q)     // Down
		{
			playerObject->ApplyForce(glm::vec3(0, -OBJECT_MOVE_SPEED, 0));
			//m_PhysicsSystem.m_PhysicsObjects[0]->ApplyForce(glm::vec3(0, -OBJECT_MOVE_SPEED, 0));
			//::g_pMeshObjects[currentModel]->position.y -= OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_E)     // Up
		{
			playerObject->ApplyForce(glm::vec3(0, OBJECT_MOVE_SPEED, 0));
			//m_PhysicsSystem.m_PhysicsObjects[0]->ApplyForce(glm::vec3(0, OBJECT_MOVE_SPEED, 0));
			//::g_pMeshObjects[currentModel]->position.y += OBJECT_MOVE_SPEED;
		}
		if (key == GLFW_KEY_Z)     // Rotate x
		{
			::g_pMeshObjects[2]->rotation.x += 0.5f;
		}
		if (key == GLFW_KEY_X)     // Rotate x
		{
			::g_pMeshObjects[2]->rotation.x -= 0.5f;
		}
		if (key == GLFW_KEY_V)     // Rotate y
		{
			::g_pMeshObjects[2]->rotation.y += 0.5f;
		}
		if (key == GLFW_KEY_B)     // Rotate y
		{
			::g_pMeshObjects[2]->rotation.y -= 0.5f;
		}
		if (key == GLFW_KEY_F)     // Rotate z
		{
			::g_pMeshObjects[2]->rotation.z += 0.5f;
		}
		if (key == GLFW_KEY_G)     // Rotate z
		{
			::g_pMeshObjects[2]->rotation.z -= 0.5f;
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