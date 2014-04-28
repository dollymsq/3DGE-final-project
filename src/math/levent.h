#ifndef LEVENT_H
#define LEVENT_H

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

struct LEvent  {
   glm::vec3 currentUp,currentLeft,currentDir,currentPos,prevDir;
   float currentThickness,prevThickness;
   QPair<glm::vec3,glm::vec3> currentVecs;
};

#endif // LEVENT_H
