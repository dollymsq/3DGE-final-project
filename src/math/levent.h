#ifndef LEVENT_H
#define LEVENT_H

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

struct LEvent  {
   glm::vec3 currentUp,currentLeft,currentDir,currentPos;
   glm::mat3 currentRot;
   float currentThickness;
};

#endif // LEVENT_H
