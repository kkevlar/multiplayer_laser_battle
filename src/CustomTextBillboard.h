#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "AnimTextureBillboard.h"
#include "Program.h"

class CustomTextBillboard
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
    void renderCustomText(const glm::mat4& P,
                          const glm::mat4& V,
                          glm::vec3 campos,
                          glm::vec3 position_xyz,
                          glm::vec3 modify_color,
                          std::string text);
};
