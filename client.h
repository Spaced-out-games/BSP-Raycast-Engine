#pragma once
#include "ent_camera.h"
#include "ent_controller.h"
#include "level_editor_controller.h"

struct Client
{
	level_editor_controller controller;
	ent_camera* camera;
	Client()
	{
		
	}
};