#include "PlaneRenderer.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using namespace std;
using namespace glm;
#include "Program.h"
#include "Shape.h"
#include "log.h"

PlaneRenderer::PlaneRenderer()
{
    this->init_geom = false;
    this->init_program = false;
}

void PlaneRenderer::initGeom(const std::string& resDir, PlaneImageLoader loader)
{
    if (!this->init_program)
    {
        log_fatal("Bad user");
        exit(2);
    }
    if (this->init_geom)
    {
        log_fatal("Bad user");
        exit(2);
    }

    plane = make_shared<Shape>();
    string mtldir = resDir + "/FA-18E_SuperHornet/";
    plane->loadMesh(resDir + "/FA-18E_SuperHornet/FA-18E_SuperHornet.obj", &mtldir, loader);

    plane->resize();
    plane->init();

    this->init_geom = true;
}

void PlaneRenderer::initProgram(const std::string& resourceDirectory)
{
    if (this->init_program)
    {
        log_fatal("bad user");
        exit(3);
    }
    if (this->init_geom)
    {
        log_fatal("bad user");
        exit(3);
    }
    internal_plane_prog = std::make_shared<Program>();
    internal_plane_prog->setVerbose(true);
    internal_plane_prog->setShaderNames(resourceDirectory + "/plane_vertex.glsl",
                                        resourceDirectory + "/plane_frag.glsl");
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

    this->init_program = true;
}

void PlaneRenderer::renderAirplane(glm::mat4& P,
                                   glm::mat4& V,
                                   glm::vec3 position_xyz,
                                   glm::quat plane_rot_must_include_default_rotation,
                                   glm::vec3 campos,
                                   glm::vec3 tint_color,
                                   std::string badge_text,
                                   CustomTextBillboard* customtext,
                                   bool textonly
                                   )
{
    internal_plane_prog->bind();

    mat4 scale_plane = scale(mat4(1), vec3(10));
    mat4 translate_plane = translate(mat4(1), position_xyz);
    mat4 plane_overall_rot = mat4(plane_rot_must_include_default_rotation);
    mat4 M = translate_plane * plane_overall_rot * scale_plane;

    glUniformMatrix4fv(internal_plane_prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(internal_plane_prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(internal_plane_prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
    glUniform3fv(internal_plane_prog->getUniform("campos"), 1, &campos[0]);
    glUniform3fv(internal_plane_prog->getUniform("tint_color"), 1, &tint_color[0]);
    glActiveTexture(GL_TEXTURE0);
    plane->draw(internal_plane_prog);
    internal_plane_prog->unbind();

    customtext->renderCustomText(P, V, campos, position_xyz + vec3(0, 5, 0), tint_color, badge_text);
}