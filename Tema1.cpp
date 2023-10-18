#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/Transform2D.h"
#include "lab_m1/Tema1/GameObject.h"
// #include "lab_m1/Tema1/glut.h"  <- if i do this I get "cannot open file 'glut32.lib'

#include <vector>
#include <iostream>
#include <random>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>  <- this one won't work

using namespace std;
using namespace m1;

Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

void Tema1::Init()
{
    //glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    cameraPos = glm::vec3(0, 0, 50);
    camera->SetOrthographic(0, 1280, 0, 720, 0.01f, 400);
    camera->SetPosition(cameraPos);
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(630, 350, 0);
    glm::vec3 playerColor = glm::vec3(0.8, 0.7, 0);
    glm::vec3 enemyColor = glm::vec3(0.7f, 0.1f, 0.1f);
    glm::vec3 projectileColor = glm::vec3(0, 0, 0);
    srand(time(0));

    playerPosX = 0;
    playerPosY = 0;
    angularStep = 0;
    playerSpeed = 300;
    playerSize = 50;
    health = 100;
    shoot = false;

    enemySize = 100;
    enemyCount = 0;
    enemyStep = 0;
    enemyCount = 0;
    enemyPoz = new glm::vec3[100];
    enemySpeed = new float[100];

    projectilePoz = new glm::vec3[100];
    projectileDir = new glm::vec3[100];
    projectileLife = new float[100];
    projectileSpeed = 550;
    projectileCount = 0;
    projectileSize = 26;
    projectileStep = 0;

    triangles = 60;
    timer = 0.f;
    score = 0;
    shootingTimer = 1.f;

    playerCenter = corner;
    enemyCorner = corner - glm::vec3(playerSize, playerSize, 0);
    projectileCorner = playerCenter - glm::vec3(projectileSize / 2, projectileSize / 2, 0);

    createPlayer(corner, playerSize, triangles, playerColor);
    createEnemy(enemyCorner, enemySize, enemyColor);

    Mesh* projectile = GameObject::CreateSquare("projectile", projectileCorner, projectileSize, projectileColor, true);
    AddMeshToList(projectile);

    //map & obstacles
    Mesh* map = GameObject::CreateRectangle("map", glm::vec3(-360, -390, 0), 2000, 1500, glm::vec3(0.1f, 0.15f, 0.2f), true);
    AddMeshToList(map);

    Mesh* obstacle1 = GameObject::CreateRectangle("obstacle1", glm::vec3(-100, -50, 0), 430, 80, glm::vec3(0.5f, 0.55f, 0.6f), true);
    AddMeshToList(obstacle1);

    Mesh* obstacle2 = GameObject::CreateRectangle("obstacle2", glm::vec3(40, 400, 0), 110, 310, glm::vec3(0.5f, 0.55f, 0.6f), true);
    AddMeshToList(obstacle2);

    Mesh* obstacle3 = GameObject::CreateRectangle("obstacle3", glm::vec3(1100, 680, 0), 170, 120, glm::vec3(0.5f, 0.55f, 0.6f), true);
    AddMeshToList(obstacle3);

    Mesh* obstacle4 = GameObject::CreateRectangle("obstacle4", glm::vec3(1100, 40, 0), 100, 400, glm::vec3(0.5f, 0.55f, 0.6f), true);
    AddMeshToList(obstacle4);

    Mesh* obstacle5 = GameObject::CreateRectangle("obstacle5", glm::vec3(40, 710, 0), 430, 110, glm::vec3(0.5f, 0.55f, 0.6f), true);
    AddMeshToList(obstacle5);

    //obstacle 1 blc, brc, tlc, trc:  (-100, -50), (330, -50), (-100, 30), (330, 30)
    //obstacle 2 blc, brc, tlc, trc:  (40, 400), (150, 400), (40, 710), (150, 710)
    //obstacle 3 blc, brc, tlc, trc:  (1100, 680), (1270, 680), (1110, 800), (1270, 800)
    //obstacle 4 blc, brc, tlc, trc:  (1100, 40), (1200, 40), (1110, 440), (1200, 440)
    //obstacle 5 blc, brc, tlc, trc:  (40, 710), (470, 710), (40, 820), (470, 820)

    //UI elements

    Mesh* healthBar = GameObject::CreateRectangle("healthBar", corner - glm::vec3(playerSize, playerSize + 30, 0), 100, 20, glm::vec3(0.1f, 0.6f, 0.1f), true);
    AddMeshToList(healthBar);
}


void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    timer += deltaTimeSeconds;
    shootingTimer += deltaTimeSeconds;

    // Player section
    // player movement
    modelMatrix = glm::mat3(1);
    modelMatrix *= Transform2D::Translate(playerPosX, playerPosY);

    glm::mat3 healthBarMatrix = modelMatrix;

    // player rotation
    modelMatrix *= Transform2D::Translate(630, 350);
    modelMatrix *= Transform2D::Rotate(- angularStep);
    modelMatrix *= Transform2D::Translate(-630, -350);

    drawPlayer(modelMatrix);

    // draw healthbar
    healthBarMatrix *= Transform2D::Translate(630, 350);
    healthBarMatrix *= Transform2D::Scale(health/100, 1);
    healthBarMatrix *= Transform2D::Translate(-630, -350);
    RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], healthBarMatrix);

    // draw score
    // glm::vec3 scorePos = glm::vec3(10, 10, 0);
    // showScore(score, scorePos);

    //camera follows player
    auto camera = GetSceneCamera();
    camera->SetPosition(cameraPos + glm::vec3(playerPosX, playerPosY, 0));

    // Projectile section
    // to spawn a projectile in the center of the player
    if(shoot)
    {
        // create a projectile
        projectilePoz[projectileCount] = glm::vec3(playerPosX, playerPosY, 0);
        projectileLife[projectileCount] = 0;
        projectileCount++;
        // direction should be calculated and set on click
        shoot = false;
    }
    projectileStep += deltaTimeSeconds * projectileSpeed / 5;
    // the array of projectiles and their movement, similar to the enemy spawner
    for (int i = 0; i < projectileCount; i++)
    {
        //projectile life timers
        projectileLife[i] += deltaTimeSeconds;

        modelMatrix = glm::mat3(1);
        projectilePoz[i] = ProjectileMovement(deltaTimeSeconds, projectileSpeed, projectilePoz[i], projectileDir[i]);
        modelMatrix *= Transform2D::Translate(projectilePoz[i].x, projectilePoz[i].y);
        // constant rotation
        modelMatrix *= Transform2D::Translate(630, 350);
        modelMatrix *= Transform2D::Rotate(projectileStep);
        modelMatrix *= Transform2D::Translate(-630, -350);
        RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);

        // despawn projectile after 5 s or if it met a wall
        // the timer exists in case the collisions fail
        if(projectileLife[i] >= 5.f || (projCanMove(projectilePoz[i]) > 0))
        {
            projectilePoz = deleteFromVector(projectilePoz, projectileCount, i);
            projectileDir = deleteFromVector(projectileDir, projectileCount, i);
            projectileLife = deleteFromVectorF(projectileLife, projectileCount, i);
            projectileCount--;
        }

        // check if the projectile hit any enemy
        int index = projectileMetEnemy(enemyPoz, enemyCount, projectilePoz[i]);
        
        // if it did, increase the score, despawn the projectile and the enemy
        if (index != -1)
        {
            // despawn projectile
            projectilePoz = deleteFromVector(projectilePoz, projectileCount, i);
            projectileDir = deleteFromVector(projectileDir, projectileCount, i);
            projectileLife = deleteFromVectorF(projectileLife, projectileCount, i);
            projectileCount--;

            // despawn enemy
            enemyPoz = deleteFromVector(enemyPoz, enemyCount, index);
            enemySpeed = deleteFromVectorF(enemySpeed, enemyCount, index);
            enemyCount--;

            score += 50;
            printf("Score is now: %d\n", score);
        }
    }

    // Enemies section
    // spawn enemy
    if (enemyCount == 0 || timer >= 3.f)
    {
        enemyPoz[enemyCount] = enemySpawningPos();
        enemySpeed[enemyCount] = EnemySpeed();
        enemyCount++;
        timer = 0.f;
    }

    for(int i = 0; i < enemyCount; i++)
    {
        modelMatrix = glm::mat3(1);
        enemyPoz[i] = EnemyMovement(deltaTimeSeconds, enemySpeed[i], enemyPoz[i]);
        modelMatrix *= Transform2D::Translate(enemyPoz[i].x, enemyPoz[i].y);
        drawEnemy(modelMatrix);

        bool check = enemyMetPlayer(glm::vec3(playerPosX, playerPosY, 0), enemyPoz[i]);
        if (check)
        {
            // player takes damage
            if (health > 0)
            {
                health -= 10;
            }

            // despawn enemy and its speed
            enemyPoz = deleteFromVector(enemyPoz, enemyCount, i);
            enemySpeed = deleteFromVectorF(enemySpeed, enemyCount, i);
            enemyCount--;
        }
    }
    
    // look at player <- not working
    /*enemyStep = atan2(enemyPoz.y - playerPosY, enemyPoz.x - playerPosX) + atan(90);
    modelMatrix *= Transform2D::Translate(630 - playerSize + enemyPoz.x, 350 - playerSize + enemyPoz.y);
    modelMatrix *= Transform2D::Rotate(- enemyStep);
    modelMatrix *= Transform2D::Translate(- 630 + playerSize - enemyPoz.x, - 350 + playerSize - enemyPoz.y);*/

    //draw fixed map and obstacles
    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["obstacle1"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["obstacle2"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["obstacle3"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["obstacle4"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["obstacle5"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["map"], shaders["VertexColor"], modelMatrix);
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
    int check = 0;
    //constant player movement
    if (window->KeyHold(GLFW_KEY_W))
    {
        if (window->KeyHold(GLFW_KEY_A))
        {
            // move up
            playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            // move left
            playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch(check)
            {
                // player can't move left
                case 2: 
                    playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
                // player can't move up
                case 3: 
                    playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        if (window->KeyHold(GLFW_KEY_D))
        {
            // move up
            playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            // move right
            playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move right
            case 1:
                playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
                // player can't move up
            case 3:
                playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        {
            // move up
            playerPosY = playerPosY + deltaTime * playerSpeed;
            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            // player can't move up
            if(check == 3)
                playerPosY = playerPosY - deltaTime * playerSpeed;
        }
    }
    else
    if (window->KeyHold(GLFW_KEY_A))
    {
        if (window->KeyHold(GLFW_KEY_W))
        {
            // move left
            playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
            // move up
            playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move left
            case 2:
                playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
                // player can't move up
            case 3:
                playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        if (window->KeyHold(GLFW_KEY_S))
        {
            // move left
            playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
            // move down
            playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move left
            case 2:
                playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
                // player can't move down
            case 4:
                playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        {
            // move left
            playerPosX = playerPosX - deltaTime * playerSpeed;
            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            // playet can't move left
            if(check == 2)
                playerPosX = playerPosX + deltaTime * playerSpeed;
        }
    }
    else
    if (window->KeyHold(GLFW_KEY_S))
    {
        if (window->KeyHold(GLFW_KEY_A))
        {
            // move down
            playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            // move left
            playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move left
            case 2:
                playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
                // player can't move down
            case 4:
                playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        if (window->KeyHold(GLFW_KEY_D))
        {
            // move down
            playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            // move right
            playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move right
            case 1:
                playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
                // player can't move down
            case 4:
                playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        {
            // move down
            playerPosY = playerPosY - deltaTime * playerSpeed;
            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            //player can't move down
            if(check == 4)
                playerPosY = playerPosY + deltaTime * playerSpeed;
        }
    }
    else
    if (window->KeyHold(GLFW_KEY_D))
    {
        if (window->KeyHold(GLFW_KEY_W))
        {
            // move right
            playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
            // move up
            playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move right
            case 1:
                playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
                // player can't move up
            case 3:
                playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        if (window->KeyHold(GLFW_KEY_S))
        {
            // move right
            playerPosX = playerPosX + deltaTime * playerSpeed / sqrt(2);
            // move down
            playerPosY = playerPosY - deltaTime * playerSpeed / sqrt(2);

            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            switch (check)
            {
                // player can't move right
            case 1:
                playerPosX = playerPosX - deltaTime * playerSpeed / sqrt(2);
                // player can't move down
            case 4:
                playerPosY = playerPosY + deltaTime * playerSpeed / sqrt(2);
            }
        }
        else
        {
            // move right
            playerPosX = playerPosX + deltaTime * playerSpeed;
            check = playerCanMove(glm::vec3(playerPosX, playerPosY, 0));
            // player can't move right
            if (check == 1)
                playerPosX = playerPosX - deltaTime * playerSpeed;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    // healthbar testing - manually remove or add health
    if (key == GLFW_KEY_O)
    {
        if(health > 0)
        {
            health = health - 10;
            printf("health: %f\n", health);
        }
    }
    if (key == GLFW_KEY_P)
    {
        if (health < 100)
        {
            health = health + 10;
            printf("health: %f\n", health);
        }
    }

    // get player position
    if (key == GLFW_KEY_L)
    {
        printf("Player position: %f, %f\n", playerPosX, playerPosY);
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    // din moment ce ochii i-am pus la 90 de grade pe cercul trigonometric, adaug atan(90) in ecuatie
    angularStep = atan2(mouseY - 360, mouseX - 640) + atan(90);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // I seem to keep failing at calculating the position
    // 
    // Add mouse button press event
    // can shoot ever 0.3 s
    if (shootingTimer > 0.3f)
    {
        shoot = true;
        float difX, difY, s;

        difX = abs(640 - mouseX);
        difY = abs(360 - mouseY);
        s = difX + difY;

        // calculate the direction
        // noticed the mouse y is reversed
        if (mouseX > 640)
        {
            if (mouseY < 360) 
            {
                //cadran 1
                //set direction towards the corresponding edge of the map
                projectileDir[projectileCount].x = difX / s;
                projectileDir[projectileCount].y = difY / s;
                //printf("cadran 1\n");
            }
            else
            {
                //cadran 4
                //set direction towards the corresponding edge of the map
                projectileDir[projectileCount].x = difX / s;
                projectileDir[projectileCount].y = - difY / s;
                //printf("cadran 4\n");
            }
        }
        else
        {
            if (mouseY < 360)
            {
                //cadran 2
                //set direction towards the corresponding edge of the map
                projectileDir[projectileCount].x = - difX / s;
                projectileDir[projectileCount].y = difY / s;
                //printf("cadran 2\n");
            }
            else 
            {
                //set direction towards the corresponding edge of the map
                projectileDir[projectileCount].x = - difX / s;
                projectileDir[projectileCount].y = - difY / s;
                //printf("cadran 3\n");
            }
        }

        //printf("Steps for projectile nr %d: %f, %f\n", projectileCount, projectileDir[projectileCount].x, projectileDir[projectileCount].y);
        shootingTimer = 0;
    }
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

void Tema1::createPlayer(glm::vec3 corner, int playerSize, int triangles, glm::vec3 playerColor)
{
    float eyeSize = playerSize / 3;
    glm::vec3 leftEyeCenter = playerCenter + glm::vec3(-playerSize / 2, playerSize, 0) + glm::vec3(0, - eyeSize / 2, 0);
    glm::vec3 rightEyeCenter = playerCenter + glm::vec3(playerSize / 2, playerSize, 0) + glm::vec3(0, - eyeSize / 2, 0);

    Mesh* playerLefEyeOutline = GameObject::CreateCircle("playerLeftEyeOutline", leftEyeCenter, eyeSize, triangles, glm::vec3(0.5f, 0.2f, 0));
    AddMeshToList(playerLefEyeOutline);
    Mesh* playerRightEyeOutline = GameObject::CreateCircle("playerRightEyeOutline", rightEyeCenter, eyeSize, triangles, glm::vec3(0.5f, 0.2f, 0));
    AddMeshToList(playerRightEyeOutline);

    Mesh* playerLeftEye = GameObject::CreateCircle("playerLeftEye", leftEyeCenter, eyeSize, triangles, playerColor, true);
    AddMeshToList(playerLeftEye);
    Mesh* playerRightEye = GameObject::CreateCircle("playerRightEye", rightEyeCenter, eyeSize, triangles, playerColor, true);
    AddMeshToList(playerRightEye);

    Mesh* playerBodyOutline = GameObject::CreateCircle("playerBodyOutline", playerCenter, playerSize, triangles, glm::vec3(0.5f, 0.2f, 0));
    AddMeshToList(playerBodyOutline);
    Mesh* playerBody = GameObject::CreateCircle("playerBody", playerCenter, playerSize, triangles, playerColor, true);
    AddMeshToList(playerBody);
}

void Tema1::drawPlayer(glm::mat3 modelMatrix)
{
    RenderMesh2D(meshes["playerLeftEyeOutline"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["playerRightEyeOutline"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["playerLeftEye"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["playerRightEye"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["playerBodyOutline"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["playerBody"], shaders["VertexColor"], modelMatrix);
}

void Tema1::createEnemy(glm::vec3 enemyCorner, int enemySize, glm::vec3 enemyColor)
{
    float eyeSize = enemySize / 3;
    glm::vec3 leftEyeCorner = enemyCorner + glm::vec3(- enemySize / 4 + enemySize / 2, enemySize, 0) + glm::vec3(- eyeSize / 2, - eyeSize / 2, 0);
    glm::vec3 rightEyeCorner = enemyCorner + glm::vec3(enemySize / 4 + enemySize / 2, enemySize, 0) + glm::vec3(- eyeSize / 2, - eyeSize / 2, 0);

    Mesh* enemyLefEyeOutline = GameObject::CreateSquare("enemyLeftEyeOutline", leftEyeCorner, eyeSize, glm::vec3(0.3f, 0, 0));
    AddMeshToList(enemyLefEyeOutline);
    Mesh* enemyRightEyeOutline = GameObject::CreateSquare("enemyRightEyeOutline", rightEyeCorner, eyeSize, glm::vec3(0.3f, 0, 0));
    AddMeshToList(enemyRightEyeOutline);

    Mesh* enemyLeftEye = GameObject::CreateSquare("enemyLeftEye", leftEyeCorner, eyeSize, enemyColor, true);
    AddMeshToList(enemyLeftEye);
    Mesh* enemyRightEye = GameObject::CreateSquare("enemyRightEye", rightEyeCorner, eyeSize, enemyColor, true);
    AddMeshToList(enemyRightEye);

    Mesh* enemyBodyOutline = GameObject::CreateSquare("enemyBodyOutline", enemyCorner, enemySize, glm::vec3(0.3f, 0, 0));
    AddMeshToList(enemyBodyOutline);
    Mesh* enemyBody = GameObject::CreateSquare("enemyBody", enemyCorner, enemySize, enemyColor, true);
    AddMeshToList(enemyBody);
}

void Tema1::drawEnemy(glm::mat3 modelMatrix)
{
    RenderMesh2D(meshes["enemyLeftEyeOutline"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["enemyRightEyeOutline"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["enemyLeftEye"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["enemyRightEye"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["enemyBodyOutline"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["enemyBody"], shaders["VertexColor"], modelMatrix);
}

glm::vec3 Tema1::enemySpawningPos() 
{
    // max spawning range based on fixed res 
    // -1000 : (1000 - enemySize) ; - 750 : (750  - enemySize - (enemySize / 6))
    // left some space for the eys
    // calculate total posibilities for x and y
    int xValues, yValues;
    xValues = 2001 - enemySize;
    yValues = 1501 - enemySize - (enemySize / 6);

    // we shouldn't spawn an enemy where the player cam see them
    int minX, minY,maxX, maxY;
    minX = -640 - enemySize + playerPosX;
    minY = -360 - enemySize - (enemySize / 6) - playerPosX;
    maxX = 640 + playerPosX;
    maxY = 360 - playerPosX;

    // generating spawning coordinates
    int enemyX, enemyY;

    enemyX = rand() % xValues - 1000;
    enemyY = rand() % yValues - 750;

    if(enemyX < maxX && enemyX > minX)
    {
        // if x is too small, send it just outside the screen
        if ((maxX - enemyX) < (enemyX - minX))
        {
            //check map boundries
            if (maxX > 1000)
            {
                enemyX = minX;
            }
            else
            {
                enemyX = maxX;
            }            
        }
        else
        {
            //check map boundries
            if (minX < -1000)
            {
                enemyX = maxX;
            }
            else
            {
                enemyX = minX;
            }
        }
    }
    if (enemyY < maxY && enemyY > minY)
    {
        // if y is too small, send it just outside the screen
        if ((maxY - enemyY) < (enemyY - minY))
        {
            //check map boundries
            if (maxY > 750) 
            {
                enemyY = minY;
            }
            else
            {
                enemyY = maxY;
            }
        }
        else
        {
            if (minY < -750)
            {
                enemyY = maxY;
            }
            else
            {
                enemyY = minY;
            }
        }
    }
    
    return glm::vec3(enemyX, enemyY, 0);
}

int Tema1::EnemySpeed()
{
    // generate a random speed for the enemy in range of (5 * 10 : 20 * 10)
    int speed = rand() % 15 + 5;
    return speed * 10;
}

// enemy map collision seems rater redundant since they'll always follow the player
// and the player can't get out of the map
glm::vec3 Tema1::EnemyMovement(float deltaTimeSeconds, int speed, glm::vec3 enemyPos)
{
    int check = 0;
    if (enemyPos.x < playerPosX)
    {
        if (enemyPos.y < playerPosY)
        {
            // move right
            enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
            // move up
            enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch(check)
            {
                // can't move right
                case 1:
                    enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
                // can't move up
                case 3:
                    enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        if (enemyPos.y > playerPosY)
        {
            // move right
            enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
            // move down
            enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move right
            case 1:
                enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
                // can't move down
            case 4:
                enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        {
            // move right
            enemyPos.x = enemyPos.x + deltaTimeSeconds * speed;
            check = enemyCanMove(enemyPos);
            if (check == 1)
                enemyPos.x = enemyPos.x - deltaTimeSeconds * speed;
        }
    }
    else
    if (enemyPos.x > playerPosX)
    {
        if (enemyPos.y < playerPosY)
        {
            // move left
            enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
            // move up
            enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move left
            case 2:
                enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
                // can't move up
            case 3:
                enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        if (enemyPos.y > playerPosY)
        {
            // move left
            enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
            // move down
            enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move left
            case 2:
                enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
                // can't move down
            case 4:
                enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        {
            // move left
            enemyPos.x = enemyPos.x - deltaTimeSeconds * speed;
            check = enemyCanMove(enemyPos);
            if (check == 2)
                enemyPos.x = enemyPos.x + deltaTimeSeconds * speed;
        }
    }
    else
    if (enemyPos.y < playerPosY)
    {
        if (enemyPos.x < playerPosX)
        {
            // move up
            enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            // move right
            enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move right
            case 1:
                enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
                // can't move up
            case 3:
                enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        if (enemyPos.x > playerPosX)
        {
            // move up
            enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            // move left
            enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move left
            case 2:
                enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
                // can't move up
            case 3:
                enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        {
            // move up
            enemyPos.y = enemyPos.y + deltaTimeSeconds * speed;
            check = enemyCanMove(enemyPos);
            if (check == 3)
                enemyPos.y = enemyPos.y - deltaTimeSeconds * speed;
        }
    }
    else
    if (enemyPos.y > playerPosY)
    {
        if (enemyPos.x < playerPosX)
        {
            // move down
            enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            // move right
            enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move right
            case 1:
                enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);
                // can't move down
            case 4:
                enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        if (enemyPos.x > playerPosX)
        {
            // move down
            enemyPos.y = enemyPos.y - deltaTimeSeconds * speed / sqrt(2);
            // move right
            enemyPos.x = enemyPos.x - deltaTimeSeconds * speed / sqrt(2);

            check = enemyCanMove(enemyPos);
            switch (check)
            {
                // can't move left
            case 2:
                enemyPos.x = enemyPos.x + deltaTimeSeconds * speed / sqrt(2);
                // can't move down
            case 4:
                enemyPos.y = enemyPos.y + deltaTimeSeconds * speed / sqrt(2);
            }
        }
        else
        {
            // move down
            enemyPos.y = enemyPos.y - deltaTimeSeconds * speed;
            check = enemyCanMove(enemyPos);
            if (check == 4)
                enemyPos.y = enemyPos.y + deltaTimeSeconds * speed;
        }
    }

    return enemyPos;
}

glm::vec3 Tema1::ProjectileMovement(float deltaTimeSeconds, int speed, glm::vec3 projectilePos, glm::vec3 direction) 
{
    projectilePos.x += direction.x * deltaTimeSeconds * speed;
    projectilePos.y += direction.y * deltaTimeSeconds * speed;
    
    return projectilePos;
}

glm::vec3* Tema1::deleteFromVector(glm::vec3* vector, int size, int position) 
{
    for (int i = position; i < size-1; i++)
    {
        vector[i] = vector[i + 1];
    }
    vector[size - 1] = glm::vec3(0);

    return vector;
    // don't forget to change the size after deleting an entry
}

float* Tema1::deleteFromVectorF(float* vector, int size, int position)
{
    for (int i = position; i < size - 1; i++)
    {
        vector[i] = vector[i + 1];
    }
    vector[size - 1] = 0.f;

    return vector;
    // don't forget to change the size after deleting an entry
}

int Tema1::AABBcollision(glm::vec3 obj1_min, glm::vec3 obj1_max, glm::vec3 obj2_min, glm::vec3 obj2_max)
{
    // detect collision
    if (!(obj1_max.x < obj2_min.x || obj1_min.x > obj2_max.x) && !(obj1_max.y < obj2_min.y || obj1_min.y > obj2_max.y))
    {
        // can't move right
        if (obj1_max.x < obj2_max.x)
            return 1;
        // can't move left
        if (obj1_min.x > obj2_min.x)
            return 2;
        // can't move up
        if (obj1_max.y < obj2_max.y)
            return 3;
        // can't move down
        if (obj1_min.y > obj2_min.y)
            return 4;
    }

    // return 0 if no collision
    return 0;
}

// 0 - player can move freely
// 1 - player can't go right // for D key
// 2 - player can't go left // for A key
// 3 - player can't go up // for W key
// 4 - player can't go down // for S key
int Tema1::playerCanMove(glm::vec3 player)
{   
    glm::vec3 playerMinPos = player - glm::vec3(playerSize + playerSize / 3, playerSize + playerSize / 3, 0); //considering the eyes
    glm::vec3 playerMaxPos = player + glm::vec3(playerSize, playerSize, 0); // or i could substract from here and not consider the eyes

    // if player is too far to the right, return 1
    if (playerMaxPos.x >= 1000)
    {
        return 1;
    }
    // if player is too far to the left, return 2
    if (playerMinPos.x <= -1000)
    {
        return 2;
    }
    // if player is too far up, return 3
    if (playerMaxPos.y >= 750)
    {
        return 3;
    }
    // if player is too far down, return 4
    if (playerMinPos.y <= -750)
    {
        return 4;
    }
    
    // else, we check with the obstacles
    /*
    // obstacle 1
    glm::vec3 obstacle1MinPos = glm::vec3(-100, -50, 0);
    glm::vec3 obstacle1MaxPos = glm::vec3(330, 30, 0);

    // obstacle 2
    glm::vec3 obstacle2MinPos = glm::vec3(40, 400, 0);
    glm::vec3 obstacle2MaxPos = glm::vec3(150, 710, 0);

    // obstacle 3
    glm::vec3 obstacle3MinPos = glm::vec3(1100, 680, 0);
    glm::vec3 obstacle3MaxPos = glm::vec3(1270, 800, 0);

    // obstacle 4
    glm::vec3 obstacle4MinPos = glm::vec3(1100, 40, 0);
    glm::vec3 obstacle4MaxPos = glm::vec3(1200, 440, 0);

    //obstacle 5
    glm::vec3 obstacle5MinPos = glm::vec3(40, 710, 0);
    glm::vec3 obstacle5MaxPos = glm::vec3(470, 820, 0);
    */
    glm::vec3 offset = glm::vec3(-640, -360, 0);

    //obstacle 1 check
    int check = AABBcollision(playerMinPos, playerMaxPos, glm::vec3(-100, -50, 0) + offset, glm::vec3(330, 30, 0) + offset);
    if (check != 0) // detected collision with obstacle 1
    {
        return check;
    }
    
    //obstacle 2 check
    check = AABBcollision(playerMinPos, playerMaxPos, glm::vec3(40, 400, 0) + offset, glm::vec3(150, 710, 0) + offset);
    if (check != 0) // detected collision with obstacle 2
    {
        return check;
    }

    //obstacle 3 check
    check = AABBcollision(playerMinPos, playerMaxPos, glm::vec3(1100, 680, 0) + offset, glm::vec3(1270, 800, 0) + offset);
    if (check != 0) // detected collision with obstacle 3
    {
        return check;
    }
    //obstacle 4 check
    check = AABBcollision(playerMinPos, playerMaxPos, glm::vec3(1100, 40, 0) + offset, glm::vec3(1200, 440, 0) + offset);
    if (check != 0) // detected collision with obstacle 4
    {
        return check;
    }
    //obstacle 5 check
    check = AABBcollision(playerMinPos, playerMaxPos, glm::vec3(40, 710, 0) + offset, glm::vec3(470, 820, 0) + offset);
    if (check != 0) // detected collision with obstacle 5
    {
        return check;
    }

    // no collission detected
    return 0;
}

bool Tema1::enemyMetPlayer(glm::vec3 player, glm::vec3 enemy)
{
    glm::vec3 playerMinPos = player; // seems like this is the bottom left corner instead of the center
    glm::vec3 playerMaxPos = player + glm::vec3(2*playerSize, 2*playerSize, 0);

    glm::vec3 enemyMinPos = enemy; // this should be the bottom left corner
    glm::vec3 enemyMaxPos = enemy + glm::vec3(enemySize, enemySize, 0);

    int check = AABBcollision(playerMinPos, playerMaxPos, enemyMinPos, enemyMaxPos);

    if(check != 0)
    {
        // enemy collided with the player
        return true;
    }
    
    // enemy did not collide with the player
    return false;
}

int Tema1::projectileMetEnemy(glm::vec3* enemies, int enemyCount, glm::vec3 projectile)
{
    glm::vec3 projMinPos = projectile; // this should be the bottom left corner
    glm::vec3 projMaxPos = projectile + glm::vec3(projectileSize, projectileSize, 0);
    
    for (int i = 0; i < enemyCount; i++)
    {
        glm::vec3 enemyMinPos = enemies[i]; // this should be the bottom left corner
        glm::vec3 enemyMaxPos = enemies[i] + glm::vec3(enemySize, enemySize, 0);

        int check = AABBcollision(enemyMinPos, enemyMaxPos, projMinPos, projMaxPos);

        if (check != 0)
        {
            // if projectile hit an enemy, return the index
            return i;
        }
    }

    // if projectile did not hit any enemy return -1
    return -1;
}

int Tema1::enemyCanMove(glm::vec3 enemy) 
{
    glm::vec3 enemyMinPos = enemy; // this should be the bottom left corner
    glm::vec3 enemyMaxPos = enemy + glm::vec3(enemySize, enemySize, 0);

    // if enemy is too far to the right, return 1
    if (enemyMaxPos.x >= 1000)
    {
        return 1;
    }
    // if enemy is too far to the left, return 2
    if (enemyMinPos.x <= -1000)
    {
        return 2;
    }
    // if enemy is too far up, return 3
    if (enemyMaxPos.y >= 750)
    {
        return 3;
    }
    // if player is too far down, return 4
    if (enemyMinPos.y <= -750)
    {
        return 4;
    }

    return 0;
}

int Tema1::projCanMove(glm::vec3 proj) 
{
    glm::vec3 projMinPos = proj; // this should be the bottom left corner
    glm::vec3 projMaxPos = proj + glm::vec3(projectileSize, projectileSize, 0);

    // if enemy is too far to the right, return 1
    if (projMaxPos.x >= 1000)
    {
        return 1;
    }
    // if enemy is too far to the left, return 2
    if (projMinPos.x <= -1000)
    {
        return 2;
    }
    // if enemy is too far up, return 3
    if (projMaxPos.y >= 750)
    {
        return 3;
    }
    // if player is too far down, return 4
    if (projMinPos.y <= -750)
    {
        return 4;
    }

    glm::vec3 offset = glm::vec3(-640, -360, 0);

    //obstacle 1 check
    int check = AABBcollision(projMinPos, projMaxPos, glm::vec3(-100, -50, 0) + offset, glm::vec3(330, 30, 0) + offset);
    if (check != 0) // detected collision with obstacle 1
    {
        return check;
    }

    //obstacle 2 check
    check = AABBcollision(projMinPos, projMaxPos, glm::vec3(40, 400, 0) + offset, glm::vec3(150, 710, 0) + offset);
    if (check != 0) // detected collision with obstacle 2
    {
        return check;
    }

    //obstacle 3 check
    check = AABBcollision(projMinPos, projMaxPos, glm::vec3(1100, 680, 0) + offset, glm::vec3(1270, 800, 0) + offset);
    if (check != 0) // detected collision with obstacle 3
    {
        return check;
    }
    //obstacle 4 check
    check = AABBcollision(projMinPos, projMaxPos, glm::vec3(1100, 40, 0) + offset, glm::vec3(1200, 440, 0) + offset);
    if (check != 0) // detected collision with obstacle 4
    {
        return check;
    }
    //obstacle 5 check
    check = AABBcollision(projMinPos, projMaxPos, glm::vec3(40, 710, 0) + offset, glm::vec3(470, 820, 0) + offset);
    if (check != 0) // detected collision with obstacle 5
    {
        return check;
    }

    // no collission detected
    return 0;
}

// this function was supposed to show the text, but VS can't seem to find the glut library
/* void Tema1::showScore(int score, glm::vec3 pos)
{
    string text = "Score: ";
    glColor3f(0, 0, 0);
    glRasterPos3f(pos.x, pos.y, pos.z);
    stringstream strm;
    strm << text << score;
    string fulltext = strm.str();
    for (string::iterator it = text.begin(); it != text.end(); ++it) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *it);
    }
} */