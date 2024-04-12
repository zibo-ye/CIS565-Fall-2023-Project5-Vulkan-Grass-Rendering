
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform ModelBufferObject {
    mat4 model;
};

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 up;

struct tcs_in {
    vec4 v1;
    vec4 v2;
    vec4 up;
};

layout(location = 0) out tcs_in tcs_data;


out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = model * vec4(v0.xyz, 1.0);
    gl_Position.w = v0.w;
    tcs_data.v1 = model * vec4(v1.xyz, 1.0);
    tcs_data.v1.w = v1.w;
    tcs_data.v2 = model * vec4(v2.xyz, 1.0);
    tcs_data.v2.w = v2.w;
    tcs_data.up = model * vec4(up.xyz, 0.0);
    tcs_data.up.w = up.w;
}
