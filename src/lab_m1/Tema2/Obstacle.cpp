#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Obstacle::Obstacle()
{
}

Obstacle::Obstacle(float pos_x, float pos_y, float pos_z, float rotation_x, float rotation_y, float rotation_z, float obj_scale, char* obj_type)
{
    position.x = pos_x;
    position.y = pos_y;
    position.z = pos_z;


    rotation.x = 0;
    rotation.y = 0;
    rotation.z = 0;

    scale = obj_scale;

    type = obj_type;

}

Obstacle::Obstacle(glm::vec3 pos, glm::vec3 rot, float obj_scale, char *obj_type)
{
    position = pos;
    rotation = rot;
    scale = obj_scale;
    type = obj_type;

}


Obstacle::~Obstacle()
{
}