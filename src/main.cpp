#include <iostream>
#include "../headers/imgui/imgui.h"
#include "../headers/imgui/imgui_impl_glfw.h"
#include "../headers/imgui/imgui_impl_opengl3.h"
#include "glad.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <filesystem>

#include "../headers/shaderClass.h"
#include "../headers/EBO.h"
#include "../headers/VBO.h"
#include "../headers/VAO.h"
#include "../headers/texture.h"
#include "../headers/camera.h"
#include "../headers/model.h"

#include "../include/stb/stb_image.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

const unsigned int width = 800;
const unsigned int height = 800;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
     glViewport(0, 0, width, height);
}

int main(int, char **)
{
     // Storing our values of each vertex in our coordinate space
     GLfloat vertices[] =
         {
             // positions          // normals           // texture coords
             -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
             0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
             0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
             -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

             -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

             -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

             0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

             -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

             -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

     float planeVertices[] = {
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  
        -5.0f, -0.5f,  5.0f,  
        -5.0f, -0.5f, -5.0f,  

         5.0f, -0.5f,  5.0f,  
        -5.0f, -0.5f, -5.0f,  
         5.0f, -0.5f, -5.0f, 
    };

     // Index Buffer
     /* GLuint indices[] =
      {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
      };*/

     GLFWwindow *window;

     if (!glfwInit())
     {
          return -1;
     }
     // Create the window that appears on the screen
     window = glfwCreateWindow(width, height, "Window", NULL, NULL);

     // Tells GLFW to add the window to the current context
     // A context being an object that encapsulates the OpenGL state machine
     glfwMakeContextCurrent(window);

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
     {
          std::cout << "Couldn't load OpenGL" << std::endl;
          glfwTerminate();
          return -1;
     }
     // Parses the fragment and vertex shader files and wraps them into a shader program
     // The files are compiled to an intermediary language then translated into specific instructions for the GPU
     Shader shaderProgram("../assets/shaders/default.vert", "../assets/shaders/default.frag");
     Shader lightSourceProgram("../assets/shaders/lightSource.vert", "../assets/shaders/lightSource.frag"); // Shader program for light sources
     Shader modelShader("../assets/shaders/model.vert", "../assets/shaders/model.frag");
     
     Model model("../assets/bag/bag.obj");
     Model lightSphere("../assets/sphere/source/sphere.obj");

     // Vertex Array Buffer
     VAO VAO1;
     VAO1.Bind(); // Tells OpenGL to activate this VAO
     // Vertex Buffer Object
     // Contains vertex attributes such as position and color
     VBO VBO1(vertices, sizeof(vertices));
     // EBO EBO1(indices, sizeof(indices)); // Responsible for storing indices on the GPU and drawing them in a specific order
     // Creating a second geometrical object
     VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0); // Basically create a pointer to the VBO (position)
     VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
     VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));
     VAO1.Unbind();

     // Creating our second VAO for another object
     VAO VAO2;
     VAO2.Bind();
     // EBO EBO2(indices, sizeof(indices));
     VAO2.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0); // We reuse the first VBO since it already has the attributes we want
     VAO2.Unbind();

     // plane geometry
     VAO planeVAO;
     VBO planeVBO(planeVertices, sizeof(planeVertices));
     planeVAO.Bind();
     planeVAO.LinkAttrib(planeVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0));
     planeVAO.Unbind();

     // depth map
     unsigned int depthMapFBO;
     glGenFramebuffers(1, &depthMapFBO);
     // 2D texture for framebuffer depth buffer
     const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

     unsigned int depthMap;
     glGenTextures(1, &depthMap);
     glBindTexture(GL_TEXTURE_2D, depthMap);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
     // attatch to framebuffer's depth buffer
     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // only need depth info when rendering the scene from light's perspective
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
     glDrawBuffer(GL_NONE); // framebuffer obj is not complete without a color buffer
     glReadBuffer(GL_NONE); // so we explicitely set these states so OpenGL knows we're not going to render color data
     glBindFramebuffer(GL_FRAMEBUFFER, 0);

     glEnable(GL_DEPTH_TEST); // Allows for depth comparison and updates the depth buffer

     Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

     glm::vec3 lightPos(0.5f, 0.5f, 0.0f);
     float radius = 3.0f;

     float deltaTime = 0.0f;
     float lastFrame = 0.0f;

     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO &io = ImGui::GetIO();
     (void)io;
     ImGui::StyleColorsDark();
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL3_Init("#version 330");

     // ImGui Variables
     bool drawTriangle = true;
     glm::vec3 colorValue = {1.0f, 1.0f, 1.0f};

     glm::vec3 dirLightAmbientIntensity = {0.05f, 0.05f, 0.05f};
     glm::vec3 dirLightDiffuseIntensity = {0.4f, 0.4f, 0.4f};
     glm::vec3 dirLightSpecularIntensity = {0.1f, 0.1f, 0.1f};
     glm::vec3 dirLightVecDirection = {-0.2f, -1.0f, -0.3f};

     glm::vec3 pLightAmbient(0.6f);
     glm::vec3 pLightDiffuse(0.8f);
     glm::vec3 pLightSpecular(1.0f);
     float shinyValue = 32.0f;

     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

     // Main Render Loop
     while (!glfwWindowShouldClose(window))
     {
          // Specify color of background
          glClearColor(0.0f, 0.0f, 0.15f, 1.0f);
          // Clean the back buffer and assign the new color to it and update the depth buffer
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          ImGui_ImplOpenGL3_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();

          float crntFrame = glfwGetTime();
          deltaTime = crntFrame - lastFrame;
          lastFrame = crntFrame;

           if (!io.WantCaptureMouse)
               camera.Inputs(window);

          glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
          glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
          glClear(GL_DEPTH_BUFFER_BIT);
          // configure shader and matrices
          
          // render scene 

          glBindFramebuffer(GL_FRAMEBUFFER, 0);



          // Render scene a second time
          glViewport(0, 0, width, height);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          glBindTexture(GL_TEXTURE_2D, depthMap);
          modelShader.Activate();

          camera.Matrix(45.0f, 0.1f, 100.0f, modelShader, "camMatrix");

          glm::mat4 modelM = glm::mat4(1.0f);
          modelM = glm::translate(modelM, glm::vec3(0.0f, 0.0f, 0.0f));
          modelM = glm::scale(modelM, glm::vec3(1.0f, 1.0f, 1.0f));
          modelM = glm::rotate(modelM, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
          modelShader.SetToMat4("model", modelM);
          model.Draw(modelShader);

          lightPos.x = cos(crntFrame) * radius;
          lightPos.y = sin(crntFrame) * radius;
          // Directional Light Uniforms
          modelShader.SetToFloat("u_mat.shininess", shinyValue);
          modelShader.SetToVec3("dirLight.direction", &dirLightVecDirection[0]);
          modelShader.SetToVec3("dirLight.ambient", &dirLightAmbientIntensity[0]);
          modelShader.SetToVec3("dirLight.diffuse", &dirLightDiffuseIntensity[0]);
          modelShader.SetToVec3("dirLight.specular", &dirLightSpecularIntensity[0]);
          modelShader.SetToVec3("u_viewPos", &camera.Position[0]);
          // Point Light Uniforms
          modelShader.SetToVec3("pointLight.position", &lightPos[0]);
          modelShader.SetToVec3("pointLight.ambient", &pLightAmbient[0]);
          modelShader.SetToVec3("pointLight.diffuse", &pLightDiffuse[0]);
          modelShader.SetToVec3("pointLight.specular", &pLightSpecular[0]);
          modelShader.SetToFloat("pointLight.constant", 1.0f);
          modelShader.SetToFloat("pointLight.linear", 0.09f);
          modelShader.SetToFloat("pointLight.quadratic", 0.032f);
          // directional light uniforms
          shaderProgram.Activate();
          shaderProgram.SetToVec3("u_mat.objectColor", &glm::vec3(0.5f)[0]);
          shaderProgram.SetToFloat("u_mat.shininess", shinyValue);
          shaderProgram.SetToVec3("u_dirLight.direction", &dirLightVecDirection[0]);
          shaderProgram.SetToVec3("u_dirLight.ambient", &dirLightAmbientIntensity[0]);
          shaderProgram.SetToVec3("u_dirLight.diffuse", &dirLightDiffuseIntensity[0]);
          shaderProgram.SetToVec3("u_dirLight.specular", &dirLightSpecularIntensity[0]);
          shaderProgram.SetToVec3("u_viewPos", &camera.Position[0]);
          // point light uniforms
          shaderProgram.SetToVec3("u_pointLights[0].position", &lightPos[0]);
          shaderProgram.SetToVec3("u_pointLights[0].ambient", &pLightAmbient[0]);
          shaderProgram.SetToVec3("u_pointLights[0].diffuse", &pLightDiffuse[0]);
          shaderProgram.SetToVec3("u_pointLights[0].specular", &pLightSpecular[0]);
          shaderProgram.SetToFloat("u_pointLights[0].constant", 1.0f);
          shaderProgram.SetToFloat("u_pointLights[0].linear", 0.09f);
          shaderProgram.SetToFloat("u_pointLights[0].quadratic", 0.032f);
          // other uniforms

          lightSourceProgram.Activate();
          camera.Matrix(45.0f, 0.1f, 100.0f, lightSourceProgram, "camMatrix");

          glm::mat4 lightModel = glm::mat4(1.0f);
          lightModel = glm::translate(lightModel, lightPos);
          lightModel = glm::scale(lightModel, glm::vec3(0.15f, 0.15f, 0.15f));
          
          lightSourceProgram.SetToMat4("model", lightModel);
          lightSphere.Draw(lightSourceProgram);

          shaderProgram.Activate();
          camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
          planeVAO.Bind();
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
          shaderProgram.SetToMat4("model", model);
          glDrawArrays(GL_TRIANGLES, 0, 6);
          planeVAO.Unbind();


          // GUI STUFF
          ImGui::Begin("OpenGL Settings Panel");
          ImGui::Text("Tweaks");
          ImGui::Checkbox("Draw Triangle", &drawTriangle);

          ImGui::Separator();

          ImGui::Text("Edit Directional Light");
          ImGui::SliderFloat3("Light Direction", &dirLightVecDirection[0], 0.0f, 1.0f, "%.2f");
          ImGui::SliderFloat("Shininess", &shinyValue, 0.0f, 64.0f, 0);

          ImGui::Separator();

          ImGui::Text("Edit Point Light");
          ImGui::SliderFloat3("Point Ambience", &pLightAmbient[0], 0.0f, 1.0f, "%.2f");
          ImGui::SliderFloat3("Point Diffusion", &pLightDiffuse[0], 0.0f, 1.0f, "%.2f");
          ImGui::SliderFloat3("Point Specular", &pLightSpecular[0], 0.0f, 1.0f, "%.2f");
          ImGui::End();
          
          ImGui::Render();
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

          // Swap back buffer with front buffer
          glfwSwapBuffers(window);
          // Makes sure our window is responsive (such as resizing it and moving it)
          glfwPollEvents();
     }

     ImGui_ImplOpenGL3_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();

     // Deleting objects (memory management!)
     VAO1.Delete();
     VAO2.Delete();
     VBO1.Delete();
     // EBO1.Delete();
     // EBO2.Delete();
     shaderProgram.Delete();
     lightSourceProgram.Delete();
     // diffuseMap.Delete();
     // specularMap.Delete();

     glfwTerminate();
     return 0;
}
