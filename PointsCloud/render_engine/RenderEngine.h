#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



#include "../core/Points.h"
#include "../core/core.h"
#include "RenderEngineHelper.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>



class RenderEngine
{
public:
    RenderEngine(std::vector<float>& points);
    void view();

    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
private:
    void createWindow();
    void processInput(GLFWwindow* window);


    void recalculateMVP();



private:
    std::vector<float> cloudPoints;
    GLFWwindow* window;
    int width = 900;
    int height = 900;
    std::string_view title = "Points Cloud - Kamil Kopryk, Ignacy Grabowski, Patryk Pardej";


    std::unique_ptr<Shader> shader;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<VertexArray> vertexArray;

    // glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    // glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f,-3.0f));
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 worldRight = glm::vec3(1.0f, 0.0f, 0.0f);


    float Yaw = -90.0f;
    float Pitch = 0.0f;

    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;


};

