#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform vec3 campos;
uniform vec3 modify_color;
uniform vec2 topleft_start_coords;
uniform vec2 botright_end_coords;
uniform float frames_height;
uniform float frames_width;
uniform float frame_select;
uniform int numletters;
uniform mat4 letterselect;

uniform sampler2D tex;
uniform sampler2D tex2;

vec4 bruhmoment(vec2 incoming_tex_corods, int row, int col, vec3 color)
{
    vec2 diff = botright_end_coords - topleft_start_coords;
    vec2 bruh = vec2(diff.x * incoming_tex_corods.x / frames_width, diff.y * incoming_tex_corods.y / frames_height);
    bruh += vec2(col * diff.x / frames_width, row * diff.y / frames_height);
    bruh += topleft_start_coords;

    vec4 tcol;
    tcol = texture(tex, bruh);

    if (tcol.a < 0.2) tcol.rgba = vec4(0);
    if (tcol.r > 0.01 || tcol.g > 0.01 || tcol.r > 0.01)
        tcol.rgba = vec4(0);
    else
    {
        tcol.rgb = color;
    }
    return tcol;
}

vec4 wetterbruhmoment(vec2 incoming_tex_corods, int row, int col)
{
    vec4 buster = bruhmoment(incoming_tex_corods, row, col, modify_color);

    return buster;
}

void main()
{
    vec3 n = normalize(vertex_normal);
    vec3 lp = vec3(10, -20, -100);
    vec3 ld = normalize(vertex_pos - lp);
    float diffuse = dot(n, ld);

    int treatframe = int(frame_select);

    vec4 oof;
    float diff = (1.0f / numletters);

    int rindex = 0;
    int cindex = 0;
    for (int i = 0; i < numletters; i++)
    {
        float cut = (i + 1) * diff;

        if (vertex_tex.x < cut)
        {
            int frame = int(letterselect[rindex][cindex]);
            int row = frame / int(frames_width);
            int col = frame % int(frames_width);
            oof = wetterbruhmoment(vec2((vertex_tex.x - (cut - diff)) * numletters, vertex_tex.y), row, col);
            break;
        }

        cindex += 1;
        if (cindex >= 4)
        {
            cindex -= 4;
            rindex += 1;
        }
    }

    if (oof.a > 0.1)
    oof.rgb = modify_color;

    color = oof;
}
