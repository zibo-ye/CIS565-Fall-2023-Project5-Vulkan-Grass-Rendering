#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(vertices = 1) out;

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    mat4 invView;
} camera;

struct tcs_in {
    vec4 v1;
    vec4 v2;
    vec4 up;
};

layout(location = 0) in tcs_in tcs_data[];

struct tes_in {
    vec4 v1;
    vec4 v2;
    vec4 up;
};

layout(location = 0) out tes_in tes_data[];

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[gl_MaxPatchVertices];

void main() {
	// Don't move the origin location of the patch
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tes_data[gl_InvocationID].v1 = tcs_data[gl_InvocationID].v1;
    tes_data[gl_InvocationID].v2 = tcs_data[gl_InvocationID].v2;
    tes_data[gl_InvocationID].up = tcs_data[gl_InvocationID].up;

    float tessellationLevel = 5; //TODO: distance based tessellation
    gl_TessLevelInner[0] = 1;
    gl_TessLevelInner[1] = tessellationLevel;
    gl_TessLevelOuter[0] = tessellationLevel;
    gl_TessLevelOuter[1] = 2;
    gl_TessLevelOuter[2] = tessellationLevel;
    gl_TessLevelOuter[3] = 2;
}
