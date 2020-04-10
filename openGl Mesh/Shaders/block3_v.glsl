#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texCoords;
layout(location = 3) in mat4 model;

out vec3 Normal;
out vec3 FragPos;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    FragPos = vec3(model * vec4(position, 1));
    gl_Position = projection * view * model * vec4(position, 1);
	Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}  