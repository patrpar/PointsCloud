#include "RenderEngine.h"
#include <memory>

#include "Shader.h"
#include "VertexBuffer.h"

RenderEngine::RenderEngine(std::vector<float>& points)
{
    createWindow();

    cloudPoints = points;


    std::string vertexShaderPath = R"(.\render_engine\shaders\vertexShader.shader)";
    std::string fragmentShaderPath = R"(.\render_engine\shaders\fragmentShader.shader)";

    shader.reset(new Shader(vertexShaderPath, fragmentShaderPath));
    vertexArray.reset(new VertexArray());

    vertexBuffer.reset(new VertexBuffer(cloudPoints));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    glPointSize(5.0f);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


}


void RenderEngine::view()
{

    glEnable(GL_DEPTH_TEST);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // TODO RENDERER ABSTRACT CLASS HERE

        processInput(window);

        /* Render here */
        

        glClearColor(1, 1, 1, 1);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //  glClear( GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setUniformMat4f("u_mvp", mvp);
        vertexBuffer->bind();
        glDrawArrays(GL_POINTS, 0, vertexBuffer->getSize());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}


void RenderEngine::createWindow()
{
    if (!glfwInit()) {
        throw std::runtime_error("Couldn't initalize glfw!");
    }


    window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Couldn't create window!");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initalize GLAD!");
    }

    glViewport(0, 0, width, height);
}

void RenderEngine::processInput(GLFWwindow* window)
{
    auto cameraSpeed = 0.005f;

    auto change = 0.1f * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPosition += glm::normalize(glm::cross(cameraRight, cameraFront)) * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPosition -= glm::normalize(glm::cross(cameraRight, cameraFront)) * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        cameraPosition -= cameraFront * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        cameraPosition += cameraFront * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {

        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();

        float xoffset = 0.5f;
        float yoffset = 0;

        Yaw += xoffset;
        Pitch += yoffset;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        cameraFront = glm::normalize(front);

        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {

        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();

        float xoffset = -1.5f;
        float yoffset = 0;

        Yaw += xoffset;
        Pitch += yoffset;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        cameraFront = glm::normalize(front);

        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp)); 
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        recalculateMVP();
    }



}


void RenderEngine::recalculateMVP()
{
    viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    mvp = projectionMatrix * viewMatrix * modelMatrix;
}
