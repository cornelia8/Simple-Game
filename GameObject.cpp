#include "GameObject.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* GameObject::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* GameObject::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float ray,
    int triangles,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    //center = center + glm::vec3(1);

    float angle = 0.0;

    for (int i = 0; i < triangles; i++) {
        vertices.push_back(VertexFormat(center + glm::vec3(ray * cos(angle), ray * sin(angle), 0), color));
        indices.push_back(i);
        angle = angle + 2 * M_PI / triangles;
    }
    Mesh* circle = new Mesh(name);

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* GameObject::CreateRectangle(
    const std::string& name, 
    glm::vec3 leftBottomCorner, 
    float length, 
    float height, 
    glm::vec3 color, 
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}