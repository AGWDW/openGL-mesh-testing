#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 vp;
    mat4 m;
} vs_out[];

flat out uint blockColourIndex;

const float size = 0.5;
const float inv_size = 1.0 / size;

vec3 vertices[8] = vec3[](
	vec3(-size,-size,-size), // 0
	vec3( size,-size,-size), // 1
	vec3(-size,-size, size), // 2
	vec3( size,-size, size), // 3

	vec3(-size, size,-size), // 4
	vec3( size, size,-size), // 5
	vec3( size, size, size), // 6
	vec3(-size, size, size)  // 7
);

int indices[] = int[](
    3, 2, 6, 7, // +z
    5, 4, 1, 0, // -z
    0, 4, 2, 7, // -x
    1, 3, 5, 6, // +x
    6, 7, 5, 4, // +y
    0, 2, 1, 3  // -y
);


void main() {
    blockColourIndex = vs_out[0].blockColourIndex;
    if(blockColourIndex != 5u) {
        return; // discards if not water
    }
    for (uint j = 0u; j < 4u; j++){ 
        int l = indices[vs_out[0].cubeType * 4u + j];
        vec3 v = vertices[l];

        gl_Position = vs_out[0].vp * vs_out[0].m * vec4(v, 1);
        EmitVertex();
    }
    EndPrimitive();
}