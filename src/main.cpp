#include <vulkan/vulkan.hpp>
#include "Instance.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Image.h"
#include "utilities/ArgsParser.hpp"
#include "GlobalDef.h"

Device* device;
SwapChain* swapChain;
Renderer* renderer;
Camera* camera;

ApplicationArgs args;
namespace {
    void resizeCallback(GLFWwindow* window, int width, int height) {
        if (width == 0 || height == 0) return;

        vkDeviceWaitIdle(device->GetVkDevice());
        swapChain->Recreate();
        renderer->RecreateFrameResources();
    }

    bool leftMouseDown = false;
    bool rightMouseDown = false;
    double previousX = 0.0;
    double previousY = 0.0;

    void mouseDownCallback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                leftMouseDown = true;
                glfwGetCursorPos(window, &previousX, &previousY);
            }
            else if (action == GLFW_RELEASE) {
                leftMouseDown = false;
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                rightMouseDown = true;
                glfwGetCursorPos(window, &previousX, &previousY);
            }
            else if (action == GLFW_RELEASE) {
                rightMouseDown = false;
            }
        }
    }

    void mouseMoveCallback(GLFWwindow* window, double xPosition, double yPosition) {
        if (leftMouseDown) {
            double sensitivity = 0.5;
            float deltaX = static_cast<float>((previousX - xPosition) * sensitivity);
            float deltaY = static_cast<float>((previousY - yPosition) * sensitivity);

            camera->UpdateOrbit(deltaX, deltaY, 0.0f);

            previousX = xPosition;
            previousY = yPosition;
        } else if (rightMouseDown) {
            double deltaZ = static_cast<float>((previousY - yPosition) * 0.05);

            camera->UpdateOrbit(0.0f, 0.0f, deltaZ);

            previousY = yPosition;
        }
    }


    void ParseArguments(const Utility::ArgsParser& argsParser)
	{
        argsParser.PrintArgs();
		auto windowSizeArg = argsParser.GetArg("drawing-size");
		if (windowSizeArg.has_value()) {
			args.windowSize.first = std::stoi(windowSizeArg.value()[0]);
			args.windowSize.second = std::stoi(windowSizeArg.value()[1]);
		}

		auto modeArg = argsParser.GetArg("mode"); // "tess" or "mesh"
		if (modeArg.has_value()) {
			args.mode = modeArg.value()[0];
		}

		auto measureArg = argsParser.GetArg("measure");
		if (measureArg.has_value()) {
			args.measure = true;
		}

		auto limitFPSArg = argsParser.GetArg("limitfps");
		if (limitFPSArg.has_value()) {
			args.limitFPS = true;
		}

		auto bladeNumArg = argsParser.GetArg("bladeNumExponent");
		if (bladeNumArg.has_value()) {
			args.bladeNumExponent = std::stoi(bladeNumArg.value()[0]);
			args.bladeNum = 1 << args.bladeNumExponent;
		}

		auto enableMeshShaderComputeArg = argsParser.GetArg("enableMeshShaderCompute");
		if (enableMeshShaderComputeArg.has_value()) {
			args.enableMeshShaderCompute = true;
		}

        auto cameraArg = argsParser.GetArg("camera_arg");
        if (cameraArg.has_value()) {
			args.camera_distance = std::stof(cameraArg.value()[0]);
			args.camera_theta = std::stof(cameraArg.value()[1]);
			args.camera_phi = std::stof(cameraArg.value()[2]);
		}
	}

	void Measure()
	{
		static std::vector<float> frameTimes;
		static auto lastFrameTime = std::chrono::high_resolution_clock::now();
		static auto lastOutputTime = std::chrono::high_resolution_clock::now();
		static float deltaOutputTime = 0.0f;
		auto currentTime = std::chrono::high_resolution_clock::now();
		float frameTimeInMicrosec = std::chrono::duration<float, std::chrono::microseconds::period>(currentTime - lastFrameTime).count();
		deltaOutputTime += frameTimeInMicrosec;

		frameTimes.push_back(frameTimeInMicrosec);

		// Calculate statistics every second or every N frames
		if (deltaOutputTime >= 1000000.f) { // N is the number of frames after which you want to calculate statistics
			float averageFrameTime = deltaOutputTime / frameTimes.size();
			float fps = 1000000.0f / averageFrameTime;

			// Sort frame times for percentile calculations
			std::sort(frameTimes.begin(), frameTimes.end());
			float p99 = frameTimes.at(std::lround(frameTimes.size() * 0.99) - 1);
			float p95 = frameTimes.at(std::lround(frameTimes.size() * 0.95) - 1);
			float p90 = frameTimes.at(std::lround(frameTimes.size() * 0.90) - 1);

			// Calculate standard deviation
			float sumOfSquaredDifferences = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0f,
				[averageFrameTime](float acc, float ft) { return acc + (ft - averageFrameTime) * (ft - averageFrameTime); });
			float std_dev = std::sqrt(sumOfSquaredDifferences / frameTimes.size());

			std::cout << "FPS: " << fps << ", Avg Frame Time: " << averageFrameTime
				<< "us , P99: " << p99 << "us , P95: " << p95 << "us , P90: " << p90
				<< "us , Std Dev: " << std_dev << std::endl;

			frameTimes.clear(); // Reset for next batch
			deltaOutputTime = 0.0f;
		}
		lastFrameTime = std::chrono::high_resolution_clock::now(); // To eliminate the time taken to print statistics from the next batch
	}
}

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(int argc, const char* argv[]) {
    auto argsParser = Utility::ArgsParser(argc, argv);
	ParseArguments(argsParser);

    static constexpr char* applicationName = "Vulkan Grass Rendering";
    InitializeWindow(args.windowSize.first, args.windowSize.second, applicationName);

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // add_instance_extension
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;

    Instance* instance = new Instance(applicationName, glfwExtensionCount, glfwExtensions);

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance->GetVkInstance(), GetGLFWWindow(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }

    instance->PickPhysicalDevice(
        { 
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			VK_EXT_MESH_SHADER_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
        }, 
        QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit | QueueFlagBit::ComputeBit | QueueFlagBit::PresentBit, surface);

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.tessellationShader = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceMeshShaderFeaturesEXT enabledMeshShaderFeatures = {};
	enabledMeshShaderFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
	enabledMeshShaderFeatures.meshShader = VK_TRUE;
	enabledMeshShaderFeatures.taskShader = VK_TRUE;

    device = instance->CreateDevice(QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit | QueueFlagBit::ComputeBit | QueueFlagBit::PresentBit, deviceFeatures, &enabledMeshShaderFeatures);

    swapChain = device->CreateSwapChain(surface, 5);

    camera = new Camera(device, float(args.windowSize.first) / float(args.windowSize.second));

    VkCommandPoolCreateInfo transferPoolInfo = {};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.queueFamilyIndex = device->GetInstance()->GetQueueFamilyIndices()[QueueFlags::Transfer];
    transferPoolInfo.flags = 0;

    VkCommandPool transferCommandPool;
    if (vkCreateCommandPool(device->GetVkDevice(), &transferPoolInfo, nullptr, &transferCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    VkImage grassImage;
    VkDeviceMemory grassImageMemory;
    Image::FromFile(device,
        transferCommandPool,
        "images/grass.jpg",
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        grassImage,
        grassImageMemory
    );

    float planeDim = 15.f;
    float halfWidth = planeDim * 0.5f;
    Model* plane = new Model(device, transferCommandPool,
        {
            { { -halfWidth, 0.0f, halfWidth }, { 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
            { { halfWidth, 0.0f, halfWidth }, { 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
            { { halfWidth, 0.0f, -halfWidth }, { 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
            { { -halfWidth, 0.0f, -halfWidth }, { 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
        },
        { 0, 1, 2, 2, 3, 0 }
    );
    plane->SetTexture(grassImage);
    
    Blades* blades = new Blades(device, transferCommandPool, planeDim);

    vkDestroyCommandPool(device->GetVkDevice(), transferCommandPool, nullptr);

    Scene* scene = new Scene(device);
    scene->AddModel(plane);
    scene->AddBlades(blades);

    renderer = new Renderer(device, swapChain, scene, camera);

    glfwSetWindowSizeCallback(GetGLFWWindow(), resizeCallback);
    glfwSetMouseButtonCallback(GetGLFWWindow(), mouseDownCallback);
    glfwSetCursorPosCallback(GetGLFWWindow(), mouseMoveCallback);

    while (!ShouldQuit()) {
        glfwPollEvents();
        Measure();
        scene->UpdateTime();
        renderer->Frame();
    }

    vkDeviceWaitIdle(device->GetVkDevice());

    vkDestroyImage(device->GetVkDevice(), grassImage, nullptr);
    vkFreeMemory(device->GetVkDevice(), grassImageMemory, nullptr);

    delete scene;
    delete plane;
    delete blades;
    delete camera;
    delete renderer;
    delete swapChain;
    delete device;
    delete instance;
    DestroyWindow();
    return 0;
}
