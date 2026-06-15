#pragma once

#include <Mesh.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Primitive {
    Mesh* createTriangle();
    Mesh* createQuad();
    Mesh* createCube();
    Mesh* createSphere(unsigned int sectors = 36, unsigned int stacks = 18);
};
