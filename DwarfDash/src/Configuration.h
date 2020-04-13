#pragma once

#include "INIReader.h"

class Configuration
{

public:

	Configuration(string path) {
		INIReader reader("assets/settings.ini"); ///simply path??
		width = reader.GetInteger("window", "width", 800);
		height = reader.GetInteger("window", "height", 800);
		refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
		this->antialiasing = (int)reader.GetInteger("window", "antialiasing", 4);
		this->brightness = (float)reader.GetReal("window", "brightness", 1.f);
		this->fullscreen = reader.GetBoolean("window", "fullscreen", false);
		this->fov = (float)reader.GetReal("camera", "fov", 60.f);
		this->nearZ = (float)reader.GetReal("camera", "near", 0.1f);
		this->farZ = (float)reader.GetReal("camera", "far", 100.f);
		this->title = reader.Get("window", "title", "Dwarf Dash");
	}

	~Configuration() {};

	int width;
	int height;
	int refresh_rate;
	int antialiasing;
	float brightness;
	float fov;
	float nearZ;
	float farZ;
	std::string title;
	bool fullscreen = false;
	bool wireframe = false;
	bool culling = true;
	bool dragging = false;
	bool strafing = false;
	float zoom = 6.0f;
};

//Declare global and initialize
//extern Configuration config = Configuration("assets/settings.ini");