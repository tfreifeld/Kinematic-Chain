#version 130

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 color0;

uniform vec3 cubeColor;
uniform sampler2D texture;

void main()
{

	gl_FragColor =  clamp(vec4(cubeColor,1.0), 0.0, 1.0);
}
