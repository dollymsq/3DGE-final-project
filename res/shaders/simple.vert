#version 150
#extension GL_ARB_explicit_attrib_location : enable
uniform mat4 modelView;
uniform mat4 projection;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out V_OUT
{
    vec2 texcoord;
} f_out;

void main(void)
{
    f_out.texcoord = texcoord;

    gl_Position = projection * modelView * vec4(position, 1.0);
}
