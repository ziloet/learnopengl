#version 330 core

in vec3 PixelColor;
out vec4 FragmentColor;

in vec2 TexCoords;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
	vec2 InvertedTexCoords = vec2(1 - TexCoords.x, TexCoords.y);
	vec4 Color1 = texture(Texture1, TexCoords);
	vec4 Color2 = texture(Texture2, InvertedTexCoords);
	FragmentColor = mix(Color1, Color2, 0.2);
}