/**
* Bakalarska praca - Simualace pevnych teles
* VUT FIT, 2018/2019
*
* Autor:	Denis Leitner, xleitn02
* Subor:	vertex.vert
*
*/

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
out vec3 fragmentPos;
out vec3 Normal;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Normal = normalMatrix * normal;
	fragmentPos =  vec3(model * vec4(aPos, 1.0));
}