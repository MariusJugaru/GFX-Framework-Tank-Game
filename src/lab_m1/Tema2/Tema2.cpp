#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

#define PI 3.14f

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    // camera settings
    fov = RADIANS(60);
    zNear = 0.01f;
    zFar = 200.0f;

    left = -50.0f * 1.77f;
    right = 50.0f * 1.77f;
    bottom = -50.0f;
    top = 50.0f;

    // camera
    renderCameraTarget = false;

    camera = new my_Camera::Camera();
    camera->Set(glm::vec3(0, 4.5f, 2.0f), glm::vec3(0, 2.5, 0), glm::vec3(0, 1, 0));
    camera->projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);

    // minimap camera
    minimap_camera = new my_Camera::Camera();
    minimap_camera->Set(glm::vec3(0, 100, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
    minimap_camera->projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
 

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);


    // terrain
    num_squares = 100;

    // shader
    // Create a shader program
    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("ObjectShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderObjects.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    GenerateTerrain("terrain", glm::vec3(0.3f, 0.65f, 0.3f), num_squares);
    CreateTree("tree", glm::vec3(0.1f, 0.5f, 0.1f), glm::vec3(0.3f, 0.15f, 0.05f));
    CreateRock("rock", glm::vec3(0.4f, 0.4f, 0.4f));
    

    // drone
    CreateRectangle("rectangle", glm::vec3(0.7f, 0.7f, 0.7f));
    CreateRectangle("rotor", glm::vec3(0.1f, 0.1f, 0.1f));
    CreateBox("cube", glm::vec3(0.7f, 0.7f, 0.7f));
    CreateBox("packet", glm::vec3(0.8f, 0.4f, 0.3f));
    CreateBox("cloud", glm::vec3(0.95f, 0.95f, 0.95f));
    CreateArrow("direction", glm::vec3(0, 0, 0));
    CreateArrow("target_direction", glm::vec3(0.9f, 0.9f, 0.3f));
    CreateCylinder("destination", glm::vec3(0.2f, 0.2f, 0.2f));

    drone = Drone(0.0f, 2.0f, 0.0f);
    rotation_speed = 10;

    // generate obstacles
    num_obstacles = 150;
    z_min = -200;
    z_max = 200;
    x_min = -200;
    x_max = 200;

    GenerateObstacles();
    CreateCylinder("circle", glm::vec3(0, 0, 0));

    new_packet = 1;
    packet_picked = 0;

    new_destination = 0;

    score = 0;

    // clouds
    num_clouds = 50;
    GenerateClouds();

}

void m1::Tema2::GeneratePacket() {
    while (1) {
        glm::vec3 rand_pos(x_min + (float)std::rand() / RAND_MAX * (x_max - x_min), 0, z_min + (float)std::rand() / RAND_MAX * (z_max - z_min));
        int collision = 0;

        float scale = 3;

        for (const auto& obs : obstacles) {
            if (IsCollidingSpheres(obs.position, rand_pos, obs.scale, scale, 2.0f, 2.0f) ||
                IsCollidingSpheres(glm::vec3(0, 0, 0), rand_pos, 1, scale, 15, 2.0f)) {
                collision = 1;
                break;
            }
        }

        if (!collision) {
            rand_pos.y = 1;
            packet = Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "packet");

            break;
        }
    }
        
}

void m1::Tema2::GenerateDestination() {
    while (1) {
        glm::vec3 rand_pos(x_min + (float)std::rand() / RAND_MAX * (x_max - x_min), 0, z_min + (float)std::rand() / RAND_MAX * (z_max - z_min));
        int collision = 0;


        float scale = 5;

        for (const auto& obs : obstacles) {
            if (IsCollidingSpheres(obs.position, rand_pos, obs.scale, scale, 2.0f, 5.0f)) {
                collision = 1;
                break;
            }
        }

        if (!collision) {
            rand_pos.y = 1;
            destination = Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "destination");

            break;
        }

    }

}

void m1::Tema2::GenerateObstacles()
{
    for (int i = 0; i < num_obstacles; i++) {
        glm::vec3 rand_pos(x_min + (float)std::rand() / RAND_MAX * (x_max - x_min), 0, z_min + (float)std::rand() / RAND_MAX * (z_max - z_min));
        int collision = 0;

        if (std::rand() % 2) {
            float scale = 2 + (float)std::rand() / RAND_MAX * 2;

            for (const auto& obs : obstacles) {
                if (IsCollidingSpheres(obs.position, rand_pos, obs.scale, scale, 2.0f, 2.0f) ||
                    IsCollidingSpheres(glm::vec3(0, 0, 0), rand_pos, 1, scale, 15, 2.0f)) {
                    i--;
                    collision = 1;
                    break;
                }
            }

            if (!collision) {
                obstacles.push_back(Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "tree"));
            }
        }
        else {
            float scale = 5 + (float)std::rand() / RAND_MAX * 5;
            rand_pos.y = (float)std::rand() / RAND_MAX;
            glm::vec3 rand_rot((float)std::rand() / RAND_MAX * 2 * PI, (float)std::rand() / RAND_MAX * 2 * PI, (float)std::rand() / RAND_MAX * 2 * PI);

            for (const auto& obs : obstacles) {
                if (IsCollidingSpheres(obs.position, rand_pos, obs.scale, scale, 1.5f, 1.5f) ||
                    IsCollidingSpheres(glm::vec3(0, 0, 0), rand_pos, 1, scale, 15, 2.0f)) {
                    i--;
                    collision = 1;
                    break;
                }
            }

            if (!collision) {
                obstacles.push_back(Obstacle(rand_pos, rand_rot, scale, "rock"));
            }
        }

    }
}

void m1::Tema2::GenerateClouds()
{
    for (int i = 0; i < num_clouds; i++) {
        glm::vec3 rand_pos(x_min + (float)std::rand() / RAND_MAX * (x_max - x_min), 100 + (float)std::rand() / RAND_MAX, z_min + (float)std::rand() / RAND_MAX * (z_max - z_min));

        float scale = 5 + (float)std::rand() / RAND_MAX * 5;

        int cloud_type = rand() % 3;
        switch (cloud_type) {
            case 0:
                clouds.push_back(Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "cloud0"));
                break;
            case 1:
                clouds.push_back(Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "cloud1"));
                break;
            case 2:
                clouds.push_back(Obstacle(rand_pos, glm::vec3(0, 0, 0), scale, "cloud2"));
                break;
        }


        
        
    }
}

int m1::Tema2::IsCollidingSpheres(glm::vec3 pos1, glm::vec3 pos2, float rad1, float rad2, float max1, float max2) {
    return glm::distance(pos1, pos2) < max1 * rad1 + max2 * rad2;

}

int m1::Tema2::IsCollidingTree(glm::vec3 pos1, glm::vec3 pos2, float rad1, float rad2, float max1, float max2) {
    
    // if the drone isn't on pos1, change it
    if (pos1.y == 0) {
        glm::vec3 aux_vec;
        float aux_rad, aux_max;

        aux_vec = pos1;
        pos1 = pos2;
        pos2 = aux_vec;

        aux_rad = rad1;
        rad1 = rad2;
        rad2 = aux_rad;

        aux_max = max1;
        max1 = max2;
        max2 = aux_max;
    }
    
    // under the leaves
    if (pos1.y + 0.5f < 1.5f * rad2) {
        return glm::distance(glm::vec2(pos1.x, pos1.z), glm::vec2(pos2.x, pos2.z)) < max1 * rad1 + 0.5f * rad2;
    } else if (pos1.y + 0.5f >= 1.5f * rad2 && pos1.y < 6.0f * rad2) {
        return glm::distance(glm::vec2(pos1.x, pos1.z), glm::vec2(pos2.x, pos2.z)) < max1 * rad1 + max2 * rad2;
    } else if (pos1.y >= 6.0f * rad2 && pos1.y < 8.0f * rad2) {
        return glm::distance(pos1, glm::vec3(pos2.x, pos2.y + 5.0f * rad2, pos2.z)) < max1 * rad1 + max2 * rad2;
    } else {
        return 0;
    }
}

void m1::Tema2::CreateRectangle(char *name, glm::vec3 color)
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-2, -0.25f, 0.25f), color),
        VertexFormat(glm::vec3(2, -0.25f, 0.25f), color),
        VertexFormat(glm::vec3(2, 0.25f, 0.25f), color),
        VertexFormat(glm::vec3(-2, 0.25f, 0.25f), color),

        VertexFormat(glm::vec3(-2, -0.25f, -0.25f), color),
        VertexFormat(glm::vec3(2, -0.25f, -0.25f), color),
        VertexFormat(glm::vec3(2, 0.25f, -0.25f), color),
        VertexFormat(glm::vec3(-2, 0.25f, -0.25f), color),
    };

    vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        0, 1, 5,
        5, 4, 0,
        4, 0, 3,
        3, 7, 4,
        3, 2, 6,
        6, 7, 3,
        5, 4, 7,
        7, 6, 5,

    };

    // Actually create the mesh from the data
    CreateMesh(name, vertices, indices);
}

void m1::Tema2::CreateBox(char* name, glm::vec3 color)
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-0.25f, -0.25f, 0.25f), color),
        VertexFormat(glm::vec3(0.25f, -0.25f, 0.25f), color),
        VertexFormat(glm::vec3(0.25f, 0.25f, 0.25f), color),
        VertexFormat(glm::vec3(-0.25f, 0.25f, 0.25f), color),

        VertexFormat(glm::vec3(-0.25f, -0.25f, -0.25f), color),
        VertexFormat(glm::vec3(0.25f, -0.25f, -0.25f), color),
        VertexFormat(glm::vec3(0.25f, 0.25f, -0.25f), color),
        VertexFormat(glm::vec3(-0.25f, 0.25f, -0.25f), color),

    };

    vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        0, 1, 5,
        5, 4, 0,
        4, 0, 3,
        3, 7, 4,
        3, 2, 6,
        6, 7, 3,
        5, 4, 7,
        7, 6, 5,

    };

    // Actually create the mesh from the data
    CreateMesh(name, vertices, indices);
}

void m1::Tema2::CreateTree(char* name, glm::vec3 color1, glm::vec3 color2)
{
    std::vector<VertexFormat> vertices;

    unsigned int index = 0;

    // stump
    for (int i = 0; i < 2; i++) {
        vertices.emplace_back(glm::vec3(0.5f, i * 1.5f, 0), color2);
        vertices.emplace_back(glm::vec3(0, i * 1.5f, 0.5f), color2);
        vertices.emplace_back(glm::vec3(-0.5f, i * 1.5f, 0), color2);
        vertices.emplace_back(glm::vec3(0, i * 1.5f, -0.5f), color2);
    }

    vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        0, 1, 5,
        5, 4, 0,
        4, 0, 3,
        3, 7, 4,
        3, 2, 6,
        6, 7, 3,
        5, 4, 7,
        7, 6, 5,

    };

    index += 8;
    float base_y = 1.5f;
    float tip_y = 4.5f;

    float radius = 2.5f;
    // cones
    for (int i = 0; i < 3; ++i) {
        vertices.emplace_back(glm::vec3(radius, base_y, 0), color1);
        vertices.emplace_back(glm::vec3(radius * glm::sqrt(2) / 2, base_y, radius * glm::sqrt(2) / 2), color1);
        vertices.emplace_back(glm::vec3(0, base_y, radius * 1), color1);
        vertices.emplace_back(glm::vec3(radius * -glm::sqrt(2) / 2, base_y, radius * glm::sqrt(2) / 2), color1);

        vertices.emplace_back(glm::vec3(radius * -1, base_y, 0), color1);
        vertices.emplace_back(glm::vec3(radius * -glm::sqrt(2) / 2, base_y, radius * -glm::sqrt(2) / 2), color1);
        vertices.emplace_back(glm::vec3(0, base_y, radius * -1), color1);
        vertices.emplace_back(glm::vec3(radius * glm::sqrt(2) / 2, base_y, radius * -glm::sqrt(2) / 2), color1);

        vertices.emplace_back(glm::vec3(0, tip_y, 0), color1);

        for (int j = 0; j < 8; ++j) {
            indices.push_back(index + j);
            indices.push_back(index + (j + 1) % 8);
            indices.push_back(index + 8);
        }

        for (int j = 1; j < 7; ++j) {
            indices.push_back(index);
            indices.push_back(index + j);
            indices.push_back(index + j + 1);
        }

        index += 9;
        base_y += 1.5f;
        tip_y += 1.5f;
    }

    // Actually create the mesh from the data
    CreateMesh(name, vertices, indices);
}

void m1::Tema2::CreateRock(char* name, glm::vec3 color)
{

    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(0, 0, 1), color),
        VertexFormat(glm::vec3(0.7f, 0, 0.7f), color),
        VertexFormat(glm::vec3(1.2f, 0, -0.2f), color),
        VertexFormat(glm::vec3(-0.2f, 0, -1.2f), color),
        VertexFormat(glm::vec3(-1.2f, 0, 0), color),
        VertexFormat(glm::vec3(0.25f, 1, 0.6f), color),
        VertexFormat(glm::vec3(-0.5f, 0.8f, 0.3f), color),
        VertexFormat(glm::vec3(0, 0.8f, -0.75f), color),
        VertexFormat(glm::vec3(0.3, -0.6f, 0.5f), color),
        VertexFormat(glm::vec3(0.6, -0.7f, -0.3f), color),
        VertexFormat(glm::vec3(-0.7, -0.6f, -0.1), color),

    };

    vector<unsigned int> indices =
    {
        0, 6, 5,
        0, 4, 6,
        4, 3, 6,
        3, 7, 6,
        3, 2, 7,
        2, 1, 7,
        1, 5, 7,
        1, 0, 5,
        5, 6, 7,

        0, 1, 8,
        1, 2, 8,
        8, 2, 9,
        2, 3, 9,
        3, 4, 9,
        9, 4, 10,
        4, 0, 10,
        10, 0, 8,
        10, 8, 9,

    };


    // Actually create the mesh from the data
    CreateMesh(name, vertices, indices);
}

void m1::Tema2::CreateArrow(char* name, glm::vec3 color)
{

    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-1, 0, 0), color),
        VertexFormat(glm::vec3(0, 0, -1), color),
        VertexFormat(glm::vec3(0, 0, -0.5), color),
        VertexFormat(glm::vec3(1, 0, 0), color),
    };

    vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 1, 3,
    };


    // Actually create the mesh from the data
    CreateMesh(name, vertices, indices);
}

void m1::Tema2::CreateCylinder(char* name, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float radius = 1;
    int points = 16;

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < points; i++) {
            float theta = 2.0f * PI * i / points;
            float x = radius * glm::cos(theta);
            float z = radius * glm::sin(theta);

            vertices.emplace_back(glm::vec3(x, -0.5f + 1 * j, z), color);
        }
    }

    for (int i = 0; i < points; i++) {
        indices.push_back(0 );
        indices.push_back(i);
        indices.push_back((i + 1) % points);
    }

    for (int i = 0; i < points; i++) {
        indices.push_back(points);
        indices.push_back(points + i);
        indices.push_back(points + (i + 1) % points);
    }

    for (int i = 0; i < points; i++) {
        indices.push_back(i);
        indices.push_back((i + 1) % points);
        indices.push_back(points + (i + 1) % points);

        indices.push_back(points + (i + 1) % points);
        indices.push_back(points + i % points);
        indices.push_back(i);
        
    }
    
    
    CreateMesh(name, vertices, indices);
}

// num squares: size x size
void m1::Tema2::GenerateTerrain(char* name, glm::vec3 color, int size) {
    vector<VertexFormat> vertices;

    for (int i = 0; i <= size; i++) {
        for (int j = 0; j <= size; j++) {
            vertices.push_back(VertexFormat(glm::vec3(i, 0, j), color));
        }
    }

    vector<unsigned int> indices;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            indices.push_back(i * (size + 1) + j);
            indices.push_back(i * (size + 1) + j + 1);
            indices.push_back((i + 1) * (size + 1) + j + 1);

            indices.push_back((i + 1) * (size + 1) + j + 1);
            indices.push_back((i + 1) * (size + 1) + j);
            indices.push_back(i * (size + 1) + j);
        }
    }

    CreateMesh(name, vertices, indices);
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.8f, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene(my_Camera::Camera* camera) {
    glm::mat4 terrainMatrix = glm::mat4(1);
    terrainMatrix = glm::scale(terrainMatrix, glm::vec3(5, 1, 5));
    terrainMatrix = glm::translate(terrainMatrix, glm::vec3((-num_squares / 2), 0, (-num_squares / 2)));
    RenderSimpleMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix, camera);

    glm::mat4 cloudMatrix = glm::mat4(1);
    RenderSimpleMesh(meshes["sphere"], shaders["VertexColor"], cloudMatrix, camera);


    for (const auto& obs : obstacles) {
        glm::mat4 obstacleMatrix = glm::mat4(1);
        obstacleMatrix = glm::translate(obstacleMatrix, obs.position);
        obstacleMatrix = glm::scale(obstacleMatrix, glm::vec3(obs.scale, obs.scale, obs.scale));
        obstacleMatrix = glm::rotate(obstacleMatrix, obs.rotation.x, glm::vec3(1, 0, 0));
        obstacleMatrix = glm::rotate(obstacleMatrix, obs.rotation.y, glm::vec3(0, 1, 0));
        obstacleMatrix = glm::rotate(obstacleMatrix, obs.rotation.z, glm::vec3(0, 0, 1));
        RenderSimpleMesh(meshes[obs.type], shaders["ObjectShader"], obstacleMatrix, camera);
    }

    {
        glm::mat4 transformMatrix = glm::mat4(1);
        transformMatrix = glm::translate(transformMatrix, glm::vec3(drone.position.x, drone.position.y, drone.position.z));
        transformMatrix = glm::rotate(transformMatrix, drone.rotation_y, glm::vec3(0, 1, 0));
        transformMatrix = glm::rotate(transformMatrix, drone.rotation_x, glm::vec3(1, 0, 0));
        transformMatrix = glm::rotate(transformMatrix, drone.rotation_z, glm::vec3(0, 0, 1));
        transformMatrix = glm::rotate(transformMatrix, PI / 4, glm::vec3(0, 1, 0));
        transformMatrix = glm::scale(transformMatrix, glm::vec3(drone.scale, drone.scale, drone.scale));

        if (camera == minimap_camera) {
            glm::mat4 arrowMatrix = glm::mat4(1);
            arrowMatrix = glm::translate(arrowMatrix, glm::vec3(drone.position.x, drone.position.y, drone.position.z));
            arrowMatrix = glm::rotate(arrowMatrix, drone.rotation_y, glm::vec3(0, 1, 0));
            arrowMatrix = glm::translate(arrowMatrix, glm::vec3(0, 25, -5));
            arrowMatrix = glm::scale(arrowMatrix, glm::vec3(5, 1, 5));
            RenderSimpleMesh(meshes["direction"], shaders["VertexColor"], arrowMatrix, camera);

            float packet_angle;
            if (!packet_picked) {
                packet_angle = glm::atan2(drone.position.x - packet.position.x, drone.position.z - packet.position.z);
            } else {
                packet_angle = glm::atan2(drone.position.x - destination.position.x, drone.position.z - destination.position.z);;
            }

            arrowMatrix = glm::mat4(1);
            arrowMatrix = glm::translate(arrowMatrix, glm::vec3(drone.position.x, drone.position.y, drone.position.z));
            arrowMatrix = glm::rotate(arrowMatrix, packet_angle, glm::vec3(0, 1, 0));
            arrowMatrix = glm::translate(arrowMatrix, glm::vec3(0, 30, -5));
            arrowMatrix = glm::scale(arrowMatrix, glm::vec3(5, 1, 5));
            RenderSimpleMesh(meshes["target_direction"], shaders["VertexColor"], arrowMatrix, camera);

            glm::mat4 circleMatrix = glm::mat4(1);
            circleMatrix = glm::translate(circleMatrix, glm::vec3(drone.position.x, drone.position.y, drone.position.z));
            circleMatrix = glm::translate(circleMatrix, glm::vec3(0, 25, 0));
            circleMatrix = glm::scale(circleMatrix, glm::vec3(5, 1, 5));
            RenderSimpleMesh(meshes["circle"], shaders["VertexColor"], circleMatrix, camera);
        }
        
        if (!packet_picked) {
            glm::mat4 packetMatrix = glm::mat4(1);
            packetMatrix = glm::translate(packetMatrix, packet.position);
            packetMatrix = glm::scale(packetMatrix, glm::vec3(packet.scale, packet.scale, packet.scale));
            RenderSimpleMesh(meshes["packet"], shaders["ObjectShader"], packetMatrix, camera);
        }

        if (packet_picked) {
            glm::mat4 packetMatrix = glm::mat4(1);
            packetMatrix = transformMatrix * packetMatrix;
            packetMatrix = glm::translate(packetMatrix, glm::vec3(0, -1, 0));
            packetMatrix = glm::scale(packetMatrix, glm::vec3(3, 3, 3));
            RenderSimpleMesh(meshes["packet"], shaders["ObjectShader"], packetMatrix, camera);

            glm::mat4 destinationMatrix = glm::mat4(1);
            destinationMatrix = glm::translate(destinationMatrix, destination.position);
            destinationMatrix = glm::scale(destinationMatrix, glm::vec3(destination.scale, 1, destination.scale));
            RenderSimpleMesh(meshes["destination"], shaders["VertexColor"], destinationMatrix, camera);
        }
        

        glm::mat4 baseMatrix1 = glm::mat4(1);
        baseMatrix1 = transformMatrix * baseMatrix1;
        RenderSimpleMesh(meshes["rectangle"], shaders["VertexColor"], baseMatrix1, camera);

        glm::mat4 baseMatrix2 = glm::mat4(1);
        baseMatrix2 = transformMatrix * baseMatrix2;
        baseMatrix2 = glm::rotate(baseMatrix2, PI / 2, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["rectangle"], shaders["VertexColor"], baseMatrix2, camera);

        glm::mat4 cubeMatrix1 = glm::mat4(1);
        cubeMatrix1 = transformMatrix * cubeMatrix1;
        cubeMatrix1 = glm::translate(cubeMatrix1, glm::vec3(1.75f, 0.5f, 0));
        RenderSimpleMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix1, camera);

        glm::mat4 cubeMatrix2 = glm::mat4(1);
        cubeMatrix2 = transformMatrix * cubeMatrix2;
        cubeMatrix2 = glm::translate(cubeMatrix2, glm::vec3(-1.75f, 0.5f, 0));
        RenderSimpleMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix2, camera);

        glm::mat4 cubeMatrix3 = glm::mat4(1);
        cubeMatrix3 = transformMatrix * cubeMatrix3;
        cubeMatrix3 = glm::translate(cubeMatrix3, glm::vec3(0, 0.5f, -1.75f));
        RenderSimpleMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix3, camera);

        glm::mat4 cubeMatrix4 = glm::mat4(1);
        cubeMatrix4 = transformMatrix * cubeMatrix4;
        cubeMatrix4 = glm::translate(cubeMatrix4, glm::vec3(0, 0.5f, 1.75f));
        RenderSimpleMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix4, camera);

        glm::mat4 rotorMatrix1 = glm::mat4(1);
        rotorMatrix1 = transformMatrix * rotorMatrix1;
        rotorMatrix1 = glm::translate(rotorMatrix1, glm::vec3(1.75f, 0.8f, 0));
        rotorMatrix1 = glm::scale(rotorMatrix1, glm::vec3(0.3f, 0.2f, 0.2f));
        rotorMatrix1 = glm::rotate(rotorMatrix1, drone.rotor_rotation, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["rotor"], shaders["VertexColor"], rotorMatrix1, camera);

        glm::mat4 rotorMatrix2 = glm::mat4(1);
        rotorMatrix2 = transformMatrix * rotorMatrix2;
        rotorMatrix2 = glm::translate(rotorMatrix2, glm::vec3(-1.75f, 0.8f, 0));
        rotorMatrix2 = glm::scale(rotorMatrix2, glm::vec3(0.3f, 0.2f, 0.2f));
        rotorMatrix2 = glm::rotate(rotorMatrix2, drone.rotor_rotation, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["rotor"], shaders["VertexColor"], rotorMatrix2, camera);

        glm::mat4 rotorMatrix3 = glm::mat4(1);
        rotorMatrix3 = transformMatrix * rotorMatrix3;
        rotorMatrix3 = glm::translate(rotorMatrix3, glm::vec3(0, 0.8f, 1.75f));
        rotorMatrix3 = glm::scale(rotorMatrix3, glm::vec3(0.3f, 0.2f, 0.2f));
        rotorMatrix3 = glm::rotate(rotorMatrix3, drone.rotor_rotation, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["rotor"], shaders["VertexColor"], rotorMatrix3, camera);

        glm::mat4 rotorMatrix4 = glm::mat4(1);
        rotorMatrix4 = transformMatrix * rotorMatrix4;
        rotorMatrix4 = glm::translate(rotorMatrix4, glm::vec3(0, 0.8f, -1.75f));
        rotorMatrix4 = glm::scale(rotorMatrix4, glm::vec3(0.3f, 0.2f, 0.2f));
        rotorMatrix4 = glm::rotate(rotorMatrix4, drone.rotor_rotation, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["rotor"], shaders["VertexColor"], rotorMatrix4, camera);
    }


}

void Tema2::Update(float deltaTimeSeconds)
{

    if (new_packet) {
        GeneratePacket();
        new_packet = 0;
    }

    if (new_destination) {
        GenerateDestination();
        new_destination = 0;
    }

    if (IsCollidingSpheres(drone.position, packet.position, drone.scale, 3, 1, 1)) {
        packet.position = glm::vec3(0, -10, 0);
        packet_picked = 1;
        new_destination = 1;
    }

    if (destination.position.y > 0 &&
        glm::distance(glm::vec2(drone.position.x, drone.position.z), glm::vec2(destination.position.x, destination.position.z)) < 5) {
        destination.position.y = -20;
        packet_picked = 0;
        new_packet = 1;

        score++;
        std::cout << "Score: " << score << endl;
    }

    // Drone
    {
        if (drone.rotor_rotation < 2 * PI) {
            drone.rotor_rotation += rotation_speed * deltaTimeSeconds;
        }
        else {
            drone.rotor_rotation = 0;
        }
    }


    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene(camera);
    for (auto& cloud : clouds) {
        glm::mat4 cloudMatrix = glm::mat4(1);
        cloudMatrix = glm::translate(cloudMatrix, cloud.position);
        if (strcmp(cloud.type, "cloud0")) {
            cloudMatrix = glm::scale(cloudMatrix, glm::vec3(cloud.scale, 5, cloud.scale * 2));
        }
        if (strcmp(cloud.type, "cloud1")) {
            cloudMatrix = glm::scale(cloudMatrix, glm::vec3(cloud.scale, 5, cloud.scale));
        }
        if (strcmp(cloud.type, "cloud2")) {
            cloudMatrix = glm::scale(cloudMatrix, glm::vec3(cloud.scale * 2, 5, cloud.scale));
        }
        RenderSimpleMesh(meshes["cloud"], shaders["VertexColor"], cloudMatrix, camera);

        cloud.position.z += rand() % 10 * deltaTimeSeconds;

        if (cloud.position.z > z_max) {
            cloud.position.z = z_min;
        }
    }
    // DrawCoordinateSystem(camera->GetViewMatrix(), camera->projectionMatrix);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    RenderScene(minimap_camera);
    // DrawCoordinateSystem(camera->GetViewMatrix(), camera->projectionMatrix);

    glClear(GL_DEPTH_BUFFER_BIT);

}


void Tema2::FrameEnd()
{
    
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, my_Camera::Camera* camera)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    int location_model = glGetUniformLocation(shader->program, "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    int location_view = glGetUniformLocation(shader->program, "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glUniformMatrix4fv(location_view, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    int location_projection = glGetUniformLocation(shader->program, "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glUniformMatrix4fv(location_projection, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}



/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{

    drone.rotation_x = 0;
    drone.rotation_z = 0;

    if (window->KeyHold(GLFW_KEY_W)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position + drone.forward * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position + drone.forward * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (!collision) {
            drone.TranslateForward(drone.movement_speed * deltaTime);
            camera->MoveForward(drone.movement_speed * deltaTime);
            //minimap_camera->MoveForward(drone.movement_speed * deltaTime);
            minimap_camera->position.x = drone.position.x;
            minimap_camera->position.z = drone.position.z;
            drone.rotation_x = -0.5f;
        }
        
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position - drone.right * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position - drone.right * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (!collision) {
            drone.TranslateRight(-drone.movement_speed * deltaTime);
            camera->TranslateRight(-drone.movement_speed * deltaTime);
            //minimap_camera->TranslateRight(-drone.movement_speed * deltaTime);
            minimap_camera->position.x = drone.position.x;
            minimap_camera->position.z = drone.position.z;
            drone.rotation_z = 0.5f;
        }
        
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position - drone.forward * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position - drone.forward * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (!collision) {
            drone.TranslateForward(-drone.movement_speed * deltaTime);
            camera->MoveForward(-drone.movement_speed * deltaTime);
            //minimap_camera->MoveForward(-drone.movement_speed * deltaTime);
            minimap_camera->position.x = drone.position.x;
            minimap_camera->position.z = drone.position.z;
            drone.rotation_x = 0.5f;
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position + drone.right * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position + drone.right * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (!collision) {
            drone.TranslateRight(drone.movement_speed* deltaTime);
            camera->TranslateRight(drone.movement_speed* deltaTime);
            //minimap_camera->TranslateRight(drone.movement_speed* deltaTime);
            minimap_camera->position.x = drone.position.x;
            minimap_camera->position.z = drone.position.z;
            drone.rotation_z = -0.5f;

        }
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position + drone.up * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position + drone.up * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (!collision) {
            drone.position.y += drone.movement_speed * deltaTime;
            camera->TranslateUpward(drone.movement_speed* deltaTime);
        }
        
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        int collision = 0;
        for (const auto& obs : obstacles) {
            if (!strcmp(obs.type, "rock")) {
                if (IsCollidingSpheres(drone.position - drone.up * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 1.2f)) {
                    collision = 1;
                    break;
                }
            }
            if (!strcmp(obs.type, "tree")) {
                if (IsCollidingTree(drone.position - drone.up * (drone.movement_speed * deltaTime),
                    obs.position, drone.scale, obs.scale, 1, 2.5f)) {
                    collision = 1;
                    break;
                }
            }
        }

        if (drone.position.y - drone.movement_speed * deltaTime > 1.5f && !collision) {
            drone.position.y -= drone.movement_speed * deltaTime;
            camera->TranslateUpward(-drone.movement_speed * deltaTime);
        }
        
        
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections
    if (key == GLFW_KEY_P)
    {
        // perspective
        camera->projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);
    }

    if (key == GLFW_KEY_O)
    {
        // ortho
        camera->projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
    }

    if (key == GLFW_KEY_MINUS) {
        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            fov -= 0.1f;

            camera->projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);
        }
        else {
            left /= 1.1f;
            right /= 1.1f;
            top /= 1.1f;
            bottom /= 1.1f;

            camera->projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
        }
    }

    if (key == GLFW_KEY_EQUAL) {
        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            fov += 0.1f;

            camera->projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);
        }
        else {
            left *= 1.1f;
            right *= 1.1f;
            top *= 1.1f;
            bottom *= 1.1f;

            camera->projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
        }
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.

            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            
        }

        if (window->GetSpecialKeyState() == 0) {
            // renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);

            // minimap_camera->RotateFirstPerson_OY(-deltaX * sensivityOX);

            drone.RotateOY(-deltaX * sensivityOY);
            drone.rotation_y += -deltaX * sensivityOY;
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // TODO(student): Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    // TODO(student): Create the VBO and bind it
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // TODO(student): Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO = 0;
    // TODO(student): Create the IBO and bind it
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // TODO(student): Send indices data into the IBO buffer
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

    // TODO(student): Unbind the VAO
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
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
}