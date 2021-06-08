
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using namespace std;
using namespace glm;
#include "PlaneRenderer.h"
#include "Program.h"

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

void PlaneRenderer::initGeom(const std::string& resourceDirectory)
{

        plane = make_shared<Shape>();
        string mtldir = resourceDirectory + "/FA-18E_SuperHornet/";
        plane->loadMesh(resourceDirectory + "/FA-18E_SuperHornet/FA-18E_SuperHornet.obj", &mtldir, stbi_load);

        plane->resize();
        plane->init();
}

void PlaneRenderer::initProgram(const std::string& resourceDirectory,)
{
        internal_plane_prog = std::make_shared<Program>();
        internal_plane_prog->setVerbose(true);
        internal_plane_prog->setShaderNames(resourceDirectory + "/plane_vertex.glsl", resourceDirectory + "/plane_frag.glsl");
        if (!internal_plane_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        internal_plane_prog->addUniform("P");
        internal_plane_prog->addUniform("V");
        internal_plane_prog->addUniform("M");
        internal_plane_prog->addUniform("campos");
        internal_plane_prog->addUniform("tint_color");
        internal_plane_prog->addAttribute("vertPos");
        internal_plane_prog->addAttribute("vertNor");
        internal_plane_prog->addAttribute("vertTex");
}

void PlaneRenderer::renderLaser(glm::mat4& P,
                                       glm::mat4& V,
                                       glm::vec3 campos,
                                       glm::vec3 position_xyz,
                                       glm::vec3 where_to_look,
                                       glm::vec3 modify_color,
                                       float time)
{
    // Draw the box using GLSL.
    prog->bind();

    // send the matrices to the shaders
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniform3fv(prog->getUniform("campos"), 1, &campos[0]);
    glUniform3fv(prog->getUniform("modify_color"), 1, &modify_color[0]);
    glUniform2f(prog->getUniform("topleft_start_coords"), 0, 0);
    glUniform2f(prog->getUniform("botright_end_coords"), 1.0f, 1.0f);
    glUniform2f(prog->getUniform("ratio_texslice_show"), time * 14, 1.0f);
    glUniform1f(prog->getUniform("frames_width"), 1);
    glUniform1f(prog->getUniform("frames_height"), 11);
    glUniform1f(prog->getUniform("frame_select"), time * 10);

    glBindVertexArray(VertexArrayID);
    // actually draw from vertex 0, 3 vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    mat4 Vi = glm::transpose(V);
    Vi[0][3] = 0;
    Vi[1][3] = 0;
    Vi[2][3] = 0;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    float scaleydiff = sin(time * 50);
    vec2 scalexy = vec2(40, 2.5 + 1 * scaleydiff);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scalexy.x, scalexy.y, 1));
    glm::mat4 TransCaller = glm::translate(glm::mat4(1.0f), position_xyz);

    glm::mat4 rr = safe_lookat(position_xyz, where_to_look, vec3(0, 1, 0));

    glm::mat4 yyy = rotate(mat4(1), 3.14159265f / 2, vec3(0, -1, 0));
    float xrot = 3.14159265 / 2.0f;
    glm::mat4 xxx = rotate(mat4(1), xrot, vec3(1, 0, 0));

    glm::mat4 M;
    M = TransCaller * rr * yyy * xxx * S;
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
    // M =  TransZ *  rr * zzz *yyy *    S;
    // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

    glBindVertexArray(0);

    prog->unbind();
}