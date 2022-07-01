/**
* Bakalarska praca - Simualace pevnych teles
* VUT FIT, 2018/2019
*
* Autor:	Denis Leitner, xleitn02
* Subor:	fragment.frag
*
*/

#version 330 core

out vec4 FragColor;

in vec3 fragmentPos;
in vec3 Normal;
 
const vec3 ligthPos = vec3(-100, 200, 250);
const vec3 colorOrange = vec3(1.0f, 0.5f, 0.2f);
const vec3 colorDarkGrey = vec3(0.66f, 0.66f, 0.66f);
const vec3 colorLightGray = vec3(0.824f, 0.824f, 0.824f);

uniform vec3 color;

void main()
{
	vec3 materialColor = color;							// color of the material
	vec3 lightColor = vec3(1.0, 1.0, 1.0);				// color of the light (White)

	vec3 lightDir = ligthPos - fragmentPos;

	vec3 L = normalize(lightDir);						// light direction
	float diff = max(dot(Normal, L), 0.0);

	vec3 diffuse = lightColor * diff;

	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	FragColor = vec4(materialColor * (diffuse + ambient), 1.0);
}