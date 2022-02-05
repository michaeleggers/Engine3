#version 450
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_nonuniform_qualifier    : enable

layout(location = 0) in vec2  in_uv;
layout(location = 1) in vec3  in_normal;
layout(location = 0) out vec4 outColor;


void main() 
{
	outColor = vec4(in_normal, 1.0);
}
