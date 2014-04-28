#version 130

in vec4 pos;
in vec2 tex;

out vec2 vs_tex_coord;

void main()
{
    gl_Position = pos;
    vs_tex_coord = tex;
}
