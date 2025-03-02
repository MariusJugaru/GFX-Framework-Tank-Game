#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    // Sets the resolution of the small viewport
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

    rx = 0;
    ry = 0;
    rz = 0;

    cx = 0;
    cy = 0;
    cz = 0;
}

void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Lab4::RenderScene() {
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    glm::mat4 cube1;

    cube1 = glm::mat4(1);
    cube1 *= transform3D::Translate(0, 3.0f, -1.5f);
    cube1 *= transform3D::RotateOX(cx);
    cube1 *= transform3D::RotateOY(cy);
    cube1 *= transform3D::RotateOZ(cz);
    RenderMesh(meshes["box"], shaders["VertexNormal"], cube1);

    glm::mat4 cube2;
    glm::mat4 cube1_rotation;
    cube1_rotation = glm::rotate(cube1, rx, glm::vec3(1, 0, 0));

    cube2 = glm::mat4(1);
    
    // cube2 *= transform3D::Translate(-2.0f, 3.0f, -1.5f);
    
    cube2 *= cube1_rotation;
    cube2 *= transform3D::Translate(0, -2.0f, 0);
    // cube2 *= transform3D::Translate(0, -3.0f, -1.5f);

    

    RenderMesh(meshes["box"], shaders["VertexNormal"], cube2);
}

void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene();
    DrawCoordinateSystem();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // TODO(student): render the scene again, in the new viewport
    RenderScene();
    DrawCoordinateSystem();

    glClear(GL_DEPTH_BUFFER_BIT);

    if (rx < 6.28f) {
        rx += 1 * deltaTimeSeconds;
    }
    else {
        rx = 0;
    }
}

void Lab4::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // Movement
    if (window->KeyHold(GLFW_KEY_W)) {
        translateZ -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        translateZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        translateX += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        translateX -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_R)) {
        translateY += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_F)) {
        translateY -= 1 * deltaTime;
    }

    // Scaling
    if (window->KeyHold(GLFW_KEY_1)) {
        scaleX += 1 * deltaTime;
        scaleY += 1 * deltaTime;
        scaleZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        scaleX -= 1 * deltaTime;
        scaleY -= 1 * deltaTime;
        scaleZ -= 1 * deltaTime;
    }

    // Rotation
    if (window->KeyHold(GLFW_KEY_3)) {
        angularStepOX += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4)) {
        angularStepOX -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_5)) {
        angularStepOY += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6)) {
        angularStepOY -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_7)) {
        angularStepOZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_8)) {
        angularStepOZ -= 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        cx += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        cx -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        cz += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        cz -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_COMMA)) {
        cy += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_PERIOD)) {
        cy -= 1 * deltaTime;
    }

}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
    
    // TODO(student): Add viewport movement and scaling logic
    // View port
    if (key == GLFW_KEY_I) {
        miniViewportArea.y += 5;
    }
    if (key == GLFW_KEY_K) {
        miniViewportArea.y -= 5;
    }
    if (key == GLFW_KEY_J) {
        miniViewportArea.x -= 5;
    }
    if (key == GLFW_KEY_L) {
        miniViewportArea.x += 5;
    }

    if (key == GLFW_KEY_U) {
        miniViewportArea.width += 3;
        miniViewportArea.height += 3;
    }
    if (key == GLFW_KEY_O) {
        miniViewportArea.width -= 3;
        miniViewportArea.height -= 3;
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
