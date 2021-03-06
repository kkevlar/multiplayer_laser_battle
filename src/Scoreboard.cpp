
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using namespace std;
using namespace glm;
#include "Scoreboard.h"
#include "Program.h"
#include "log.h"

static mat4 safe_lookat(vec3 me, vec3 target, vec3 up)
{
    mat4 m1;
    vec3 ex, ey, ez;
    ey = up;
    ez = -normalize(target - me);
    ex = cross(ey, ez);
    m1[0][0] = ex.x;
    m1[0][1] = ex.y;
    m1[0][2] = ex.z;
    m1[0][3] = 0;
    m1[1][0] = ey.x;
    m1[1][1] = ey.y;
    m1[1][2] = ey.z;
    m1[1][3] = 0;
    m1[2][0] = ez.x;
    m1[2][1] = ez.y;
    m1[2][2] = ez.z;
    m1[2][3] = 0;
    m1[3][0] = 0;
    m1[3][1] = 0;
    m1[3][2] = 0;
    m1[3][3] = 1.0f;

    return m1;
}

void Scoreboard::initGeom(const std::string& resourceDirectory)
{
    // generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // generate vertex buffer to hand off to OGL
    glGenBuffers(1, &VertexBufferID);
    // set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

    GLfloat cube_vertices[] = {
        // front
        -1.0,
        -1.0,
        1.0,  // LD
        1.0,
        -1.0,
        1.0,  // RD
        1.0,
        1.0,
        1.0,  // RU
        -1.0,
        1.0,
        1.0,  // LU
    };
    // make it a bit smaller
    for (int i = 0; i < 12; i++) cube_vertices[i] *= 0.5;
    // actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

    // we need to set up the vertex array
    glEnableVertexAttribArray(0);
    // key function to get up how many elements to pull out at a time (3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // color
    GLfloat cube_norm[] = {
        // front colors
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        1.0,

    };
    glGenBuffers(1, &VertexNormDBox);
    // set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // color
    glm::vec2 cube_tex[] = {
        // front colors
        glm::vec2(0.0, 1.0),
        glm::vec2(1.0, 1.0),
        glm::vec2(1.0, 0.0),
        glm::vec2(0.0, 0.0),

    };
    glGenBuffers(1, &VertexTexBox);
    // set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &IndexBufferIDBox);
    // set the current state to focus on our vertex buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
    GLushort cube_elements[] = {

        // front
        0,
        1,
        2,
        2,
        3,
        0,
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Scoreboard::initProgram(const std::string& resourceDirectory)
{
    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/" + "scoreboard_vertex.glsl",
                         resourceDirectory + "/" + "scoreboard_fragment.glsl");
    if (!prog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    prog->addUniform("M");
    prog->addUniform("topleft_start_coords");
    prog->addUniform("botright_end_coords");
    prog->addUniform("frames_height");
    prog->addUniform("frames_width");
    prog->addUniform("modify_color");
    prog->addUniform("numletters");
    prog->addUniform("letterselect");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
}

void Scoreboard::initTexture(const std::string& resourceDirectory, ImageLoader loader)
{
    int width, height, channels;
    char filepath[1000];

    // texture 1
    string str = resourceDirectory + "/" + "orbitron.png";
    strcpy(filepath, str.c_str());
    unsigned char* data = loader(filepath, &width, &height, &channels, 4);
    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(prog->pid);
    //[TWOTEXTURES]
    // set the 2 textures to the correct samplers in the fragment shader:
    GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");  // tex, tex2... sampler in the fragment shader
    glUseProgram(prog->pid);
    glUniform1i(Tex1Location, 0);
}

void Scoreboard::renderCustomText( int frombot,  glm::vec3 modify_color, std::string text)
{
    // Draw the box using GLSL.
    prog->bind();

    int count = 0;
    int index = 0;
    mat4 letterselect = mat4(1);
    while (count < 16 && index < text.size())
    {
        char c = text.at(index);
        if ((c >= 'a' && c <= 'z'))
        {
            letterselect[count / 4][count % 4] = ((int)c - 'a');
            count += 1;
        }
        else if ((c >= 'A' && c <= 'Z'))
        {
            letterselect[count / 4][count % 4] = ((int)c - 'A');
            count += 1;
        }
        else if (c >= '0' && c <= '9')
        {
            letterselect[count / 4][count % 4] = ((int)c - '0') + ((int)'z' - 'a') + 2;
            count += 1;
        }
        else if (c == ' ' || c == '_')
        {
            letterselect[count / 4][count % 4] = ((int)'z' - 'a') + 1;
            count += 1;
        }
        index += 1;
    }

    // send the matrices to the shaders
    glUniform2f(prog->getUniform("topleft_start_coords"), 0, 0);
    glUniform2f(prog->getUniform("botright_end_coords"), 1.0f, 1.0f);
    glUniform1f(prog->getUniform("frames_width"), 37);
    glUniform1f(prog->getUniform("frames_height"), 1);
    glUniform1i(prog->getUniform("numletters"), count);
    glUniform3fv(prog->getUniform("modify_color"), 1, &modify_color[0]);
    glUniformMatrix4fv(prog->getUniform("letterselect"), 1, GL_FALSE, &letterselect[0][0]);

    glBindVertexArray(VertexArrayID);
    // actually draw from vertex 0, 3 vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
  
    glBindTexture(GL_TEXTURE_2D, Texture);

    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.1, 1));
    glm::mat4 TransCaller = glm::translate(glm::mat4(1.0f), vec3(-0.8, -0.8 + frombot * 0.1, 0.0f));

    glm::mat4 M;
    M =  TransCaller *  S;
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
    // M =  TransZ *  rr * zzz *yyy *    S;
    // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

    glBindVertexArray(0);

    prog->unbind();
}