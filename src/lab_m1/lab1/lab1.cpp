#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
    RED_VAL = 0;
    GREEN_VAL = 0;
    BLUE_VAL = 0;

    magic_x = 1;
    magic_y = 1;
    magic_z = 1;

    obj_cycle = 0;

    jump_height = 1;
    jump_time = 0.2;
    gravity = 2 * jump_height / jump_time;
    jump_velocity = gravity * jump_time;

    prev_y = magic_y;
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.

    // teapot
    {
        Mesh* teapot_mesh = new Mesh("teapot");
        teapot_mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[teapot_mesh->GetMeshID()] = teapot_mesh;
    }

    // sphere
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(RED_VAL, GREEN_VAL, BLUE_VAL, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.
    if (obj_cycle == 0) {
        RenderMesh(meshes["teapot"], glm::vec3(magic_x, magic_y, magic_z));
    }
    else if (obj_cycle == 1)
    {
        RenderMesh(meshes["sphere"], glm::vec3(magic_x, magic_y, magic_z));
    }
    else if (obj_cycle == 2)
    {
        RenderMesh(meshes["box"], glm::vec3(magic_x, magic_y, magic_z));
    }


    

}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.
    if (window->KeyHold(GLFW_KEY_W)) {
        magic_y = magic_y + 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        magic_y = magic_y - 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        magic_x = magic_x + 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        magic_x = magic_x - 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        magic_z = magic_z + 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        magic_z = magic_z - 1 * deltaTime;
    }

    /*
    if (jump) {
        if (magic_y <= prev_y + jump_height) {
            magic_y += jump_velocity * deltaTime;
        }
        else {
            jump = false;
        }
    }
    if (!jump && !on_ground) {
        if (magic_y >= prev_y) {
            magic_y -= gravity * deltaTime;
        }
        else {
            on_ground = true;
        }
    }
    */

}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_UP) {
        // TODO(student): Change the values of the color components.
        if (!RED_VAL && !GREEN_VAL && !BLUE_VAL) {
            RED_VAL = 1;
        }
        else if (RED_VAL) {
            RED_VAL = 0;
            GREEN_VAL = 1;
        }
        else if (GREEN_VAL) {
            GREEN_VAL = 0;
            BLUE_VAL = 1;
        }
        else {
            BLUE_VAL = 0;
        }
        
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.

    if (key == GLFW_KEY_RIGHT) {
        obj_cycle++;
        obj_cycle = obj_cycle % 3;
    }

    if (key == GLFW_KEY_SPACE && on_ground) {
        jump = true;
        on_ground = false;
    }

}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
