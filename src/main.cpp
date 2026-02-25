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

     float planeVertices[] = {
      // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

     float quadVertices[] = {
          // positions     // texCoords
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,

          -1.0f,  1.0f,  0.0f, 1.0f,
          1.0f, -1.0f,  1.0f, 0.0f,
          1.0f,  1.0f,  1.0f, 1.0f
     };
     GLFWwindow *window;
     glfwWindowHint(GLFW_SAMPLES, 4);

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
     Shader defaultShader("../assets/shaders/default.vert", "../assets/shaders/default.frag");
     Shader modelShader("../assets/shaders/model.vert", "../assets/shaders/model.frag");
     Shader depthShader("../assets/shaders/depth.vert", "../assets/shaders/depth.frag");
     depthShader.LinkGeometry("../assets/shaders/depth.geom");
     Shader framebufferShader("../assets/shaders/framebuffer.vert", "../assets/shaders/framebuffer.frag");
     Shader shadowShader("../assets/shaders/shadow.vert", "../assets/shaders/shadow.frag");
     // models
     Model cube("../assets/cube.obj");
     Model plane("../assets/plane.obj");
     Model torus("../assets/torus.obj");
     Model sphere("../assets/sphere/source/sphere.obj");
     // textures
     TextureUnit woodTexture("../assets/textures/woodTex.jpeg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

     // plane geometry
     VAO planeVAO;
     VBO planeVBO(planeVertices, sizeof(planeVertices));
     planeVAO.Bind();
     planeVAO.LinkAttrib(planeVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)(0));
     planeVAO.LinkAttrib(planeVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
     planeVAO.LinkAttrib(planeVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
     planeVAO.Unbind();
     // quad geometry
     VAO quadVAO;
     VBO quadVBO(quadVertices, sizeof(quadVertices));
     quadVAO.Bind();
     quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
     quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
     quadVAO.Unbind();

     //-----------IMAGE VARIABLES-----------
     unsigned int depthMapFBO;
     glGenFramebuffers(1, &depthMapFBO);
     // 2D texture for framebuffer depth buffer
     const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

     unsigned int depthCubemap;
     glGenTextures(1, &depthCubemap);
     glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
     for(unsigned int i = 0; i < 6; ++i)
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
     glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
     glDrawBuffer(GL_NONE);
     glReadBuffer(GL_NONE);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*unsigned int depthMap;
     glGenTextures(1, &depthMap);
     glBindTexture(GL_TEXTURE_2D, depthMap);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
     float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

     // attatch to framebuffer's depth buffer
     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // only need depth info when rendering the scene from light's perspective
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
     glDrawBuffer(GL_NONE); // framebuffer obj is not complete without a color buffer
     glReadBuffer(GL_NONE); // so we explicitely set these states so OpenGL knows we're not going to render color data
     
     if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          return -1;
     glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
      //-----------END OF IMAGE VARIABLES-----------

     glEnable(GL_DEPTH_TEST); // Allows for depth comparison and updates the depth buffer
     glEnable(GL_MULTISAMPLE);
     glEnable(GL_CULL_FACE);
    

     // -----------RENDER LOOP VARIABLES-----------
     Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
     glm::vec3 lightPos(0.1f, 8.0f, 5.0f);
     glm::vec3 dirLightPos(0);
     float radius = 3.0f;

     float deltaTime = 0.0f;
     float lastFrame = 0.0f;
     //-----------RENDER LOOP VARIABLES-----------
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO &io = ImGui::GetIO();
     (void)io;
     ImGui::StyleColorsDark();
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL3_Init("#version 330");
     
     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

     // Main Render Loop
     while (!glfwWindowShouldClose(window))
     {
          glClearColor(0.0f, 0.0f, 0.15f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          ImGui_ImplOpenGL3_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();

          float crntFrame = glfwGetTime();
          deltaTime = crntFrame - lastFrame;
          lastFrame = crntFrame;

           if (!io.WantCaptureMouse)
               camera.Inputs(window);
          
          camera.Matrix(45, 0.1, 100);
         
          //--------------SHADERS & MODEL DRAWING--------------

          // MVP from light's point of view
          float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;
          float near = 1.0f;
          float far = 25.0f;
          glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
          std::vector<glm::mat4> shadowTransforms;
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
          shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
          glm::mat4 model = glm::mat4(1.0f);
          
          depthShader.Activate();
          // ----------FIRST PASS (DEPTH)----------
          glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
          glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
          glClear(GL_DEPTH_BUFFER_BIT);
          glCullFace(GL_BACK);
          for(unsigned int i = 0; i < 6; ++i)
               depthShader.SetToMat4(&("shadowMatrices[" + std::to_string(i) + "]")[0], shadowTransforms[i]);
          depthShader.SetToFloat("far_plane", far);
          depthShader.SetToVec3("lightPos", &lightPos[0]);
          // Drawing Models
          // Plane
          model = glm::scale(model, glm::vec3(5.0));
          depthShader.SetToMat4("model", model);
          plane.Draw(depthShader);
          // Cube
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(0.0f, 2.0f, 2.0f));
          model = glm::scale(model, glm::vec3(0.8));
          depthShader.SetToMat4("model", model);
          cube.Draw(depthShader);
          // Torus
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(-0.5f, 0.5, -2.0f));
          depthShader.SetToMat4("model", model);
          torus.Draw(depthShader);
          // Sphere
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(0.0f, 2.0f, -1.0f));
          depthShader.SetToMat4("model", model);
          sphere.Draw(depthShader);
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
          // Viewport Reset
          glViewport(0, 0, width, height);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          // ----------SECOND PASS (SHADOWS)----------
          // Vertex Uniforms
          model = glm::mat4(1.0f);
          glm::mat4 view = camera.GetViewMatrix();
          glm::mat4 proj = camera.GetProjMatrix();
          shadowShader.Activate();
          shadowShader.SetToMat4("projection", proj);
          shadowShader.SetToMat4("view", view);
          // Fragment Uniforms
          glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
          glActiveTexture(GL_TEXTURE0);
          shadowShader.SetToInt("depthMap", 0);
          shadowShader.SetToVec3("lightPos",&lightPos[0]);
          shadowShader.SetToVec3("viewPos", &camera.Position[0]);
          shadowShader.SetToVec3("objColor", &glm::vec3(0.9f)[0]);
          shadowShader.SetToFloat("far_plane", far);
          // Drawing Objects Again
          // Plane
          model = glm::scale(model, glm::vec3(5.0));
          shadowShader.SetToMat4("model", model);
          plane.Draw(shadowShader);
          // Cube
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(0.0f, 2.0f, 2.0f));
          model = glm::scale(model, glm::vec3(0.8));
          shadowShader.SetToMat4("model", model);
          cube.Draw(depthShader);
          // Torus
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(-0.5f, 0.5, -2.0f));
          shadowShader.SetToMat4("model", model);
          torus.Draw(depthShader);
          // Sphere
          model = glm::mat4(1.0f);
          model = glm::translate(model, glm::vec3(0.0f, 2.0f, -1.0f));
          shadowShader.SetToMat4("model", model);
          sphere.Draw(depthShader);
          // Light Position Model
          model = glm::mat4(1.0f);
          model = glm::translate(model, lightPos);
          model = glm::scale(model, glm::vec3(0.2f));
          shadowShader.SetToMat4("model", model);
          sphere.Draw(shadowShader);
          glCullFace(GL_FRONT);
          //--------------END OF SHADERS & MODEL DRAWING--------------

          // ---------DEPTH DEBUGGING---------
          framebufferShader.Activate();
          /*quadVAO.Bind();
          glDrawArrays(GL_TRIANGLES, 0, 6);
          quadVAO.Unbind();*/
          // ---------END OF DEPTH DEBUGGING---------

          // ---------IMGUI---------
          ImGui::Begin("OpenGL Settings Panel");
          ImGui::Text("Tweaks");

          ImGui::Separator();

          ImGui::Text("Edit Directional Light");
          ImGui::SliderFloat3("DirLight Pos", &dirLightPos[0], 0.0f, 10.0f);
          
          ImGui::Separator();

          ImGui::Text("Edit Point Light");
          ImGui::SliderFloat3("Light Pos", &lightPos[0], -10.0f, 15.0f);
          ImGui::End();
          
          ImGui::Render();
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
          // ---------END OF IMGUI---------

          // Swap back buffer with front buffer
          glfwSwapBuffers(window);
          // Makes sure our window is responsive (such as resizing it and moving it)
          glfwPollEvents();
     }

     ImGui_ImplOpenGL3_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();

     // ------------OBJECT DELETION------------
     defaultShader.Delete();

     glfwTerminate();
     return 0;
}
