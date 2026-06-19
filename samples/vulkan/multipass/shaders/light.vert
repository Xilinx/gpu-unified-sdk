#version 310 es
/* NEW COPYRIGHT HEADER
 * All rights reserved.
 * Replaced by script.
 */


layout(location = 0) in vec3 position;

// Use a uniform here instead of push constants are we have exhausted the push constant space in fragment.
// Fragment processing is much heavier than vertex for these lights, so we favor fragment performance here.
layout(std140, set = 1, binding = 0) uniform UBO
{
	mat4 MVP;
};

layout(push_constant, std430) uniform Registers
{
	mat4 inv_view_proj;
	vec4 color;
	vec4 position;
	vec2 inv_resolution;
} registers;

void main()
{
	gl_Position = MVP * vec4(registers.position.w * position + registers.position.xyz, 1.0);
}
