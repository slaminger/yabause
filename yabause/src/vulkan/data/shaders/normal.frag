#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision highp float;

layout (binding = 0) uniform UBO 
{
 vec4 u_color_offset;
} ubo;
layout (binding = 1) uniform sampler2D s_texture;

layout (location = 0) in highp vec4 v_texcoord;

layout (location = 0) out vec4 fragColor;

void main()
{
/*
  ivec2 addr;
  addr.x = int(v_texcoord.x);
  addr.y = int(v_texcoord.y);
  vec4 txcol = texelFetch( s_texture, addr,0 );
  if(txcol.a > 0.0)
     fragColor = clamp(txcol+ubo.u_color_offset,vec4(0.0),vec4(1.0));
  else
     discard;
*/
	 fragColor = vec4(1.0,1.0,1.0,1.0);
}