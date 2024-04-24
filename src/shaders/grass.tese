#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(quads, equal_spacing, ccw) in;

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    mat4 invView;
} camera;

struct tes_in {
    vec4 v1;
    vec4 v2;
    vec4 up;
};

layout(location = 0) in tes_in tes_data[];

struct fragData {
    vec3 pos;
    vec3 normal;
    vec2 uv;
};

layout(location = 0) out fragData frag;

float tParabola(float u, float v) {
    return (u * 2 - 1) * (1 - v * v);
}

void main() {
    const float u = gl_TessCoord.x;
    const float v = gl_TessCoord.y;
    
    const vec3 v0 = gl_in[0].gl_Position.xyz;
    const float theta = gl_in[0].gl_Position.w;

    const vec3 v1 = tes_data[0].v1.xyz;
    const vec3 v2 = tes_data[0].v2.xyz;
    const vec3 up = tes_data[0].up.xyz;
    const float w = tes_data[0].v2.w; // width
    
    const vec3 a = mix(v0,v1,v); //when v=1, a=v1
    const vec3 b = mix(v1,v2,v); //when v=1, b=v2
    const vec3 c = mix(a,b,v);   //when v=1, c=b=v2
    const float t = tParabola(u, v); //the shape, [-(1-v^2),1-v^2] for v in [0,1], when v=1, t=0; when v=0, t=[-1,1] for u in [0,1]

    const vec3 t1 = vec3(cos(theta), 0, sin(theta));  // bitangent (blade width)

    const vec3 pos = c + w * t1 * t;

    gl_Position = camera.proj * camera.view * vec4(pos, 1.0);

    frag.pos = pos;
    const vec3 t0 = normalize(b - a);                 // tangent (blade height)
    frag.normal = normalize(cross(t0, t1));          // normal calculation
    frag.uv = vec2(u, v);
}