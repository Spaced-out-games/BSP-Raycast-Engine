#pragma once

#include "Camera.h"
#include "Controller.h"
#include <SDL.h>
#include "debug_utils.h"
//#include "bspnode.h"
class freecam : public Controller<Camera>
{
	public:
		freecam() {
			windowDimensions = nullptr;
			deltaTime = nullptr;

		}
		void init(glm::vec2* window_width_ptr, double* deltaTime_ptr){
			windowDimensions = window_width_ptr;
			deltaTime = deltaTime_ptr;

		}
		void onMouseMove(SDL_Event event) override
		{




		}

		void CheckContinuousInput()
		{
			deltaMousePos = getNormalizedMousePos() - lastMousePos;


			
			state = SDL_GetKeyboardState(nullptr);
			float dt = (float)(*deltaTime);
			Camera& target_camera = getPawn();
			glm::vec3 position = target_camera.getPosition();

			if (state[SDL_SCANCODE_W])
			{
				
				target_camera.moveForward(speed * (*deltaTime));

			}
			if (state[SDL_SCANCODE_S])
			{
				target_camera.moveForward(-speed * (*deltaTime));
			}

			if (state[SDL_SCANCODE_A])
			{
				target_camera.moveRight(speed * (*deltaTime));

			}
			if (state[SDL_SCANCODE_D])
			{
				target_camera.moveRight(-speed * (*deltaTime));

			}

			glm::vec2& mouse = getNormalizedMousePos();

			if (mouse.x < -0.5f || mouse.x > 0.5f || mouse.y < -0.5f || mouse.y > 0.5f)
			{



				SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), windowDimensions->x/2, windowDimensions->y / 2);

			}
			getPawn().rotate(deltaMousePos.y * glm::radians(90.0) * vertical_sensitivity, -deltaMousePos.x * glm::radians(90.0) * horizontal_sensitivity);

			lastMousePos = getNormalizedMousePos();

		}
		void onKeyDown(SDL_Event event) override 
		{
			switch (event.key.keysym.sym)
			{
			default:
				break;
			case SDLK_ESCAPE:
				std::cout << "exiting game";
				exit(32);
				break;
			case SDLK_LSHIFT:
				speed = 25.0;
				break;
			case SDLK_SPACE:
				clip = !clip;
				std::cout << "noclip ";
				if (clip) { std::cout << "enabled.\n"; }
				else { std::cout << "enabled.\n"; }


			}



		};
		void onKeyUp(SDL_Event event) override
		{
			switch (event.key.keysym.sym)
			{
			default:
				break;

			case SDLK_LSHIFT:
				speed = 10.0;
			}
		};

	double* deltaTime;
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	glm::vec2 lastMousePos = { 0.0,0.0 };
	glm::vec2 deltaMousePos = {0.0,0.0};
	float vertical_sensitivity = 50.0f;
	float horizontal_sensitivity = 50.0f;
	float speed = 10.0;
	glm::vec3 last_camera_pos;
	glm::vec3 delta_camera_pos;
	bool clip = false;
	// test
	//BSPNode* level_root;




};