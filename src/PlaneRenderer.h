#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "Program.h"

class PlaneRenderer
{
   private:
    std::shared_ptr<Program> prog;

   public:
    void initGeom(const std::string& resDir);
    void initProgram(const std::string& resourceDirectory,
                     const std::string& vert_shader_name,
                     const std::string& frag_shader_name);
    void renderLaser(glm::mat4& P,
                     glm::mat4& V,
                     glm::vec3 campos,
                     glm::vec3 position_xyz,
                     glm::vec3 where_to_look,
                     glm::vec3 modify_color,
                     float time);
};
