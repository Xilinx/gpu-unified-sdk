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
	// Load G-Buffer albedo from tile buffer.
	vec4 color = subpassLoad(albedo);

	// Load depth from tile buffer and reconstruct world position.
	vec4 clip = vec4(gl_FragCoord.xy * registers.inv_resolution * 2.0 - 1.0, subpassLoad(depth).x, 1.0);
	highp vec4 world_w = registers.inv_view_proj * clip;
	highp vec3 world = world_w.xyz / world_w.w;

	// Load normal from tile buffer.
	vec3 normal = 2.0 * subpassLoad(normals).xyz - 1.0;

	// Basic point light shading.
	vec3 world_to_light = registers.position.xyz - world;
	float dist = length(world_to_light);
	float atten = 1.0 - smoothstep(0.5 * registers.position.w, registers.position.w, dist);
	world_to_light = normalize(world_to_light);
	float ndotl = clamp(dot(normal, world_to_light), 0.0, 1.0);

	outLight = ndotl * atten * registers.color * color;
}
