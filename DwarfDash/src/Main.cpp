#include "Utils.h"

//Config
#include "Configuration.h"

#include <sstream>
#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"

//PhysX
#include "physxInclude/PxPhysicsAPI.h"
#include "physxInclude/pvd/PxPvd.h"
#include "physxInclude/pvd/PxPvdSceneClient.h"
#include "physxInclude/pvd/PxPvdTransport.h"

// Model loading
#include "Model.h"

// Camera
#include "FPSCamera.h"

//Game
#include "Game.h"
#include <stb_image.h>

//Namespaces
using namespace physx;
using namespace std;

//Temp
#include <glm/gtx/string_cast.hpp>

// TextRendering
#include "TextRendering.h"

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setWindowFPS(GLFWwindow *window,float& t_sum);
void initPhysX();
void releasePhysX();
void stepPhysics(float deltaTime);

// FPS Camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window, float deltaTime);

void setPerFrameUniforms(Shader* shader, FPSCamera camera, DirectionalLight& dirL, std::vector<PointLight> pointlightArray);

// Skybox
unsigned int loadCubemap(vector<std::string> faces);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

// settings.ini
Configuration config = Configuration("assets/settings.ini");

// camera
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 10.0f;

// normal mapping
static int normalMapping = false;

// PhysX
float mAccumulator = 0.0f;
float timeStep = 1.0f / 60.0f;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* gFoundation = nullptr;
static PxPhysics* gPhysics = nullptr;
static PxScene* gScene = nullptr;
static PxPvd* gPvd = nullptr;
static PxCudaContextManager* gCudaContextManager = NULL;

// Game
Game* game = new Game();

int frames = 0;
// fps camera settings
float lastX = config.width / 2.0f;
float lastY = config.height / 2.0f;
bool firstMouse = true;
FPSCamera camera(config.fov, float(config.width) / float(config.height), config.nearZ, config.farZ); // new constructor

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */
int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW")
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, config.refresh_rate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (config.fullscreen)
		monitor = glfwGetPrimaryMonitor();

	GLFWwindow* window = glfwCreateWindow(config.width, config.height, config.title.c_str(), monitor, nullptr);

	if (!window) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to create window")
	}

	//Set gamma for monitor =/= window
	glfwSetGamma(glfwGetPrimaryMonitor(), config.brightness);

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("Failed to init GLEW")
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework")
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// set GL defaults
	glClearColor(0.0, 0.0, 0.0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// needed for text rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// Init PhysX
		initPhysX();
		 
		// Init game
		game->camPointer = &camera;
		game->gPhysics = gPhysics;
		game->gScene = gScene;
		game->init();

		Shader lightCubeShader("light_cube.vert", "light_cube.frag");
		Shader textShader("textRendering.vert", "textRendering.frag");

		TextRendering textRenderer(textShader, float(config.width), float(config.height));

		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(1.0f, 1.4f, 1.0f)); // color,  direction;

		glm::vec3 boxpos1 = glm::vec3(0.61f, 2.8f, -19.8f);
		glm::vec3 boxpos2 = glm::vec3(12.27f, 2.8f, -19.8f);
		glm::vec3 boxpos3 = glm::vec3(12.5f, 6.8f, -50.0f );
		glm::vec3 boxpos4 = glm::vec3(11.47f, 6.8f, -88.0f);

		std::vector<PointLight> pointlightArray = std::vector<PointLight>(); // pointlight array filled with pointlights for each level
		PointLight pointLight1 = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(boxpos1), glm::vec3(1.0f, 0.1f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)
		PointLight pointLight2 = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(boxpos2), glm::vec3(1.0f, 0.1f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)
		PointLight pointLight3 = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(boxpos3), glm::vec3(1.0f, 0.1f, 0.01f)); // plattform in the middle
		PointLight pointLight4 = PointLight(glm::vec3(1.0f, 0.0f, 0.f),  glm::vec3(boxpos4), glm::vec3(1.0f, 0.4f, 0.1f));   // plattform right before the goal

		pointlightArray.push_back(pointLight1);
		pointlightArray.push_back(pointLight2);
		pointlightArray.push_back(pointLight3);
		pointlightArray.push_back(pointLight4);

		// visualization cube
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		// first, configure the cube's VAO (and VBO)
		unsigned int VBO, cubeVAO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		unsigned int lightCubeVAO;
		glGenVertexArrays(1, &lightCubeVAO);
		glBindVertexArray(lightCubeVAO);

		// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// skybox VAO
		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// cave skybox
		vector<std::string> faces{
			("assets/models/skybox/cave/right.png"),
			("assets/models/skybox/cave/left.png"),
			("assets/models/skybox/cave/top.png"),
			("assets/models/skybox/cave/bottom.png"),
			("assets/models/skybox/cave/front.png"),
			("assets/models/skybox/cave/back.png")
		};
		unsigned int cubemapTexture = loadCubemap(faces);

		// set skybox
		game->skyboxShader->use();
		game->skyboxShader->setUniform("skybox", 0);

		// Render loop
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;

		while (!glfwWindowShouldClose(window)) {
			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Poll events
			glfwPollEvents();
			processInput(window, dt);

			// Set per-frame uniforms
			setPerFrameUniforms(game->primaryShader.get(), camera, dirL, pointlightArray); // multiple pointlights

			//PhysX
			stepPhysics(dt);

			// draw skybox before opaque particle shader, to give information about background
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			game->skyboxShader->use();
			glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
			glm::mat4 projection = glm::mat4(camera.getProjectionMatrix());
			game->skyboxShader->setUniform("view", view);
			game->skyboxShader->setUniform("projection", projection);

			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default

			// Render
			game->update(dt);
			game->draw();

			/*
			// light visualization
			// only needed for visual debugging
			lightCubeShader.use();
			lightCubeShader.setUniform("view", camera.getViewMatrix());
			lightCubeShader.setUniform("projection", camera.getProjectionMatrix());
			lightCubeShader.setUniform("viewProjMatrix", camera.getViewProjectionMatrix());

			// multiple cubes
			glm::vec3 cubePositions[] = {
				glm::vec3(boxpos1),
				glm::vec3(boxpos2),
				glm::vec3(boxpos3),
				glm::vec3(boxpos4)
			};

			for (unsigned int i = 0; i <= 4; i++) {
				// calculate the model matrix for each cube and pass it to shader before drawing
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				model = glm::scale(model, glm::vec3(0.4f));
				lightCubeShader.setUniform("modelMatrix", model);
				glBindVertexArray(lightCubeVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			*/



			// render text
			textRenderer.renderText("current points:   " + to_string(game->player->score), 25.0f, 25.0f, 1.0f, glm::vec3(0.6f, 1.0f, 0.5f));

			// Compute frame time
			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;
			frames++;

			setWindowFPS(window,t_sum);

			// Swap buffers
			glfwSwapBuffers(window);
		}
	}


	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy Game
	/* --------------------------------------------- */

	game->~Game();

	/* --------------------------------------------- */
	// Destroy PhysX
	/* --------------------------------------------- */

	releasePhysX();

	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}

void stepPhysics(float deltaTime)
{
	mAccumulator += deltaTime;
	if (mAccumulator < timeStep) {
		return;
	}
	else {
		mAccumulator -= timeStep;
		gScene->simulate(timeStep);
		gScene->fetchResults(true);
	}
}

void initPhysX() {
	//Creating foundation for PhysX
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!gFoundation)
	{
		cerr << "Error creating PhysX3 foundation, Exiting..." << endl;
		exit(1);
	}

	bool recordMemoryAllocations = true;

	gPvd = PxCreatePvd(*gFoundation);
	const char* gPvdHostIP = "127.0.0.1"; // IP of local PC machine PVD
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(gPvdHostIP, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Creating instance of PhysX SDK
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	if (gPhysics == NULL)
	{
		cerr << "Error creating PhysX3 device, Exiting..." << endl;
		exit(1);
	}

	//Creating scene with GPU support (Only for CUDA->Nvidia)
	PxCudaContextManagerDesc cudaContextManagerDesc;

	gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(4);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.cudaContextManager = gCudaContextManager;

	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;

	gScene = gPhysics->createScene(sceneDesc);

	//Pvd Client
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS | PxPairFlag::eTRIGGER_DEFAULT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;

	return  PxFilterFlag::eDEFAULT;
}

void releasePhysX()
{
	gScene->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	gFoundation->release();
}

void setWindowFPS(GLFWwindow *window, float& t_sum)
{
	//Every second print frametime and frames per second window title
	if (t_sum >= 1.0f) {
		stringstream title;
		title << config.title << " [" << to_string(1000.0f / double(frames)) + "ms/frame | " + to_string(frames) + " FPS]";
		glfwSetWindowTitle(window, title.str().c_str());
		t_sum -= 1.0f;
		frames = 0;
	}
}

// for multiple pointlights
void setPerFrameUniforms(Shader* shader, FPSCamera camera, DirectionalLight& dirL, std::vector<PointLight> pointlightArray)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);

	shader->setUniform("normalMapping", normalMapping);
	//std::cout << "set normalMapping to: " << normalMapping << std::endl;

	// TODO: check for level and add pointlights accordingly
	// iterate over all the pointlights	
	for (GLuint i = 0; i < pointlightArray.size(); i++) {
		string number = std::to_string(i);
		PointLight& pointLight = pointlightArray[i];

		shader->setUniformArr("pointLights", i, "color", pointLight.color);
		shader->setUniformArr("pointLights", i, "position", pointLight.position);
		shader->setUniformArr("pointLights", i, "attenuation", pointLight.attenuation);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}

// loads a cubemap texture from 6 individual texture faces
unsigned int loadCubemap(vector<std::string> faces) {

	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// FPS Camera & Player input processing
void processInput(GLFWwindow* window, float deltaTime){
	
	glm::vec3 displacement = glm::vec3(0.f);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		displacement += camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		displacement += camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		displacement += camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		displacement += camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(camera.getPosition()) << std::endl;
		camera.resetPosition();
	}

	//Movespeed fix while afloat
	if (displacement != glm::vec3(0.f,0.f,0.f))
	{
		displacement = glm::vec3(displacement.x, 0.f, displacement.z);
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		game->reset();
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		game->player->wantsToJump(deltaTime);
	}


	game->player->jump(deltaTime);

	game->player->moveChar(displacement, deltaTime);

	camera.setPosition(glm::vec3(
		game->player->gPlayerController->getPosition().x,
		game->player->gPlayerController->getPosition().y,
		game->player->gPlayerController->getPosition().z));
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit

	if (action != GLFW_RELEASE) return;

	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_F1:
			config.wireframe = !config.wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, config.wireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_F2:
			config.culling = !config.culling;
			if (config.culling) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			break;
		case GLFW_KEY_F4:
			normalMapping = !normalMapping;
			//std::cout << "set normalMapping to: " << (normalMapping ? "true" : "false") << std::endl;
			break;
	}
	
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	string error = FormatDebugOutput(source, type, id, severity, message);
	cout << error << endl;
}

static string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	stringstream stringStream;
	string sourceString;
	string typeString;
	string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}