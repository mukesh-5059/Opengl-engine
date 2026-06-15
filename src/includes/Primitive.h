#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Mesh;

namespace Primitive {
    Mesh* createTriangle();
    Mesh* createQuad();
    Mesh* createCube();
    Mesh* createSphere(unsigned int sectors = 36, unsigned int stacks = 18);
};
