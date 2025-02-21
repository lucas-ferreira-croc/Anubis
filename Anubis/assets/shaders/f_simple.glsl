#version 330

layout(location = 0) out vec4 FragColor;

uniform int is_colliding;

void main()
{
	if(is_colliding == 1)
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		FragColor = vec4(.0, 1.0, 0.0, 1.0);
}