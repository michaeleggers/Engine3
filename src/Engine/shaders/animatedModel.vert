#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3    position;
layout (location = 1) in vec3    normal;
layout (location = 2) in uvec4   boneIdx;
layout (location = 3) in vec4    boneWeights;
layout (location = 4) in vec2    uv;

layout (location = 0) out vec2  out_uv;
layout (location = 1) out vec3  out_normal;

layout (set = 0, binding = 0) uniform ViewProj_t
{
    mat4  view;
    mat4  proj;
} u_view_proj;

layout (set = 0, binding = 1) uniform Data_t
{
    mat4 model;
} u_data;

// const mat4 blender2engine = mat4(
//     1, 0, 0, 0,
//     0, 0, 1, 0,
//     0, 1, 0, 0,
//     0, 0, 0, 1
// );

void main()
{
    out_uv = uv;
    out_normal = normal;
	gl_Position = u_view_proj.proj * u_view_proj.view * u_data.model * vec4(position, 1.0);
    gl_Position.y = -gl_Position.y; // Hack: vulkan's y is down
}
