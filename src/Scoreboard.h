#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "LaserRenderer.h"
#include "Program.h"

class Scoreboard
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
    void initTexture(const std::string& resourceDirectory, ImageLoader loader);
void renderCustomText( int frombot,  glm::vec3 modify_color, std::string text);
};
