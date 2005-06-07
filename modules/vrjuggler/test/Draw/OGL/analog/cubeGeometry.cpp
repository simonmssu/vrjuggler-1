#include "cubeGeometry.h"

// number of verticies
// this can be passed to opengl's glDrawArrays( .., .. );
const unsigned int cubeGeometry::mSize( 36 );

// format is: T2F_C4F_N3F_V3F
const float cubeGeometry::mData[432] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, -1, -1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1, 1, 1, 0, 0.3f, 0.3f, 0.3f, 1, 0, 0, 1, -1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1, 1, 1, 0, 0.3f, 0.3f, 0.3f, 1, 0, 0, 1, -1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, -1, 0, 0, -1, -1, 1, 0, 1, 0.3f, 0.3f, 0.3f, 1, -1, 0, 0, -1, 1, 1, 1, 0, 1, 1, 1, 1, -1, 0, 0, -1, -1, -1, 0, 1, 0.3f, 0.3f, 0.3f, 1, -1, 0, 0, -1, 1, 1, 1, 0, 1, 1, 1, 1, -1, 0, 0, -1, -1, -1, 1, 1, 0.5f, 0, 0.5f, 1, -1, 0, 0, -1, 1, -1, 0, 0, 1, 1, 1, 1, 0, 0, -1, -1, -1, -1, 0, 1, 0.5f, 0, 0.5f, 1, 0, 0, -1, -1, 1, -1, 1, 0, 0, 0.5f, 1.5f, 1, 0, 0, -1, 1, -1, -1, 0, 1, 0.5f, 0, 0.5f, 1, 0, 0, -1, -1, 1, -1, 1, 0, 0, 0.5f, 1.5f, 1, 0, 0, -1, 1, -1, -1, 1, 1, 0.5f, 0.5f, 0, 1, 0, 0, -1, 1, 1, -1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, -1, 1, 0, 1, 0, 0.5, 1.5f, 1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0.5f, 1.5f, 1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0.5f, 0.5f, 0, 1, 1, 0, 0, 1, 1, -1, 0, 0, 0.3f, 0.3f, 0.3f, 1, 0, 1, 0, -1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0.5f, 0, 0.5f, 1, 0, 1, 0, -1, 1, -1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0.5f, 0, 0.5f, 1, 0, 1, 0, -1, 1, -1, 1, 1, 0.5f, 0.5f, 0, 1, 0, 1, 0, 1, 1, -1, 0, 0, 1, 0, 0, 1, 0, -1, 0, -1, -1, 1, 0, 1, 1, 1, 1, 1, 0, -1, 0, -1, -1, -1, 1, 0, 0, 0, 1, 1, 0, -1, 0, 1, -1, 1, 0, 1, 1, 1, 1, 1, 0, -1, 0, -1, -1, -1, 1, 0, 0, 0, 1, 1, 0, -1, 0, 1, -1, 1, 1, 1, 0, 0.5f, 1.5f, 1, 0, -1, 0, 1, -1, -1 };
