#version 330 core

layout(location = 0) in vec3 worldPos_;
layout(location = 1) in uint cubeType_;
layout(location = 2) in uint textureIndex_;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    uint cubeType;
    uint texIndex;
    mat4 vp;
    mat4 m;
} vs_out;

void main() {
    vs_out.texIndex = textureIndex_;
    vs_out.cubeType = cubeType_;
    
    mat4 m = mat4(1);
    m[3][0] = worldPos_.x - 1;
    m[3][1] = worldPos_.y - 1;
    m[3][2] = worldPos_.z - 1;
    vs_out.m = m;

    vs_out.vp = projection * view;
}