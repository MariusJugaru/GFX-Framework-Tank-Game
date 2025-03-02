#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#include <iostream>

namespace my_Camera
{
    class Camera
    {
    public:
        Camera()
        {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2.83f;
        }

        Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            Set(position, center, up);
        }

        ~Camera()
        { }

        // Update camera
        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            if (forward.x == 0 && forward.z == 0) {
                position += glm::normalize(up) * distance;
                return;
            }

            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            // TODO(student): Translate the camera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            position += forward * distance;

        }

        void TranslateUpward(float distance)
        {
            // TODO(student): Translate the camera using the `up` vector.
            glm::vec3 dir = glm::normalize(glm::vec3(0, up.y, 0));
            position += dir * distance;

        }

        void TranslateRight(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
            position += dir * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            // TODO(student): Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), angle, right);
            glm::vec3 aux = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));

            if (std::abs(std::abs(glm::dot(aux, glm::vec3(0, 1, 0))) - 1) < 0.05f) {

                return;
            }

            forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));
            up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1)));
        }

        void RotateFirstPerson_OY(float angle)
        {
            // TODO(student): Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));
            forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));
            up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1)));
            right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(right, 1)));
        
        }

        void RotateFirstPerson_OZ(float angle)
        {
            // TODO(student): Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), angle, forward);
            forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1)));
            up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1)));
            right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(right, 1)));
        }

        void RotateThirdPerson_OX(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OY axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OZ(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OZ axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

    public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        glm::mat4 projectionMatrix;
    };
}   // namespace implemented