#version 330 core

in vec2 tex_coords;

uniform sampler2D texture_sampler;
out vec4 frag_color;

void main()
{
	frag_color = texture(texture_sampler, tex_coords);
}