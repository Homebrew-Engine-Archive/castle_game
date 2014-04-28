uniform sampler2DArray indexed;
uniform sampler1DArray palette;
uniform int palette_index;
uniform int texture_index;

in vec2 vs_tex_coord;

void main()
{
    vec3 atlas_coord = vec3(vs_tex_coord, texture_index);
    gl_FragColor = texture(palette, vec2(texture(indexed, atlas_coord).r, palette_index));
}
