#pragma once
#include "level_editor.h"
#include "ent_controller.h"
#include "level_editor_controller.h"
// #include "level_editor_controller.h"

template <typename app_t = level_editor_app, typename controller_t = level_editor_controller, typename camera_t = ent_camera>
struct client
{
	controller_t controller;
	camera_t camera;
	client(): camera(), controller(&camera) {}
};
