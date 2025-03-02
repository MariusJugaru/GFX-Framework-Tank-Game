#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

        class Projectile {
            public:
                float posX, posY;
                glm::vec3 moveVector;
                glm::mat3 projectileMatrix;

                Projectile(glm::mat3 projMatrix, float pX, float pY, glm::vec3 moveVec);
                ~Projectile();
        };

        class TerrainDeform {
            public:
                float xHit;
                int framesLeft;

                TerrainDeform(float hit);
                ~TerrainDeform();
        };

     private:
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

        void FrameStart() override;
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

        float Tema1::GetYPos(float xPos);
        float Tema1::GetAngularStep(float xPos);
        int Tema1::GetDivision(float currentXPos, int squareWidth);
        float Tema1::GetClosestXPosIdx(float currentXPos, int squareWidth);
        std::vector<float> GenerateHeightMap(int pointsNumber, int squareWidth, int width);
        void Tema1::CreateTank(glm::vec3 color1, glm::vec3 color2);
        void Tema1::CreateCircle(glm::vec3 color);
        void Tema1::CreateTankGun();
        Mesh* Tema1::GetTrajectory(glm::vec3 moveVector);
        void Tema1::CreateTrajectoryLine();

        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        float rotate;
        glm::mat3 sqrMatrix1;
        glm::mat3 sqrMatrix2;
        glm::mat3 sqrMatrix3;


        float offset;

     protected:
         glm::ivec2 resolution;
         std::vector<float> heightMap;

         // terrain
         glm::mat3 sqrMatrix;
         int pointsNumber;
         int squareWidth;
         float heightScale;
         float heightOffset;

         // tanks
         float tankSpeed;
         float tankHeightOffset;
         float healthBarHeightOffset;
         float healthBarXOffset;
         float healthBarSize;

         glm::mat3 tankMatrix1;
         glm::mat3 gunMatrix1;
         glm::mat3 trajectoryMatrix1;
         glm::mat3 healthBar1;
         glm::mat3 healthBarWidth1;
         glm::mat3 healthBarHeight1;

         float tankXPos1, tankYPos1;
         float tankAngularStep1;
         float gunAngularStep1;
         int tankHealth1;
         Mesh* trajectory1;

         float trajectoryStart1, trajectoryEnd1, trajectoryHigh1;
         
         
         glm::mat3 tankMatrix2;
         glm::mat3 gunMatrix2;
         glm::mat3 trajectoryMatrix2;
         glm::mat3 healthBar2;
         glm::mat3 healthBarWidth2;
         glm::mat3 healthBarHeight2;

         float tankXPos2, tankYPos2;
         float tankAngularStep2;
         float gunAngularStep2;
         int tankHealth2;
         Mesh* trajectory2;

         float trajectoryStart2, trajectoryEnd2, trajectoryHigh2;

         int tankNo;
         int circleNo;

         float t;
         int aX, bX;
         float aY, bY;

         std::vector<Projectile> projectiles;
         float projectileSpeed;

         std::vector<float> blastRadiusVector;
         int blastRadius;
        
         std::vector<TerrainDeform> landSlides;
         float threshold;

         glm::vec3 moveVector;
         float cannonPosX, cannonPosY;

         int trajectoryChanged1, trajectoryChanged2;
         float deltaTimeTrajectory;

         float wait_for_load;
         int loaded;

    };
}   // namespace m1
