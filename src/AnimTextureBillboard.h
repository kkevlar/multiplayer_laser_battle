#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "Program.h"

typedef unsigned char*(ImageLoader)(char const*, int*, int*, int*, int);

class AnimTextureBillboard
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
    void initProgram(const std::string& resourceDirectory,
                     const std::string& vert_shader_name,
                     const std::string& frag_shader_name);
    void initTexture(const std::string& resourceDirectory, const std::string& tex_name, ImageLoader loader);
    void renderLaser(
        glm::mat4& P, glm::mat4& V, glm::vec3 campos, glm::vec3 position_xyz, glm::vec3 where_to_look, float time);
};
