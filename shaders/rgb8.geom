#version 130

in vec2 pos[];
in vec2 tex[];

out vec2 gs_tex;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void main()
{
    for (int i = 0; i < gl_in.length (); ++i) {
        gs_tex = gl_in[i].gl_TexCoord;
        gl_
    }

    EndPrimitive();
}
