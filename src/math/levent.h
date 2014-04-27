#ifndef LEVENT_H
#define LEVENT_H

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

struct LEvent  {
   glm::vec3 currentUp,currentLeft,currentDir,currentPos;
//   glm::mat3 currentRot;
   float currentThickness;
//   QPair<glm::vec3,float> currentRotation;
//    glm::mat4 currentRotation;
   QVector<glm::mat4> currentCTMs;
};

#endif // LEVENT_H
