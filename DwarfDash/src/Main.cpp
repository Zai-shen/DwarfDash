#include "Utils.h"
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

//Config
#include "Configuration.h"

// Model loading
#include "Model.h"

// Camera
#include "FPSCamera.h"

//Game
#include "Game.h"

//Namespaces
using namespace physx;
using namespace std;

//Temp
#include <glm/gtx/string_cast.hpp>

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, PointLight& pointL);

void setWindowFPS(GLFWwindow *window, float& t_sum);
void initPhysX();
void releasePhysX();
void stepPhysics();


// FPS Camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void setPerFrameUniforms(Shader* shader, FPSCamera camera, DirectionalLight& dirL, PointLight& pointL);
void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, std::vector<PointLight> pointlightArray);



/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

// settings.ini
Configuration config = Configuration("assets/settings.ini");

// camera
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 10.0f;

// PhysX
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* gFoundation = nullptr;
static PxPhysics* gPhysics = nullptr;
static PxScene* gScene = nullptr;
static PxPvd* gPvd = nullptr;
static PxControllerManager* gCCTManager = nullptr;
static PxController* gPlayerController = nullptr;

// Game
int frames = 0;
Game* game = new Game();

// fps camera settings
float lastX = config.width / 2.0f;
float lastY = config.height / 2.0f;
bool firstMouse = true;
// timing for fps camera 
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//FPSCamera camera(config.fov, float(config.width) / float(config.height), config.nearZ, config.farZ);
Camera camera(config.fov, float(config.width) / float(config.height), config.nearZ, config.farZ);

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
	glfwSetMouseButtonCallback(window, mouse_button_callback); // not needed in fps
	glfwSetScrollCallback(window, scroll_callback);  // not needed in fps

	glfwSetKeyCallback(window, key_callback); // only needed in fps
	glfwSetCursorPosCallback(window, mouse_callback); // only needed in fps

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// set GL defaults
	glClearColor(0.5, 0.5, 0.5, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{


		// Init PhysX
		initPhysX();

		// Init game
		game->gPhysics = gPhysics;
		game->gScene = gScene;
		game->init();

		// Initialize camera
		//Camera camera(config.fov, float(config.width) / float(config.height), config.nearZ, config.farZ);


		// Initialize lights
		PointLight pointL(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.4f, 0.1f)); // color, position, attenuation
		//DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f)); // color,  direction;

		Shader lightCubeShader("light_cube.vert", "light_cube.frag");

		std::shared_ptr<Shader> textureShader = std::make_shared<Shader>("texture.vert", "texture.frag");
		std::shared_ptr<Texture> woodTexture = std::make_shared<Texture>("wood_texture.dds");
		std::shared_ptr<Material> woodTextureMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
		Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f), woodTextureMaterial);


		//PointLight pointL(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(5.0f, 15.0f, 10.0f), glm::vec3(0.2f, 0.2f, 0.1f)); // color, position, attenuation (constant, linear, quadratic)
		//PointLight pointL(glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(5.0f, 15.0f, 10.0f), glm::vec3(1.0f, 0.01f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)


		//DirectionalLight dirL(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.0f, 1.0f, 1.0f));			  // color,  direction;
		DirectionalLight dirL(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f));			  // color,  direction;

		std::vector<PointLight> pointlightArray = std::vector<PointLight>();
		PointLight pointLight1 = PointLight(glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(5.0f, 25.0f, 10.0f), glm::vec3(1.0f, 0.05f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)
		PointLight pointLight2 = PointLight(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(5.0f, 10.0f, -10.0f), glm::vec3(1.0f, 0.01f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)
		//PointLight pointLight3 = PointLight(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-5.0f, 15.0f, 10.0f), glm::vec3(1.0f, 0.01f, 0.01f)); // color, position, attenuation (constant, linear, quadratic)
		pointlightArray.push_back(pointLight1);
		pointlightArray.push_back(pointLight2);
		//pointlightArray.push_back(pointLight3);


		// Render loop
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;


		//textureShader.get()->use();
		//textureShader.get() -> setUniform("texture_diffuse", 0);
		//game->primaryShader->use();
		//game->primaryShader->setUniform("texture_diffuse", 0);

		/*****************************************************************/

			// set up vertex data (and buffer(s)) and configure vertex attributes
			// ------------------------------------------------------------------
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



		//textureShader.get() -> setUniform("texture_diffuse", 0);


		while (!glfwWindowShouldClose(window)) {

			// FPS Camera
			// per-frame time logic
			// --------------------

			double currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// input fps cam
			// -----
			processInput(window);

			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Poll events
			glfwPollEvents();

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			camera.update(int(mouse_x), int(mouse_y), _zoom, _dragging, _strafing);

			// Set per-frame uniforms
			//setPerFrameUniforms(game->primaryShader.get(), camera, dirL, pointL);
			setPerFrameUniforms(game->primaryShader.get(), camera, dirL, pointlightArray); // multiple pointlights


			setPerFrameUniforms(textureShader.get(), camera, dirL, pointL); // only used for cube
			//setPerFrameUniforms(game->modelShader.get(), camera, dirL, pointL);

			// Render
			game->update();
			game->draw();

			cube.draw();

			// visualize where the pointlight is
			/***********************************/
			lightCubeShader.use();
			lightCubeShader.setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(5.0f, 15.0f, 10.0f)); // position of the cube
			model = glm::scale(model, glm::vec3(0.4f)); // a smaller cube
			lightCubeShader.setUniform("modelMatrix", model);
			glBindVertexArray(lightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			/***********************************/


			//PhysX
			stepPhysics();

			// Compute frame time
			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;
			frames++;

			setWindowFPS(window, t_sum);

			// Swap buffers
			glfwSwapBuffers(window);
		}
	}


	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();

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


void stepPhysics()
{
	float timeStep = 1.0f / 60.0f;
	gScene->simulate(timeStep);
	gScene->fetchResults(true);
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

	//Creating scene
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	//Creating Character Controller Manager
	gCCTManager = PxCreateControllerManager(*gScene);
	//Character Controller for Player (of type capsule)
	PxCapsuleControllerDesc charDesc;
	//<fill the descriptor here>
	charDesc.position = PxExtendedVec3(-3.0f, 3.0f, 0.0f);
	charDesc.height = PxF32(0.8f);
	charDesc.radius = PxF32(0.1f);
	charDesc.contactOffset = 0.05f; //controller skin width for collisions
	charDesc.stepOffset = 0.01; //max obstacle climb height
	charDesc.slopeLimit = cosf(glm::radians(45.0f)); // max slope to walk
	charDesc.upDirection = PxVec3(0, 1, 0); // Specifies the 'up' direction
	charDesc.material = gPhysics->createMaterial(0.1f, 0.1f, 0.1f);

	gPlayerController = gCCTManager->createController(charDesc);


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
	return  PxFilterFlag::eDEFAULT;
}

void releasePhysX()
{
	gCCTManager->release();
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

void setPerFrameUniforms(Shader* shader, FPSCamera camera, DirectionalLight& dirL, PointLight& pointL) {

	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);

	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);

	for (GLuint i = 0; i < 4; i++) {
		string number = to_string(i);


	}


}

void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, PointLight& pointL)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);

	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);

}

// for multiple pointlights
void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, std::vector<PointLight> pointlightArray)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);

	// this "works" but it looks like it only draws the last pointlight
	
	for (GLuint i = 0; i < pointlightArray.size(); i++) {
		string number = std::to_string(i);
		PointLight& pointLight = pointlightArray[i];

		shader->setUniform("pointL.color", pointLight.color);
		shader->setUniform("pointL.position", pointLight.position);
		shader->setUniform("pointL.attenuation", pointLight.attenuation);
		//std::cout << "pointLights[" + number + "].attenuation" << std::endl;
		//std::cout << glm::to_string(pointLight.attenuation) << std::endl;
	}
		

	//for (GLuint i = 0; i < pointlightArray.size(); i++) {
	//	string number = std::to_string(i);
	//	PointLight& pointLight = pointlightArray[i];
	//
	//	shader->setUniform(("pointLights[" + number + "].color").c_str(), pointLight.color);
	//	shader->setUniform(("pointLights[" + number + "].position").c_str(), pointLight.position);
	//	shader->setUniform(("pointLights[" + number + "].attenuation").c_str(), pointLight.attenuation);
	//	//std::cout << "pointLights[" + number + "].attenuation" << std::endl;
	//	//std::cout << glm::to_string(pointLight.attenuation) << std::endl;
	//}

}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}

// FPS Camera 
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {

	glm::vec3 pos = camera.getPosition();
	//std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
	/*
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		std::cout << "Pressed ESC" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// reset camera to 0
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		std::cout << "Camera Position: " + glm::to_string(pos) << std::endl;
		camera.resetPosition();
	}
	*/
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	/*
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
	*/
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_zoom -= float(yoffset) * 0.5f;
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