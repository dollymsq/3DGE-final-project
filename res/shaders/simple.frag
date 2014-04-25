#version 150

uniform sampler2D texture2D;

in V_OUT
{
    vec2 texcoord;
} v_in;

out vec4 color;

void main(void)
{
    color = vec4(1.0, 0, 0, 1.0);
}
