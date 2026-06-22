#version 310 es


layout (local_size_x = 8) in;
layout (std140, binding = 0) buffer Data {
    float data[];
};

void main()
{
    data[gl_GlobalInvocationID.x] = 1.0;
}
