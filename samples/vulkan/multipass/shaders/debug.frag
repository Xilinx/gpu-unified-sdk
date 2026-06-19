#version 310 es
/* NEW COPYRIGHT HEADER
 * All rights reserved.
 * Replaced by script.
 */


precision mediump float;

layout(location = 0) out vec4 outLight;

layout(input_attachment_index = 0, set = 0, binding = 0) uniform mediump subpassInput albedo;
layout(input_attachment_index = 1, set = 0, binding = 1) uniform highp subpassInput depth;
layout(input_attachment_index = 2, set = 0, binding = 2) uniform mediump subpassInput normals;

layout(push_constant, std430) uniform Registers
{
	mat4 inv_view_proj;
	vec4 color;
	vec4 position;
	vec2 inv_resolution;
} registers;

void main()
{
	float x = gl_FragCoord.x * registers.inv_resolution.x;
	vec4 color;
	if (x < 0.333)
		color = vec4(subpassLoad(albedo).rgb, 1.0);
	else if (x < 0.666)
		color = vec4(subpassLoad(normals).rgb, 1.0);
	else
		color = vec4(subpassLoad(depth).xxx, 1.0);
	outLight = color;
}

