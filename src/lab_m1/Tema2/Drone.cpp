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

Drone::Drone()
{
}

Drone::Drone(float pos_x, float pos_y, float pos_z)
{
    position.x = pos_x;
    position.y = pos_y;
    position.z = pos_z;

    forward = glm::vec3(0, 0, -1);
    up = glm::vec3(0, 1, 0);
    right = glm::vec3(1, 0, 0);

    rotation_x = 0;
    rotation_y = 0;
    rotation_z = 0;

    scale = 0.5f;

    movement_speed = 15.5f;
    rotor_rotation = 0;

}


Drone::~Drone()
{
}

void Drone::TranslateForward(float distance)
{
    // TODO(student): Translate the camera using the `forward` vector.
    // What's the difference between `TranslateForward()` and
    // `MoveForward()`?
    position += forward * distance;

}

void Drone::TranslateUpward(float distance)
{
    glm::vec3 dir = glm::normalize(glm::vec3(0, up.y, 0));
    position += dir * distance;

}

void Drone::TranslateRight(float distance)
{
    glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
    position += dir * distance;
}

void Drone::RotateOX(float angle)
{
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), angle, right);
    forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));
    up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1)));

}

void Drone::RotateOY(float angle)
{
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));
    forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));
    up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1)));
    right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(right, 1)));
}