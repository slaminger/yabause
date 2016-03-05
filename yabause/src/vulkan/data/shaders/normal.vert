#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UBO 
{
	mat4 u_mvpMatrix;
} ubo;

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_texcoord;

out  highp vec4 v_texcoord;

void main()
{
	gl_Position = a_position*ubo.u_mvpMatrix;
	v_texcoord  = a_texcoord;
}
