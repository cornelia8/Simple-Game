#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace GameObject
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    // Create circle with given center, length, nr of triangles and color
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float ray, int triangles, glm::vec3 color, bool fill = false);

    // Create the map with given bottom left corner, length, width and color
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float length, float height, glm::vec3 color, bool fill = false);
}