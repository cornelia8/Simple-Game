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

    private:
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

        void createPlayer(glm::vec3 corner, int playerSize, int triangles, glm::vec3 playerColor);
        void drawPlayer(glm::mat3 modelMatrix);

        void createEnemy(glm::vec3 enemyCorner, int enemySize, glm::vec3 enemyColor);
        void drawEnemy(glm::mat3 modelMatrix);
        glm::vec3 enemySpawningPos();
        int EnemySpeed();
        glm::vec3 EnemyMovement(float deltaTimeSeconds, int speed, glm::vec3 enemyPos);
        glm::vec3 ProjectileMovement(float deltaTimeSeconds, int speed, glm::vec3 projectilePos, glm::vec3 direction);

        glm::vec3* deleteFromVector(glm::vec3* vector, int size, int position);
        float* deleteFromVectorF(float* vector, int size, int position);
        int AABBcollision(glm::vec3 obj1_min, glm::vec3 obj1_max, glm::vec3 obj2_min, glm::vec3 obj2_max);
        int playerCanMove(glm::vec3 player);
        int enemyCanMove(glm::vec3 enemy);
        int projCanMove(glm::vec3 proj);
        bool enemyMetPlayer(glm::vec3 player, glm::vec3 enemy);
        int projectileMetEnemy(glm::vec3* enemies, int enemyCount, glm::vec3 projectile);
        // void showScore(int score, glm::vec3 pos);

    protected:
        glm::mat3 modelMatrix;
        float playerPosX, playerPosY;
        float angularStep, enemyStep, projectileStep;

        int enemySize;
        int playerSize;
        int projectileSize;
        int triangles;
        int playerSpeed;
        float health;
        bool shoot;

        glm::vec3* enemyPoz;
        float* enemySpeed;
        int enemyCount;

        glm::vec3* projectilePoz;
        glm::vec3* projectileDir;
        int projectileSpeed;
        int projectileCount;

        glm::vec3 playerCenter;
        glm::vec3 enemyCorner;
        glm::vec3 projectileCorner;
        glm::vec3 cameraPos;

        float timer;
        float shootingTimer;
        float* projectileLife;
        int score;
    };
}   // namespace m1
