
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using namespace std;
using namespace glm;
#include "PlaneRenderer.h"
#include "Program.h"
#include "Shape.h"

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

    void PlaneRenderer::initGeom(const std::string& resDir, PlaneImageLoader loader)
{

        plane = make_shared<Shape>();
        string mtldir = resDir + "/FA-18E_SuperHornet/";
        plane->loadMesh(resDir + "/FA-18E_SuperHornet/FA-18E_SuperHornet.obj", &mtldir, loader);

        plane->resize();
        plane->init();
}

void PlaneRenderer::initProgram(const std::string& resourceDirectory)
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
//TODO TODO
        // pplane->bind();
        // glUniformMatrix4fv(pplane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        // glUniformMatrix4fv(pplane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        // glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        // glUniform3fv(pplane->getUniform("campos"), 1, &mycam.pos[0]);
        // glUniform3fv(pplane->getUniform("tint_color"), 1, &my_allocated_color_from_server[0]);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Texture2);
        // plane->draw(pplane);  // render!!!!!!

        // const auto estimates = network.GiveOtherPlaneEstimates(glfwGetTime());
        // for (const auto& estimate : estimates)
        // {
        //     translate_plane = translate(mat4(1), estimate.pos);
        //     plane_overall_rot = mat4(quat(estimate.rot));

        //     M = translate_plane * plane_overall_rot * scale_plane;
        //     glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        //     glUniform3fv(pplane->getUniform("tint_color"), 1, &estimate.color[0]);
        //     plane->draw(pplane);  // render!!!!!!
        // }

        // draw the bots

//TODO TODO
        // for (int i = 0; i < thebots.size(); i++)
        // {
        //     // scale_plane = scale(mat4(1), vec3(10));
        //     rotate_plane = safe_lookat(thebots[i].pos, thebots[i].pos + thebots[i].forward, thebots[i].up);
        //     translate_plane = translate(mat4(1), thebots[i].pos);
        //     plane_overall_rot = rotate_plane * rotate_default_plane;
        //     M = translate_plane * plane_overall_rot * scale_plane;

        //     glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        //     glUniform3fv(pplane->getUniform("tint_color"), 1, &my_allocated_color_from_server[0]);
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, Texture2);
        //     plane->draw(pplane);  // render!!!!!!
        // }

        // pplane->unbind();
}