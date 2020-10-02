// OpenGLProject.cpp : Defines the entry point for the application.
//

#include "OpenGLProject.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <chrono>

#include "ShaderLoader.h"
#include "Camera.h"
#include "LightRenderer.h"
#include "MeshRenderer.h"
#include "TextureLoader.h"
#include "TextRenderer.h"

Camera* camera;
LightRenderer* light;

MeshRenderer* sphere;
MeshRenderer* ground;
MeshRenderer* enemy;

btDiscreteDynamicsWorld* dynamicsWorld;
TextRenderer* label;

GLuint flatShaderProgram, textProgram;

GLuint texturedShaderProgram;

GLuint litTexturedShaderProgram;

GLuint sphereTexture, groundTexture;

bool grounded = false;
bool gameover = true;
int score = 0;

static void glfwError(int id, const char* description);
void initGame();
void renderScene();
void addRigidBodies();
void myTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep);
void updateKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

void initGame()
{
	glEnable(GL_DEPTH_TEST);

	ShaderLoader shader;

	flatShaderProgram = 
		shader.createProgram(
			"Assets/Shaders/FlatModel.vert", 
			"Assets/Shaders/FlatModel.frag");

	texturedShaderProgram =
		shader.createProgram("Assets/Shaders/TexturedModel.vert",
			"Assets/Shaders/TexturedModel.frag");

	litTexturedShaderProgram =
		shader.createProgram("Assets/Shaders/LitTexturedModel.vert",
			"Assets/Shaders/LitTexturedModel.frag");

	textProgram =
		shader.createProgram(
			"Assets/Shaders/text.vert",
			"Assets/Shaders/text.frag"
		);

	TextureLoader tLoader;
	sphereTexture = tLoader.getTextureID("Assets/Textures/globe.jpg");

	camera = new Camera(45.0f, 800, 600, 0.1f, 100.0f, glm::vec3(0.0f,4.0f, 20.0f));

	// Light
	light = new LightRenderer(MeshType::kSphere, camera);
	light->setProgram(flatShaderProgram);
	light->setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
	light->setColor(glm::vec3(1, 1, 1));

	// Text Label
	label = new TextRenderer("Score: 0", "Assets/fonts/gooddog.ttf", 64, glm::vec3(1, 0, 0), textProgram);
	label->setPosition(glm::vec2(320, 550));
	
	// Ground Mesh
	groundTexture = tLoader.getTextureID("Assets/Textures/ground.jpg");

	//init physics
	auto* broadphase = new btDbvtBroadphase();
	auto* collisionConfiguration = new btDefaultCollisionConfiguration();
	auto* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	auto* solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
	dynamicsWorld->setInternalTickCallback(myTickCallback);

	addRigidBodies();
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 0, 1);

	// draw game objects here
	light->draw();
	sphere->draw();
	enemy->draw();
	ground->draw();
	
	label->draw();
}

void addRigidBodies()
{
	// Sphere RighidBody
	auto* sphereShape = new btSphereShape(1.0f);
	auto* sphereMotionState = new
		btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
			btVector3(0, 0.5f, 0)));
	
	btScalar mass = 13.0;
	btVector3 sphereInertia(0, 0, 0);
	sphereShape->calculateLocalInertia(mass, sphereInertia);
	
	btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
	
	auto* sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
	
	sphereRigidBody->setRestitution(1.0f);
	sphereRigidBody->setFriction(1.0f);

	sphereRigidBody->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(sphereRigidBody);

	// Sphere Mesh
	sphere = new MeshRenderer(MeshType::kSphere, "hero", camera, sphereRigidBody, light, 0.1f, 0.5f);
	sphere->setProgram(texturedShaderProgram);
	sphere->setTexture(sphereTexture);
	sphere->setScale(glm::vec3(1.0f));

	sphereRigidBody->setUserPointer(sphere);

	// Ground RigidBody
	auto* groundShape = new btBoxShape(btVector3(4.0f, 0.5f, 4.0f));
	auto* groundMotionState = new
		btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
			btVector3(0, -1.0f, 0)));
	
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.0f, groundMotionState, groundShape, btVector3(0, 0, 0));
	auto* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	
	groundRigidBody->setFriction(1.0);
	groundRigidBody->setRestitution(0.0);
	
	groundRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	
	dynamicsWorld->addRigidBody(groundRigidBody);

	// Ground Mesh
	ground = new MeshRenderer(MeshType::kCube, "ground", camera, groundRigidBody, light, 0.1f, 0.5f);
	ground->setProgram(texturedShaderProgram);
	ground->setTexture(groundTexture);
	ground->setScale(glm::vec3(4.0f, 0.5f, 4.0f));

	groundRigidBody->setUserPointer(ground);

	// Enemy Rigid body
	auto* shape = new btBoxShape(btVector3(1.0f, 1.0f,
		1.0f));
	auto* motionState = new
		btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
			btVector3(18.0, 1.0f, 0)));
	btRigidBody::btRigidBodyConstructionInfo rbCI(0.0f, motionState,
		shape, btVector3(0.0f, 0.0f, 0.0f));

	auto* rb = new btRigidBody(rbCI);
	
	rb->setFriction(1.0);
	rb->setRestitution(0.0);
	
	rb->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	dynamicsWorld->addRigidBody(rb);
	
	// Enemy Mesh
	enemy = new MeshRenderer(MeshType::kCube, "enemy", camera, rb, light, 0.1f, 0.5f);
	enemy->setProgram(texturedShaderProgram);
	enemy->setTexture(groundTexture);
	enemy->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	
	rb->setUserPointer(enemy);
}

void myTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep)
{
	if (!gameover)
	{
		// Get enemy transform
		auto t(enemy->rigidBody->getWorldTransform());

		// Set enemy position
		t.setOrigin(t.getOrigin() + btVector3(-15, 0, 0) * timeStep);

		// Check if offScreen
		if (t.getOrigin().x() <= -18.0f)
		{
			t.setOrigin(btVector3(18, 1, 0));
			score++;
			label->setText("Score: " + std::to_string(score));
		}
		enemy->rigidBody->setWorldTransform(t);
		enemy->rigidBody->getMotionState()->setWorldTransform(t);

		/* Collision Handling
		 * Iterates over each contact manifold (contact pair)
		 * and checks for collisions and handles them appropriate to our game logic
		 */
		{
			grounded = false;

			const auto numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();

			for (auto i = 0; i < numManifolds; i++)
			{
				auto* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

				const auto numContacts = contactManifold->getNumContacts();

				if (numContacts > 0)
				{
					const auto* objA = contactManifold->getBody0();
					const auto* objB = contactManifold->getBody1();

					auto* gModA = static_cast<MeshRenderer*>(objA->getUserPointer());
					auto* gModB = static_cast<MeshRenderer*>(objB->getUserPointer());

					if ((gModA->name == "hero" && gModB->name == ("enemy"))
						|| (gModA->name == "enemy" && gModB->name == "hero"))
					{
						// std::cout << "collision: " << gModA->name << " with " << gModB->name;

						if (gModB->name == "enemy")
						{
							auto b(gModB->rigidBody->getWorldTransform());
							b.setOrigin(btVector3(18, 1, 0));
							gModB->rigidBody->setWorldTransform(b);
							gModB->rigidBody->getMotionState()->setWorldTransform(b);
						}
						else
						{
							auto a(gModA->rigidBody->getWorldTransform());
							a.setOrigin(btVector3(18, 1, 0));
							gModA->rigidBody->setWorldTransform(a);
							gModA->rigidBody->setWorldTransform(a);
							gModA->rigidBody->setWorldTransform(a);
							gModA->rigidBody->getMotionState()->setWorldTransform(a);
						}
						gameover = true;
						score = 0;
						label->setText("Score: " + std::to_string(score));
					}

					if ((gModA->name == "hero" && gModB->name == "ground")
						|| (gModA->name == "ground" && gModB->name == "hero"))
					{
						// std::cout << "collision: " << gModA->name << " with " << gModB->name;
						grounded = true;
					}
				}
			}
		}
	}
}

void updateKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		if (gameover)
		{
			gameover = false;
		} else if (grounded == true) 
		{
			grounded = false;
			
			sphere->rigidBody->applyImpulse(btVector3(0.0f, 100.0f, 0.0f),
				btVector3(0.0f, 0.0f, 0.0f));
			
			// std::cout << "pressed up key \n";
		}
	}
}


int main(int argc, char **argv)
{
	glfwSetErrorCallback(&glfwError);
	
	glfwInit();
	auto* window = glfwCreateWindow(800, 600, "Hello OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, updateKeyboard);

	glewInit();
	initGame();
	auto previousTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();

		dynamicsWorld->stepSimulation(dt);
		
		// render our scene
		renderScene();
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		previousTime = currentTime;
	}
	glfwTerminate();

	delete camera;
	delete light;
	
	return 0;
}
