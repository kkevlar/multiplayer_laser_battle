#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "Program.h"
#include "Shape.h"

typedef unsigned char*(PlaneImageLoader)(char const*, int*, int*, int*, int);

class PlaneRenderer
{
   private:
    std::shared_ptr<Program> internal_plane_prog;
    std::shared_ptr<Shape> plane;

   public:
    void initGeom(const std::string& resDir, PlaneImageLoader loader);
    void initProgram(const std::string& resourceDirectory);

    void renderLaser(glm::mat4& P,
                     glm::mat4& V,
                     glm::vec3 campos,
                     glm::vec3 position_xyz,
                     glm::vec3 where_to_look,
                     glm::vec3 modify_color,
                     float time);
};
