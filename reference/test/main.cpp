#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <numbers>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera/fpc.hpp"
#include "shader/shader.hpp"

#include "skeletal/skeleton.hpp"
#include "pipeline/skeleton.hpp"

#include "skeletal/mesh.hpp"
#include "pipeline/mesh.hpp"

#include "skeletal/animator.hpp"
// #include "pipeline/animator.hpp"

#include "gltf/tinygltf_helper.h"

void processCameraInput(GLFWwindow* window, FirstPersonCamera* camera);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Use Debug Context
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(500, 500, "Skeletal Animation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Load Opengl */
    gladLoadGL();

    /**** Initiate Objects Here ****/
    Shader shader_skel("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\shader\\skeleton.vs", "E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\shader\\skeleton.fs");
    /***************************my code*************************/
    Shader shader_mesh("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\shader\\skeleton.vs", "E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\shader\\mesh.fs");
    /***************************my code end*************************/
    tinygltf::Model model;
    if (!tinygltf_parsefile("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\mdl\\dancing_cylinder.gltf", model)) {
    // if (!tinygltf_parsefile("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\mdl\\weaving_flag.gltf", model)) {
        std::cout<<"failed to load module."<<std::endl;
        return -1;
    }

    FirstPersonCamera camera;

    ///////////////
    // Some setups here
    Skeleton skel;
    std::string warn, err;
    if (!skel.loadFromTinyGLTF(model, warn, err)) {
        std::cout << "SkeletonLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "SkeletonLoaderWarning: " << warn << std::endl;
        warn.clear();
    }
    
    // WireframeSkeletonPipeline pipeline_skel(&shader_skel, &camera, &skel);

    /***************************my code*************************/
    BoneWeightedMesh mesh;
    std::string warn_mesh, err_mesh;
    if (!mesh.loadFromTinyGLTF(model, warn_mesh, err_mesh)) {
        std::cout << "MeshLoaderError: " << err_mesh << std::endl;
    };
    if (!warn_mesh.empty()) {
        std::cout << "MeshLoaderWarning: " << warn_mesh << std::endl;
        warn_mesh.clear();
    }

    WireframeMeshPipeline pipeline_mesh(&shader_mesh, &camera, &mesh);

    SkeletalAnimator anim;
    std::string warn_anim, err_anim;
    if (!anim.loadFromTinyGLTF(model, warn, err, &skel)) {
        std::cout << "AnimationLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "AnimationLoaderWarning: " << warn << std::endl;
        warn.clear();
    }
    //  WireframeMeshPipeline pipeline_mesh(&shader_mesh, &camera, &mesh);
    WireframeSkeletonPipeline pipeline_skel(&shader_skel, &camera, &skel, &anim);
    /***************************my code end*************************/
    ///////////////

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        processCameraInput(window, &camera);
        float curFrameTime = glfwGetTime();
        float curAnimTime = int(curFrameTime) % 5 + curFrameTime - int(curFrameTime); // so that we have a looped time from 0~5
        // std::cout<<curAnimTime<<std::endl;
        ///////////////////
        // Some extra codes here 
        // todo just like some animate function?


        //////////////////


        // update skeleton & mesh
        // draw mesh 
        // draw skeleton 
        pipeline_skel.draw(curAnimTime);
        /***************************my code*************************/
        pipeline_mesh.draw();
        /***************************my code end*************************/
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/**
 * Controlling the provided first person camera
 * Use WASD to move around
 * Use arrow keys to watch around
 * Use Q and E to rotate camera
 * Use space and lctrl to move up and down 
 * */
void processCameraInput(GLFWwindow* window, FirstPersonCamera* camera)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
	static float lastFrame{0};
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

	float cameraSpeed = 3.5f * deltaTime; // adjust accordingly
	if((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS))
        camera->moveForward(cameraSpeed);
    if((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS))
        camera->moveBackward(cameraSpeed);
    if((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS))
        camera->moveRight(cameraSpeed);
    if((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS))
        camera->moveLeft(cameraSpeed);
    if((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS))
        camera->moveUp(cameraSpeed);
    if((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS))
        camera->moveDown(cameraSpeed);

    if((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS))
        camera->lookRight(1.0f * deltaTime);
    if((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS))
        camera->lookLeft(1.0f * deltaTime);
    if((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS))
        camera->lookUp(1.0f * deltaTime);
    if((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS))
        camera->lookDown(1.0f * deltaTime);

    if((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS))
        camera->rotateLeft(1.0f * deltaTime);
    if((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS))
        camera->rotateRight(1.0f * deltaTime);
}