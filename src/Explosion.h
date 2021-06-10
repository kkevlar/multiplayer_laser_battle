#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "Program.h"

typedef unsigned char*(ImageLoaderr)(char const*, int*, int*, int*, int);

class Explosion
{
   private:
    GLuint VertexArrayID;
    GLuint VertexBufferID;
    GLuint VertexNormDBox;
    GLuint VertexTexBox;
    GLuint IndexBufferIDBox;
    GLuint Texture;
    std::shared_ptr<Program> prog;

   public:
    void initGeom(const std::string& resDir);
    void initProgram(const std::string& resourceDirectory);
    void initTexture(const std::string& resourceDirectory, ImageLoaderr loader);
    void renderExplosion(glm::mat4& P, glm::mat4& V, glm::vec3 campos, glm::vec3 position_xyz, float time);
};
