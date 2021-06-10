#pragma once

#include <glad/glad.h>
#include <stdbool.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

#include "CustomTextBillboard.h"
#include "Scoreboard.h"
#include "Program.h"
#include "Shape.h"

typedef unsigned char*(PlaneImageLoader)(char const*, int*, int*, int*, int);

class PlaneRenderer
{
   private:
    std::shared_ptr<Program> internal_plane_prog;
    std::shared_ptr<Shape> plane;
    bool init_geom = false;
    bool init_program = false;

   public:
    PlaneRenderer();
    void initGeom(const std::string& resDir, PlaneImageLoader loader);
    void initProgram(const std::string& resourceDirectory);

    void renderAirplane(glm::mat4& P,
                        glm::mat4& V,
                        glm::vec3 position_xyz,
                        glm::quat plane_rot_must_include_default_rotation,
                        glm::vec3 campos,
                        glm::vec3 tint_color,
                        std::string badge_text,
                        CustomTextBillboard* customtext,
                        bool textonly
                        );
};
