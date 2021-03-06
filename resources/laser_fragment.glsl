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

vec4 bruhmoment(int frame)
{
    int row = frame / int(frames_width);
    int col = frame % int(frames_width);

    vec2 diff = botright_end_coords - topleft_start_coords;
    vec2 bruh = vec2(diff.x * vertex_tex.x / frames_width, diff.y * vertex_tex.y / frames_height);
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
    return tcol;
}

void main()
{
    vec3 n = normalize(vertex_normal);
    vec3 lp = vec3(10, -20, -100);
    vec3 ld = normalize(vertex_pos - lp);
    float diffuse = dot(n, ld);

    int treatframe = int(frame_select);
    float interp = frame_select - int(frame_select);

    vec4 oof;
    oof = (1 - interp) * bruhmoment(treatframe % int(frames_height * frames_width));
    oof += (interp)*bruhmoment((treatframe + 1) % int(frames_height * frames_width));

    float c = oof.r;

    oof.r = (oof.b);
    oof += vec4(vec3(modify_color * c), 0);

    color = oof;
}
