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
//#include "physxInclude/PxPhysicsVersion.h" hm, komisch, findet er net
#include "physxInclude/pvd/PxPvd.h"
#include "physxInclude/pvd/PxPvdSceneClient.h"
#include "physxInclude/pvd/PxPvdTransport.h"
using namespace physx;
using namespace std;

//Config
#include "Configuration.h"

// Model loading
#include "Model.h"


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, PointLight& pointL);
void setWindowFPS(GLFWwindow *window,float& t_sum);
void initPhysX();
void releasePhysX();
void examplePhysX();


/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

// settings.ini
Configuration config = Configuration("assets/settings.ini");

// camera
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 6.0f;

//Game
int frames = 0;
///Game game;

//PhysX
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* gFoundation = nullptr;
static PxPhysics* gPhysics = nullptr;
static PxScene* gScene = nullptr;
static PxPvd* gPvd = nullptr;



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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set GL defaults
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Init PhysX
	initPhysX();

	// Example PhysX code
	examplePhysX();

	//Stepping PhysX
	PxReal myTimestep = 1.0f / 60.0f;


	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// Model loading
		//shared_ptr<Shader> modelShader = make_shared<Shader>("modelloading.vert", "modelloading.frag");
		Shader modelShader("modelloading.vert", "modelloading.frag");
		//Model nanosuit("assets/models/nanosuit/nanosuit.obj");
		Model nanosuit("assets/models/lego.obj");
		modelShader.use();

		// Load shader(s)
		shared_ptr<Shader> textureShader = make_shared<Shader>("texture.vert", "texture.frag");

		// Create textures
		shared_ptr<Texture> woodTexture = make_shared<Texture>("wood_texture.dds");
		shared_ptr<Texture> brickTexture = make_shared<Texture>("bricks_diffuse.dds");
		
		// Create materials
		shared_ptr<Material> woodTextureMaterial = make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
		shared_ptr<Material> brickTextureMaterial = make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, brickTexture);
		
		// Create geometry
		Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f), woodTextureMaterial);
		Geometry cylinder = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, 0.0f)), Geometry::createCylinderGeometry(32, 1.3f, 1.0f), brickTextureMaterial);
		Geometry sphere = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, 0.0f)), Geometry::createSphereGeometry(64, 32, 1.0f), brickTextureMaterial);

		// Initialize camera
		Camera camera(config.fov, float(config.width) / float(config.height), config.nearZ, config.farZ);

		// Initialize lights
		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f));
		PointLight pointL(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.4f, 0.1f));

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

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			camera.update(int(mouse_x), int(mouse_y), _zoom, _dragging, _strafing);

			// Set per-frame uniforms
			//setPerFrameUniforms(textureShader.get(), camera, dirL, pointL);

			// Render
			//cube.draw();
			//cylinder.draw();
			//sphere.draw();
			nanosuit.Draw(modelShader);


			//PhysX
			if (gScene) {
				gScene->simulate(myTimestep);
				gScene->fetchResults(true);
			}
			//Get current position of actor (box) and print it
			///PxVec3 boxPos = gBox->getGlobalPose().p;
			///cout << "Box current Position (" << boxPos.x << " " << boxPos.y << " " << boxPos.z<<")\n";

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
	// Destroy PhysX
	/* --------------------------------------------- */

	releasePhysX();

	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}

void examplePhysX() {
	//Creating material
	PxMaterial* mMaterial =
		//static friction, dynamic friction, restitution
		gPhysics->createMaterial(0.5, 0.5, 0.5);

	//1-Creating static plane
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0,
		0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysics->createRigidStatic(planePos);
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *mMaterial);
	plane->attachShape(*shape);
	gScene->addActor(*plane);

	PxRigidDynamic*gBox;
	//2) Create cube
	PxTransform boxPos(PxVec3(0.0f, 10.0f, 0.0f));
	PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
	gBox = PxCreateDynamic(*gPhysics, boxPos, boxGeometry, *mMaterial,
		1.0f);
	gScene->addActor(*gBox);
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
}

// Cleans up all PhysX objects
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