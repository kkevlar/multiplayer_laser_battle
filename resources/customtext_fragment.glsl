#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform vec3 campos;
uniform vec3 modify_color;
uniform vec2 topleft_start_coords;
uniform vec2 botright_end_coords;
uniform vec2 ratio_texslice_show;
uniform float frames_height;
uniform float frames_width;
uniform float frame_select;

uniform sampler2D tex;
uniform sampler2D tex2;

vec4 bruhmoment(vec2 incoming_tex_corods, int frame)
{
    int row = frame / int(frames_width);
    int col = frame % int(frames_width);

    vec2 diff = botright_end_coords - topleft_start_coords;
    vec2 bruh = vec2(diff.x * incoming_tex_corods.x / frames_width, diff.y * incoming_tex_corods.y / frames_height);
    bruh += vec2(col * diff.x / frames_width, row * diff.y / frames_height);
    bruh += topleft_start_coords;

    vec4 tcol;
    if (bruh.x > ratio_texslice_show.x || bruh.y > ratio_texslice_show.y)
    {
        tcol = vec4(0, 0, 0, 0);
    }
    else
    {
        tcol = texture(tex, bruh);
    }

    if (tcol.a < 0.2) tcol.rgba = vec4(0);
    if (
        tcol.r > 0.01 ||
        tcol.g > 0.01 ||
        tcol.r > 0.01
         ) tcol.rgba = vec4(0);
    return tcol;
}

void main()
{
    vec3 n = normalize(vertex_normal);
    vec3 lp = vec3(10, -20, -100);
    vec3 ld = normalize(vertex_pos - lp);
    float diffuse = dot(n, ld);

    int treatframe = int(frame_select);

    vec4 oof;
    if (vertex_tex.x < 0.5 && vertex_tex.y < 0.5)
    oof = bruhmoment(vertex_tex*2, 0);
   else if (vertex_tex.x < 0.5)
    oof = bruhmoment((vertex_tex-vec2(0,0.5))*2 , 1);
   else if (vertex_tex.y < 0.5)
    oof = bruhmoment((vertex_tex-vec2(0.5,0.0))*2, 2);
   else 
    oof = bruhmoment((vertex_tex-vec2(0.5,0.5))*2, 3);

    float c = oof.r;

    oof.r = (oof.b);
    oof += vec4(vec3(modify_color * c), 0);

    color = oof;
}
