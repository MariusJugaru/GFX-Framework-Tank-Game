#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/my_camera.h"


namespace m1
{
    class Drone
    {
        public:
            glm::vec3 position;
            glm::vec3 forward;
            glm::vec3 right;
            glm::vec3 up;

            glm::mat4 baseMatrix1;
            glm::mat4 baseMatrix2;

            glm::mat4 cubeMatrix1;
            glm::mat4 cubeMatrix2;
            glm::mat4 cubeMatrix3;
            glm::mat4 cubeMatrix4;

            glm::mat4 rotorMatrix1;
            glm::mat4 rotorMatrix2;
            glm::mat4 rotorMatrix3;
            glm::mat4 rotorMatrix4;

            
            float rotation_x, rotation_y, rotation_z;
            float scale;

            float movement_speed;
            float rotor_rotation;

            Drone();
            Drone(float pos_x, float pos_y, float pos_z);
            ~Drone();

            void TranslateForward(float distance);
            void TranslateUpward(float distance);
            void TranslateRight(float distance);
            void Drone::RotateOX(float angle);
            void Drone::RotateOY(float angle);

    };

    class Obstacle
    {
        public:
            Obstacle();
            Obstacle(float pos_x, float pos_y, float pos_z, float rotation_x, float rotation_y, float rotation_z, float obj_scale, char *obj_type);
            Obstacle::Obstacle(glm::vec3 pos, glm::vec3 rot, float obj_scale, char *obj_type);
            ~Obstacle();

            char *type;
            glm::vec3 position;
            glm::vec3 rotation;
            float scale;
    };

    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, my_Camera::Camera* camera);
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void CreateRectangle(char* name, glm::vec3 color);
        void CreateBox(char* name, glm::vec3 color);
        void CreateTree(char* name, glm::vec3 color1, glm::vec3 color2);
        void CreateRock(char* name, glm::vec3 color);
        void CreateArrow(char* name, glm::vec3 color);
        void CreateCylinder(char* name, glm::vec3 color);
        void GenerateTerrain(char* name, glm::vec3 color, int size);
        void GenerateObstacles();
        void GeneratePacket();
        void GenerateDestination();
        void GenerateClouds();

        int IsCollidingSpheres(glm::vec3 pos1, glm::vec3 pos2, float rad1, float rad2, float max1, float max2);
        int IsCollidingTree(glm::vec3 pos1, glm::vec3 pos2, float rad1, float rad2, float max1, float max2);

        void FrameStart() override;
        void RenderScene(my_Camera::Camera* camera);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        my_Camera::Camera* camera;
        my_Camera::Camera* minimap_camera;

        bool renderCameraTarget;
        ViewportArea miniViewportArea;
        glm::vec3 target_pos;

        // camera
        float fov;
        float zNear;
        float zFar;

        float left;
        float right;
        float bottom;
        float top;


        // drone
        Drone drone;
        float rotation_speed;


        // terrain
        int num_squares;
        

        // obstacles
        int num_obstacles;
        float x_min, x_max;
        float z_min, z_max;
        std::vector<Obstacle> obstacles;
        Obstacle packet;
        Obstacle destination;

        // packets
        int new_packet;
        int packet_picked;
        int new_destination;

        int score;

        // clouds
        std::vector<Obstacle> clouds;
        int num_clouds;
    };
}   // namespace m1