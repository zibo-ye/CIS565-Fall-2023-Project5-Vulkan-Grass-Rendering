#pragma once

#include <string>
#include <optional>
#include <utility>

struct ApplicationArgs {
    //std::string scenePath;
    //std::optional<std::string> cameraName;
    //std::optional<std::string> physicalDeviceName;
    std::pair<int, int> windowSize = { 1920,1080 };
    std::optional<std::string> mode;
    //std::optional<std::string> headlessEventsPath;
    bool measure = false;
    bool limitFPS = false;
	int bladeNumExponent = 13; // 2^13 = 8192
	int bladeNum = 1<<13; // 2^13 = 8192
	float camera_distance = 20.0f;
	float camera_theta = 0.0f;
	float camera_phi = -90.f;
    bool enableMeshShaderCompute = false;
    //bool headlessIgnoreSaveFrame = false;
};

extern ApplicationArgs args;