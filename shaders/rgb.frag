#version 130

uniform sampler2D image;

in vec2 vs_tex_coord;

void main()
{
    gl_FragColor = texture (image, vs_tex_coord);
}
