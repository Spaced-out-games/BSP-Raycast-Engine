#include "movable_ent.h"
#include "uBuffer.h"

class ent_camera: public ent_3d_2a
{
	public:
		// constructor

		ent_camera(
			const glm::vec3& Position = { 0.0f,0.0f,0.0f }, const glm::vec3& Target = glm::vec3(1.0f, 0.0f, 0.0f),
			const glm::vec3& Up_vector = { 0.0f,1.0f,0.0f }, const float& fov = 80.0f,
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

		// destructor
		~ent_camera();
		void update_view_matrix();
		glm::mat4 get_view_matrix();
		glm::mat4 get_perspective_matrix();
		void update_UBO();
		void bind_UBO(GLuint program); // might be nice to add a no-arg variant

		uBuffer<>& getUBO();
	private:
		glm::vec3 up;
		float fov;
		float aspect_ratio;
		float near_clip;
		float far_clip;
		uBuffer<> ubo;
		
};

// When you aren't a lazy fuckface, change this to delete the ubo. Actually, it might free it, but still check
ent_camera::~ent_camera() = default;


glm::mat4 ent_camera::get_view_matrix() { return get_matrix(); }
glm::mat4 ent_camera::get_perspective_matrix() { return glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip); }
void ent_camera::update_UBO()
{
	glm::mat4& view_matrix = glm::inverse(get_view_matrix());
	glm::mat4& perspective_matrix = get_perspective_matrix();
	ubo.updateData(0, sizeof(glm::mat4), glm::value_ptr(view_matrix));
	ubo.updateData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(perspective_matrix));
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