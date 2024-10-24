#pragma once
#include "movable_ent.h"
#include "uBuffer.h"
#include "glm_master.h"

class ent_camera: public ent_3d_2a
{
	public:
		// constructor

		ent_camera(
			const vec3& Position = { 0.0f,0.0f,0.0f }, const vec3& Target = vec3(1.0f, 0.0f, 0.0f),
			const vec3& Up_vector = { 0.0f,1.0f,0.0f }, const float& fov = 80.0f,
			const float& aspect_ratio = 16.0f / 9.0f, const float& near_clip = 0.001f,
			const float& far_clip = 1000.0f) :
				fov(fov),
				aspect_ratio(aspect_ratio),
				near_clip(near_clip),
				far_clip(far_clip),
				ubo()
		{
			update_view_matrix();
			position = Position;
			target = Target;
			up = Up_vector;
		}
		const char* get_name() const {
			return "ent_camera";
		}

		// destructor
		~ent_camera();
		void update_view_matrix();
		mat4 get_view_matrix();
		mat4 get_perspective_matrix();
		void update_UBO();
		void bind_UBO(GLuint program); // might be nice to add a no-arg variant

		// need to implement these

		virtual void init() override {};
		virtual void tick() override;
		virtual void exec(const ent_command& command) override {};




		void look_at_object(ent_3d_2a& target_object)
		{
			ent_3d_2a::look_at(target_object.get_position());
		}
//		void look_at_brush()

		uBuffer<>& getUBO();
	private:
		vec3 up;
		float fov;
		float aspect_ratio;
		float near_clip;
		float far_clip;
		uBuffer<> ubo;
		
};


// When you aren't a lazy fuckface, change this to delete the ubo. Actually, it might free it, but still check
ent_camera::~ent_camera() = default;


mat4 ent_camera::get_view_matrix() { return get_matrix(); }
mat4 ent_camera::get_perspective_matrix() { return glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip); }
void ent_camera::update_UBO()
{
	mat4 view_matrix = glm::inverse(get_view_matrix());
	mat4 perspective_matrix = get_perspective_matrix();
	ubo.set(offsetof(gl_globals,view), view_matrix);
	ubo.set(offsetof(gl_globals, projection), perspective_matrix);

	//ubo.updateData(0, sizeof(mat4), glm::value_ptr(view_matrix));
	//ubo.updateData(sizeof(mat4), sizeof(mat4), glm::value_ptr(perspective_matrix));
}

void ent_camera::tick()
{
	update_UBO();
}


void ent_camera::bind_UBO(GLuint program)
{
	GLuint uboIndex = glGetUniformBlockIndex(program, "CameraUBO");

	glUniformBlockBinding(program, uboIndex, 0);

	ubo.bindBase(0);
}
// DEPRECATED NOW - since viewMatrix is no longer stored (and is instead computed as-needed), this is a no-op. No need to assign anything
void ent_camera::update_view_matrix() {}


uBuffer<>& ent_camera::getUBO() { return ubo; }