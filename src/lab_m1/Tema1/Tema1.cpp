#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Projectile::Projectile(glm::mat3 projMatrix, float pX, float pY, glm::vec3 moveVec) {
    posX = pX;
    posY = pY;
    moveVector = moveVec;
    projectileMatrix = glm::mat3(1);
    projectileMatrix *= transform2D::Translate(posX, posY);
}

Tema1::Projectile::~Projectile()
{
}

Tema1::TerrainDeform::TerrainDeform(float hit) {
    xHit = hit;
    framesLeft = 30;
}

Tema1::TerrainDeform::~TerrainDeform()
{
}

Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


// HeightMap needs int values for indexes, get the closest index for the current position
float Tema1::GetClosestXPosIdx(float currentXPos, int squareWidth) {
    int divisions;

    divisions = currentXPos / squareWidth;

    if (currentXPos - squareWidth * divisions < squareWidth * (divisions + 1) - currentXPos) {
        return divisions;
    } else {
        return divisions + 1;
    }
}

int Tema1::GetDivision(float currentXPos, int squareWidth) {
    return (int)(currentXPos / squareWidth) * squareWidth;
}

std::vector<float> Tema1::GenerateHeightMap(int pointsNumber, int squareWidth, int width)
{
    std::vector<float> heightMap;

    for (int i = 0; i < width; i = i + squareWidth) {
        heightMap.push_back(15 * glm::sin(i / 20.0f) + 20 * glm::sin(i / 40.0f) + 100 * glm::cos(i / 120.0f));
    }

    return heightMap;
}

void Tema1::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data. It will be learned later, when GLSL shaders will be
    // introduced. For the moment, just think that each property value from
    // our vertex format needs to be sent to a certain channel, in order to
    // know how to receive it in the GLSL vertex shader.

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    glBindVertexArray(0);

    // Check for OpenGL errors
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}

void Tema1::CreateTank(glm::vec3 color1, glm::vec3 color2) {

    {
        std::vector<glm::vec3> circleAngles;
        float theta, x, y;

        int num_points = 32;
        float r = 15;

        for (int i = 0; i < num_points / 2 + 1; i++) {
            theta = 2 * 3.14f * float(i) / float(num_points);
            x = r * glm::cos(theta);
            y = r * glm::sin(theta);
            circleAngles.push_back(glm::vec3(x, y + 30, 0));
        }

        vector<VertexFormat> vertices {
            VertexFormat(glm::vec3(0, 30, 0), color1),
        };

        for (int i = 0; i < circleAngles.size(); i++) {
            vertices.push_back(VertexFormat(circleAngles[i], color1));
        }

        vector<unsigned int> indices;

        for (int i = 1; i < circleAngles.size(); i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        vector<VertexFormat> tankBaseVertices {
            VertexFormat(glm::vec3(-25, 0, 0), color2),
            VertexFormat(glm::vec3(25, 0, 0), color2),
            VertexFormat(glm::vec3(35, 10, 0), color2),
            VertexFormat(glm::vec3(-35, 10, 0), color2),
            VertexFormat(glm::vec3(-50, 10, 0), color1),
            VertexFormat(glm::vec3(50, 10, 0), color1),
            VertexFormat(glm::vec3(35, 30, 0), color1),
            VertexFormat(glm::vec3(-35, 30, 0), color1),
        };

        indices.push_back(circleAngles.size() + 1);
        indices.push_back(circleAngles.size() + 2);
        indices.push_back(circleAngles.size() + 3);
        indices.push_back(circleAngles.size() + 3);
        indices.push_back(circleAngles.size() + 4);
        indices.push_back(circleAngles.size() + 1);

        indices.push_back(circleAngles.size() + 5);
        indices.push_back(circleAngles.size() + 6);
        indices.push_back(circleAngles.size() + 7);
        indices.push_back(circleAngles.size() + 7);
        indices.push_back(circleAngles.size() + 8);
        indices.push_back(circleAngles.size() + 5);

        vertices.insert(vertices.end(), tankBaseVertices.begin(), tankBaseVertices.end());
        // Actually create the mesh from the data

        std::string name = "tank" + std::to_string(tankNo);
        CreateMesh(name.c_str(), vertices, indices);
        tankNo++;
    }
}

void Tema1::CreateTankGun() {

    vector<VertexFormat> vertices {
        VertexFormat(glm::vec3(0, -2.5f, 0), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(40, -2.5f, 0), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(40, 2.5f, 0), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0, 2.5f, 0), glm::vec3(0, 0, 0)),
    };

    vector<unsigned int> indices {
        0, 1, 2,
        2, 3, 0,
    };

    CreateMesh("tankGun", vertices, indices);
}

void Tema1::CreateCircle(glm::vec3 color) {
    std::vector<glm::vec3> circleAngles;
    float theta, x, y;

    int num_points = 32;
    float r = 5;

    for (int i = 0; i < num_points; i++) {
        theta = 2.0f * 3.14f * float(i) / float(num_points);
        x = r * glm::cos(theta);
        y = r * glm::sin(theta);
        circleAngles.push_back(glm::vec3(x, y, 0));
    }

    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(0, 0, 0), color),
    };

    for (int i = 0; i < circleAngles.size(); i++) {
        vertices.push_back(VertexFormat(circleAngles[i], color));
    }

    vector<unsigned int> indices;

    for (int i = 1; i < circleAngles.size() - 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indices.push_back(0);
    indices.push_back(circleAngles.size() - 1);
    indices.push_back(1);


    // Actually create the mesh from the data
    std::string name = "circle" + std::to_string(circleNo);
    CreateMesh(name.c_str(), vertices, indices);
    circleNo++;
    

}

void Tema1::CreateTrajectoryLine() {
    Mesh* line = new Mesh("line");
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < 90; i++) {
        vertices.push_back(glm::vec3(i, 75 * glm::sin(i / 25.0f), 0));
        
    }
    for (int i = 0; i < 89; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    line->SetDrawMode(GL_LINES);

    line->InitFromData(vertices, indices);

    AddMeshToList(line);
    
}

float Tema1::GetYPos(float xPos) {
    float yPos;

    aX = GetDivision(xPos, squareWidth);
    bX = aX + squareWidth;

    t = (xPos - aX) / (bX - aX);

    aY = heightMap[GetClosestXPosIdx(aX, squareWidth)];
    bY = heightMap[GetClosestXPosIdx(bX, squareWidth)];
    yPos = aY + t * (bY - aY);

    return yPos;
}

float Tema1::GetAngularStep(float xPos) {
    aX = GetDivision(xPos, squareWidth);
    bX = aX + squareWidth;

    aY = heightMap[GetClosestXPosIdx(aX, squareWidth)];
    bY = heightMap[GetClosestXPosIdx(bX, squareWidth)];

    return glm::atan2(bY - aY, bX - aX);
}

Mesh* Tema1::GetTrajectory(glm::vec3 moveVector) {
    Mesh* line = new Mesh("line");
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float projectoryX = 0;
    float projectoryY = 0;

    while (projectoryY >= -400) {
        vertices.push_back(glm::vec3(projectoryX, projectoryY, 0));

        // printf("%f %f\n", projectoryX, projectoryY);
        projectoryX += moveVector[0] * deltaTimeTrajectory;
        projectoryY += moveVector[1] * deltaTimeTrajectory;
        moveVector -= glm::vec3(0, projectileSpeed * deltaTimeTrajectory, 0);
    }

    for (int i = 0; i < vertices.size() - 1; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    line->SetDrawMode(GL_LINES);

    line->InitFromData(vertices, indices);

    return line;
}

void Tema1::Init()
{
    // camera
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // terrain settings
    pointsNumber = 300;
    squareWidth = resolution[0] / pointsNumber;

    heightMap = GenerateHeightMap(pointsNumber, squareWidth, resolution[0]);
    heightScale = 400 / squareWidth;
    heightOffset = 200;

    // health bar
    healthBarSize = 30;
    healthBarHeightOffset = 100;
    healthBarXOffset = -healthBarSize * 4.0f / 2;

    // tanks
    tankSpeed = 100;
    tankHeightOffset = 195;
    tankNo = 1;

    projectileSpeed = 400;
    tankHealth1 = 4;
    tankHealth2 = 4;

    circleNo = 1;

    threshold = 0.75f;
    // blast radius
    blastRadius = 20;
    {
        float theta, y;

        float r = squareWidth * blastRadius;

        for (float x = -r; x <= r; x += squareWidth) {
            y = sqrt(r * r - x * x);
            blastRadiusVector.push_back(y);
        }
    }

    // terrain
    glm::vec3 corner = glm::vec3(0, 0, 0);
    
    Mesh* square1 = object2D::CreateSquare("square1", corner, squareWidth, glm::vec3(0.4f, 1, 0.4), true);
    AddMeshToList(square1);

    Mesh* health = object2D::CreateSquare("health", corner, healthBarSize, glm::vec3(1, 1, 1), true);
    AddMeshToList(health);

    Mesh* healthBorder = object2D::CreateSquare("healthBorder", corner, 5, glm::vec3(1, 1, 1), true);
    AddMeshToList(healthBorder);
    
    
    CreateTank(glm::vec3(.85f, 0.627f, 0.3f), glm::vec3(.65f, 0.527f, 0.2f));
    CreateTank(glm::vec3(.15f, 0.383f, 0.7f), glm::vec3(.35f, 0.473f, 0.8f));
    CreateTankGun();

    CreateCircle(glm::vec3(0, 0, 0));
    
    // tank 1
    tankXPos1 = 30;
    aX = GetDivision(tankXPos1, squareWidth);
    bX = aX + squareWidth;

    t = (tankXPos1 - aX) / (bX - aX);

    aY = heightMap[GetClosestXPosIdx(aX, squareWidth)];
    bY = heightMap[GetClosestXPosIdx(bX, squareWidth)];
    tankYPos1 = aY + t * (bY - aY) + tankHeightOffset;

    tankAngularStep1 = glm::atan2(bY - aY, bX - aX);
    gunAngularStep1 = 3.14f / 2;

    moveVector = glm::vec3(projectileSpeed * glm::cos(gunAngularStep1), projectileSpeed * glm::sin(gunAngularStep1), 0);

    deltaTimeTrajectory = 0.06f;
    trajectory1 = GetTrajectory(moveVector);

    // tank2
    tankXPos2 = 1150;
    aX = GetDivision(tankXPos2, squareWidth);
    bX = aX + squareWidth;

    t = (tankXPos2 - aX) / (bX - aX);

    aY = heightMap[GetClosestXPosIdx(aX, squareWidth)];
    bY = heightMap[GetClosestXPosIdx(bX, squareWidth)];
    tankYPos2 = aY + t * (bY - aY) + tankHeightOffset;

    tankAngularStep2 = glm::atan2(bY - aY, bX - aX);
    gunAngularStep2 = 3.14f / 2;

    moveVector = glm::vec3(projectileSpeed * glm::cos(gunAngularStep2), projectileSpeed * glm::sin(gunAngularStep2), 0);

    trajectory2 = GetTrajectory(moveVector);
  
    wait_for_load = 0;
    loaded = 0;
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.8f, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}



void Tema1::Update(float deltaTimeSeconds)
{
    if (!loaded && wait_for_load < 2.0f) {
        wait_for_load += deltaTimeSeconds;
    } else {
        loaded = 1;
    }
    

    deltaTimeTrajectory = deltaTimeSeconds;
    if (tankYPos1 < -15) {
        tankHealth1 = 0;
    }

    if (tankYPos2 < -15) {
        tankHealth2 = 0;
    }
    // terrain
    for (int i = 0; i < heightMap.size(); i++) {
        sqrMatrix = glm::mat3(1);
        sqrMatrix *= transform2D::Translate(i * squareWidth, heightMap[i] - heightOffset);
        if (i < heightMap.size() - 1) {
            float heightA = heightMap[i];
            float heightB = heightMap[i + 1];
            sqrMatrix *= transform2D::ShearOY((heightB - heightA) / squareWidth);
        }
        sqrMatrix *= transform2D::Scale(1, heightScale);

        RenderMesh2D(meshes["square1"], shaders["VertexColor"], sqrMatrix);
    }

    float heightDif;

    
    for (int i = 0; i < heightMap.size() - 1; i++) {
        tankYPos1 = GetYPos(tankXPos1) + tankHeightOffset;
        tankAngularStep1 = GetAngularStep(tankXPos1);

        tankYPos2 = GetYPos(tankXPos2) + tankHeightOffset;
        tankAngularStep2 = GetAngularStep(tankXPos2);

        if (loaded) {
            heightDif = glm::abs((heightMap[i] + heightOffset) - (heightMap[i + 1] + heightOffset));
            if (heightDif > threshold) {
                if (heightMap[i] < heightMap[i + 1]) {
                    heightMap[i] += (heightDif - threshold) * deltaTimeSeconds;
                    heightMap[i + 1] -= (heightDif - threshold) * deltaTimeSeconds;
                }
                else {
                    heightMap[i] -= (heightDif - threshold) * deltaTimeSeconds;
                    heightMap[i + 1] += (heightDif - threshold) * deltaTimeSeconds;
                }

            }
        }
        

    }

    //tank 1
    if (tankHealth1 > 0) {
        tankMatrix1 = glm::mat3(1);
        tankMatrix1 *= transform2D::Translate(tankXPos1, tankYPos1);
        tankMatrix1 *= transform2D::Rotate(tankAngularStep1);

        RenderMesh2D(meshes["tank1"], shaders["VertexColor"], tankMatrix1);

        gunMatrix1 = glm::mat3(1);

        gunMatrix1 *= tankMatrix1;
        gunMatrix1 *= transform2D::Translate(0, 30);
        gunMatrix1 *= transform2D::Rotate(gunAngularStep1);
        gunMatrix1 *= transform2D::Rotate(-tankAngularStep1);

        RenderMesh2D(meshes["tankGun"], shaders["VertexColor"], gunMatrix1);

        healthBar1 = glm::mat3(1);
        healthBar1 *= transform2D::Translate(tankXPos1 + healthBarXOffset, tankYPos1 + healthBarHeightOffset);
        healthBar1 *= transform2D::Scale(tankHealth1, 1);

        RenderMesh2D(meshes["health"], shaders["VertexColor"], healthBar1);

        healthBarWidth1 = glm::mat3(1);
        healthBarWidth1 *= transform2D::Translate(tankXPos1 + healthBarXOffset, tankYPos1 + healthBarHeightOffset - 5);
        healthBarWidth1 *= transform2D::Scale(healthBarSize * 4.0f / 5, 1);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarWidth1);

        healthBarWidth1 *= transform2D::Translate(0, healthBarSize + 5);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarWidth1);

        healthBarHeight1 = glm::mat3(1);
        healthBarHeight1 *= transform2D::Translate(tankXPos1 + healthBarXOffset - 5, tankYPos1 + healthBarHeightOffset);
        healthBarHeight1 *= transform2D::Scale(1, healthBarSize / 5.0f);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarHeight1);

        healthBarHeight1 *= transform2D::Translate(healthBarSize * 4 + 5, 0);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarHeight1);

        
    }
    

    //tank2
    if (tankHealth2 > 0) {
        tankMatrix2 = glm::mat3(1);
        tankMatrix2 *= transform2D::Translate(tankXPos2, tankYPos2);
        tankMatrix2 *= transform2D::Rotate(tankAngularStep2);

        RenderMesh2D(meshes["tank2"], shaders["VertexColor"], tankMatrix2);

        gunMatrix2 = glm::mat3(1);

        gunMatrix2 *= tankMatrix2;
        gunMatrix2 *= transform2D::Translate(0, 30);
        gunMatrix2 *= transform2D::Rotate(gunAngularStep2);
        gunMatrix2 *= transform2D::Rotate(-tankAngularStep2);

        RenderMesh2D(meshes["tankGun"], shaders["VertexColor"], gunMatrix2);

        healthBar2 = glm::mat3(1);
        healthBar2 *= transform2D::Translate(tankXPos2 + healthBarXOffset, tankYPos2 + healthBarHeightOffset);
        healthBar2 *= transform2D::Scale(tankHealth2, 1);

        RenderMesh2D(meshes["health"], shaders["VertexColor"], healthBar2);

        healthBarWidth2 = glm::mat3(1);
        healthBarWidth2 *= transform2D::Translate(tankXPos2 + healthBarXOffset, tankYPos2 + healthBarHeightOffset - 5);
        healthBarWidth2 *= transform2D::Scale(healthBarSize * 4.0f / 5, 1);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarWidth2);

        healthBarWidth2 *= transform2D::Translate(0, healthBarSize + 5);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarWidth2);

        healthBarHeight2 = glm::mat3(1);
        healthBarHeight2 *= transform2D::Translate(tankXPos2 + healthBarXOffset - 5, tankYPos2 + healthBarHeightOffset);
        healthBarHeight2 *= transform2D::Scale(1, healthBarSize / 5.0f);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarHeight2);

        healthBarHeight2 *= transform2D::Translate(healthBarSize * 4 + 5, 0);

        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], healthBarHeight2);

        
    }
    
    
    


    // projectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        RenderMesh2D(meshes["circle1"], shaders["VertexColor"], it->projectileMatrix);
        it->projectileMatrix *= transform2D::Translate(it->moveVector[0] * deltaTimeSeconds, it->moveVector[1] * deltaTimeSeconds);
        it->posX += it->moveVector[0] * deltaTimeSeconds;
        it->posY += it->moveVector[1] * deltaTimeSeconds;
        it->moveVector -= glm::vec3(0, projectileSpeed * deltaTimeSeconds, 0);

        if (it->posY < -15) {
            it = projectiles.erase(it);
        } else if (it->posY - (GetYPos(it->posX) + tankHeightOffset) < 2) {
            int hitLeft, hitRight;
            
            int hitPos = GetClosestXPosIdx(it->posX, squareWidth);
            hitLeft = glm::max(0, hitPos - blastRadius);
            hitRight = glm::min((int)heightMap.size() - 1, hitPos + blastRadius);

            int j = 0;
            for (int i = hitLeft; i < hitRight; i++) {
                heightMap[i] = glm::min(it->posY - blastRadiusVector[j] - tankHeightOffset, heightMap[i]);
                j++;
            }

            it = projectiles.erase(it);
        } else if (it->posX - 5 < 0 || it->posX + 5 > resolution[0]) {
            it = projectiles.erase(it);
        } else if ((it->posY - (tankYPos1 + 30) < 10) && (tankXPos1 - 30 + glm::sin(-tankAngularStep1) * 8 < it->posX && it->posX < tankXPos1 + 30 + glm::sin(-tankAngularStep1) * 8)) {
            tankHealth1--;
            it = projectiles.erase(it);
        } else if ((it->posY - (tankYPos2 + 30) < 10) && (tankXPos2 - 30 + glm::sin(-tankAngularStep2) * 8 < it->posX && it->posX < tankXPos2 + 30 + glm::sin(-tankAngularStep2) * 8)) {
            tankHealth2--;
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }

    // trajectory 1
    if (tankHealth1) {
        glLineWidth(3);
        trajectoryMatrix1 = glm::mat3(1);
        trajectoryMatrix1 *= tankMatrix1;
        trajectoryMatrix1 *= transform2D::Translate(0, 30);
        trajectoryMatrix1 *= transform2D::Rotate(-tankAngularStep1);
        trajectoryMatrix1 *= transform2D::Rotate(gunAngularStep1 - 3.14f / 2);
        trajectoryMatrix1 *= transform2D::Translate(0, 40);
        trajectoryMatrix1 *= transform2D::Rotate(-gunAngularStep1 + 3.14f / 2);
        RenderMesh2D(trajectory1, shaders["VertexColor"], trajectoryMatrix1);
        glLineWidth(1);
    }
    

    // trajectory 2
    if (tankHealth2) {
        glLineWidth(3);
        trajectoryMatrix2 = glm::mat3(1);
        trajectoryMatrix2 *= tankMatrix2;
        trajectoryMatrix2 *= transform2D::Translate(0, 30);
        trajectoryMatrix2 *= transform2D::Rotate(-tankAngularStep2);
        trajectoryMatrix2 *= transform2D::Rotate(gunAngularStep2 - 3.14f / 2);
        trajectoryMatrix2 *= transform2D::Translate(0, 40);
        trajectoryMatrix2 *= transform2D::Rotate(-gunAngularStep2 + 3.14f / 2);
        RenderMesh2D(trajectory2, shaders["VertexColor"], trajectoryMatrix2);
        glLineWidth(1);
    }
    
    
    // keyboard logic

    // tank1
    if (window->KeyHold(GLFW_KEY_W)) {
        gunAngularStep1 -= 0.75f * deltaTimeSeconds;
        trajectory1 = GetTrajectory(glm::vec3(projectileSpeed * glm::cos(gunAngularStep1), projectileSpeed * glm::sin(gunAngularStep1), 0));
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        gunAngularStep1 += 0.75f * deltaTimeSeconds;
        trajectory1 = GetTrajectory(glm::vec3(projectileSpeed * glm::cos(gunAngularStep1), projectileSpeed * glm::sin(gunAngularStep1), 0));
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        if (tankXPos1 < 1250) {
            tankXPos1 += tankSpeed * deltaTimeSeconds;

            tankYPos1 = GetYPos(tankXPos1) + tankHeightOffset;
            tankAngularStep1 = GetAngularStep(tankXPos1);
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        if (tankXPos1 > 30) {
            tankXPos1 -= tankSpeed * deltaTimeSeconds;

            tankYPos1 = GetYPos(tankXPos1) + tankHeightOffset;
            tankAngularStep1 = GetAngularStep(tankXPos1);
        }
    }


    // tank2
    if (window->KeyHold(GLFW_KEY_UP)) {
        gunAngularStep2 -= 0.75f * deltaTimeSeconds;
        trajectory2 = GetTrajectory(glm::vec3(projectileSpeed * glm::cos(gunAngularStep2), projectileSpeed * glm::sin(gunAngularStep2), 0));

    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        gunAngularStep2 += 0.75f * deltaTimeSeconds;
        trajectory2 = GetTrajectory(glm::vec3(projectileSpeed * glm::cos(gunAngularStep2), projectileSpeed * glm::sin(gunAngularStep2), 0));
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        if (tankXPos2 < 1250) {
            tankXPos2 += tankSpeed * deltaTimeSeconds;

            tankYPos2 = GetYPos(tankXPos2) + tankHeightOffset;
            tankAngularStep2 = GetAngularStep(tankXPos2);
        }
    }

    if (window->KeyHold(GLFW_KEY_LEFT)) {
        if (tankXPos2 > 30) {
            tankXPos2 -= tankSpeed * deltaTimeSeconds;

            tankYPos2 = GetYPos(tankXPos2) + tankHeightOffset;
            tankAngularStep2 = GetAngularStep(tankXPos2);
        }
    }
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && tankHealth1) {
        // spawn new projectile
        glm::vec3 moveVector = glm::vec3(projectileSpeed * glm::cos(gunAngularStep1), projectileSpeed * glm::sin(gunAngularStep1), 0);
        float cannonPosX = tankXPos1 + 40 * glm::cos(gunAngularStep1) + 30 * glm::sin(-tankAngularStep1);
        float cannonPosY = tankYPos1 + 40 * glm::sin(gunAngularStep1) + 30 * glm::cos(-tankAngularStep1);
        projectiles.push_back(Projectile(gunMatrix1, cannonPosX, cannonPosY, moveVector));
    }

    if (key == GLFW_KEY_ENTER && tankHealth2) {
        // spawn new projectile
        glm::vec3 moveVector = glm::vec3(projectileSpeed * glm::cos(gunAngularStep2), projectileSpeed * glm::sin(gunAngularStep2), 0);
        float cannonPosX = tankXPos2 + 40 * glm::cos(gunAngularStep2) + 30 * glm::sin(-tankAngularStep2);
        float cannonPosY = tankYPos2 + 40 * glm::sin(gunAngularStep2) + 30 * glm::cos(-tankAngularStep2);
        projectiles.push_back(Projectile(gunMatrix2, cannonPosX, cannonPosY, moveVector));
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}

