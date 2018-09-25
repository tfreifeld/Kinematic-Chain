#version 130

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 color0;

uniform vec3 lightDirection;
uniform sampler2D texture;

void main()
{
    vec3 tmp = dot(-lightDirection, normal0) * color0 ;
    if (texCoord0.x < 0.05 || texCoord0.x > 0.95
    	|| texCoord0.y < 0.05 || texCoord0.y > 0.95)
    	tmp = vec3(0,0,0);
	gl_FragColor =  texture2D (texture, texCoord0);//clamp(vec4(tmp,1.0), 0.0, 1.0) * texture2D (texture, texCoord0);

}
