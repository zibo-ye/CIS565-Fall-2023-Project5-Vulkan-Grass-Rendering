#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    mat4 invView;
} camera;

struct fragData {
    vec3 pos;
    vec3 normal;
    vec2 uv;
};

layout(location = 0) in fragData frag;

layout(location = 0) out vec4 outColor;

const vec3 grassColorEdge = vec3(0.10, 0.45, 0.07);
const vec3 grassColorCenter = vec3(0.22, 0.57, 0.04);

void main() {
    vec3 grassColor = mix(grassColorCenter, grassColorEdge, abs(frag.uv.x - 0.5) * 2);
    grassColor *= mix(0.5, 1.0, smoothstep(0, 0.6, frag.uv.y));
    outColor = vec4(grassColor, 1.f);
}