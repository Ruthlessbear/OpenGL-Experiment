#include <iostream>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include "ShaderModel.h"
#include"SOIL2/stb_image.h"
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"glm/gtx/rotate_vector.hpp"
#include <assimp/Importer.hpp>     
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include "Model.cpp"
#include "CameraNew.h"
#include <FileSystem>
#include <random>
#include "animation.h"
#include "Animator.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "MainUI.h"
#include "Noise.h"

namespace fs = std::filesystem;

const GLint WIDTH = 1000, HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void RenderScreen();
void RenderShadowTexture();
void RenderCube();
void RenderPlane();
void RenderTexture(int index, const GLuint texture, const Shader textureShader);

float lerp(float a, float b, float scale);
std::vector<glm::vec3> GetRandomSample();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
CameraNew camera(glm::vec3(0.0f, 3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int LoadCubeMap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void LoadCharacter()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //render fog
    GLfloat fog_color[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
    glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
    //glEnable(GL_FOG);
    //glFogfv(GL_FOG_COLOR, fog_color);
    //glFogf(GL_FOG_START, 0.001f);
    //glFogf(GL_FOG_END, 3.0f);
    //glFogi(GL_FOG_MODE, GL_LINEAR);
    //glFogf(GL_FOG_DENSITY, 0.9f);
    //glFogi(GL_FOG_COORD_SRC, GL_NICEST);
    //glHint(GL_FOG_HINT, GL_DONT_CARE);
    //glDepthMask(GL_TRUE);
    //颜色混合
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC1_COLOR, GL_ONE);
    //裁剪测试
    //glEnable(GL_CLIP_DISTANCE0);
    //glEnable(GL_CLIP_DISTANCE1);
    
    // build and compile shaders
    // -------------------------
    Shader ourShader("model_loadingvs_pbr.glsl", "model_loadingfs_pbr.glsl");

    //Test Shadow Shader
    Shader shadowShader("shadow_test_vs.glsl", "shadow_test_fs.glsl");

    // load models
    // -----------
    Model ourModel("E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/nanosuit/nanosuit.obj");
    Model ourModel2("E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/nanosuit/nanosuit.obj");

    //animation
    //Animation ourAnimation("E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/animation/walking.dae", &ourModel);
    //Animator ourAnimator(&ourAnimation);

    //load plane
    Shader rockShader("instance_vs.glsl", "instance_fs.glsl");
    Model rock("E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/rock/rock.obj");

    //shadow map
    Shader simpleDepthShader("simple_shadow_vs.glsl", "simple_shadow_fs.glsl");
    Shader shadowTexShader("tex_shadow_vs.glsl", "tex_shadow_fs.glsl");

    //延迟渲染
    Shader delayShader("model_loadingvs_delay.glsl", "model_loadingfs_delay.glsl");
    Shader delayLight("model_lightvs_delay.glsl", "model_lightfs_delay.glsl");
    Shader cubeShader("cube_vs.glsl", "cube_fs.glsl");

    //SSAO
    Shader aoBaseShader("ao_model_vs.glsl", "ao_model_fs.glsl");
    Shader aoPlaneShader("ao_plane_vs.glsl", "ao_plane_fs.glsl");
    Shader aoSampleShader("ao_sample_vs.glsl", "ao_sample_fs.glsl");
    Shader aoComputeShader("ao_compute_vs.glsl", "ao_compute_fs.glsl");
    Shader aoBlurShader("ao_blur_vs.glsl", "ao_blur_fs.glsl");

    //Compute
    Shader computeShader("model_loadingvs_compute.glsl", "model_loadingfs_compute.glsl");

    //Tool
    Shader textureShader("texture_vs.glsl", "texture_fs.glsl");

    //load dragon
   /* Shader dragonShader("model_loadingvs.glsl", "dragon_fs.glsl");
    Model dragonModel("E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/dragon/dragon.sbm");*/

    //CubeMap
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int amount = 1;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];

    glm::mat4 model = glm::mat4(1.0f);
    float x = 30.0f;
    float y = 10.0f;
    float z = 30.0f;
    model = glm::translate(model, glm::vec3(x, y, z));
    float scale = 2.0f;
    model = glm::scale(model, glm::vec3(scale));
    modelMatrices[0] = model;
    
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FLOAT, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FLOAT, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    Shader skyShader("skybox_vs.glsl", "skybox_fs.glsl");

    vector<std::string> faces
    {
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/right.jpg",
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/left.jpg",
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/bottom.jpg",
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/top.jpg",
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/front.jpg",
        "E:/OpenGL/OpenGLExperiment/OpenGLExperiment/OpenGLExperiment/Resources/skybox/back.jpg"
    };
    unsigned int cubemapTexture = LoadCubeMap(faces);
    skyShader.use();
    skyShader.setInt("skybox", 0);

    //shadow buffer
    //GLuint shadow_buffer;
    //GLuint shadow_tex;
    //GLuint shadow_render;
    //glGenFramebuffers(1, &shadow_buffer);
    //glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
    //glGenTextures(1, &shadow_tex);
    //glBindTexture(GL_TEXTURE_2D, shadow_tex);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    ////glGenRenderbuffers(1, &shadow_render);
    ////glBindRenderbuffer(GL_RENDERBUFFER, shadow_render);
    ////glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    ////glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadow_render);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);
    //if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
    //    std::cout << "frame buffer not complete" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //test texture
    //GLuint test_tex;
    //glGenTextures(1, &test_tex);
    //glBindTexture(GL_TEXTURE_2D, test_tex);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //延迟渲染
    /*GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    GLuint gPosition, gNormal, gColorSpec, gDiffuse, gSpecular;
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &gDiffuse);
    glBindTexture(GL_TEXTURE_2D, gDiffuse);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuse, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &gSpecular);
    glBindTexture(GL_TEXTURE_2D, gSpecular);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint attachment[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachment);
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        std::cout << "Framebuffer not complete!" << std::endl;*/
    //GLuint characterDepth;
    //glGenRenderbuffers(1, &characterDepth);
    //glBindRenderbuffer(GL_RENDERBUFFER, characterDepth);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, characterDepth);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //SSAO
    //计算遮蔽因子
    GLuint aoBuffer;
    glGenFramebuffers(1, &aoBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, aoBuffer);
    GLuint ssaoTexture;
    glGenTextures(1, &ssaoTexture);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    //相关数据存储阶段
    GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    GLuint gPositionDepth, gNormal, gColorSpec, gDiffuse, gSpecular;
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //这里拉伸边界，是为了当随机采样到屏幕外的点时，可以相对正常地计算
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint attachment[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachment);
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    GLuint characterDepth;
    glGenRenderbuffers(1, &characterDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, characterDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, characterDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //帧缓冲重指向窗口
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }
    GLuint noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint ssaoBlurBuffer;
    glGenFramebuffers(1, &ssaoBlurBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
    GLuint ssaoBlurTexture;
    glGenTextures(1, &ssaoBlurTexture);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTexture, 0);
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        std::cout << "BlurBuffer Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint noise2d;
    GLubyte* noisePtr = CreateNoise2D();
    glGenTextures(1, &noise2d);
    glBindTexture(GL_TEXTURE_2D, noise2d);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, noisePtr);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        OPUI::KeyBoardIn(window, deltaTime);

        //animation
        //ourAnimator.UpdateAnimation(deltaTime);

        //imgui
        //----------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow(); // Show demo window! :)
        OPUI::DrawMainUI();

        // render
        // ------
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        //glStencilMask(0x00);//禁止写入
        glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
        //render fog
        //glEnable(GL_FOG);
        //glFogfv(GL_FOG_COLOR, fog_color);
        //glFogf(GL_FOG_START, 1.0f);
        //glFogf(GL_FOG_END, 6.0f);
        //glFogi(GL_FOG_MODE, GL_LINEAR);
        //glFogf(GL_FOG_DENSITY, 1.0f);
        //glHint(GL_FOG_HINT, GL_NICEST);
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//模板测试：GL_STENCIL_BUFFER_BIT

        //在帧清除后再做裁剪测试
        // 先做裁剪测试再帧清除可能报错
        //裁剪测试
        //glEnable(GL_SCISSOR_TEST);
        //int scissor_width = (15 * SCR_WIDTH) / 16;
        //int scissor_heigt = (15 * SCR_HEIGHT) / 16;

        //glScissorIndexed(0, 0, 0, scissor_width, scissor_heigt);
        //glScissorIndexed(1, SCR_WIDTH - scissor_width, 0, scissor_width, scissor_heigt);
        //glScissorIndexed(2, 0, SCR_HEIGHT - scissor_heigt, scissor_width, scissor_heigt);
        //glScissorIndexed(3, SCR_WIDTH - scissor_width, SCR_HEIGHT - scissor_heigt, scissor_width, scissor_heigt);

        //模版测试：开启
        //glEnable(GL_STENCIL_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //模板测试：缓存写入规则
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        //glStencilMask(0x00);

        glm::vec3 lightPos = glm::vec3(3.0f, 3.0f, 3.0f);

        //{
        //    //render shadow map

        //    //test depth
        //    simpleDepthShader.use();
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();

        //    simpleDepthShader.setMat4("projection", projection);
        //    simpleDepthShader.setMat4("view", view);

        //    glm::mat4 light_shadow_model = glm::mat4(1.0f);
        //    light_shadow_model = glm::rotate(light_shadow_model, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //    glm::mat4 light_shadow_view = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //    glm::mat4 light_shadow_proj = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1000.0f);//转透视矩阵 left right bottom top near far
        //    glm::mat4 shadow_mvp_matrix = light_shadow_proj * light_shadow_view * light_shadow_model;

        //    glm::mat4 scale_bias_matrix = glm::mat4(
        //        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
        //        glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
        //        glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
        //        glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        //    glm::mat4 shadow_matrix = scale_bias_matrix * shadow_mvp_matrix;
        //    simpleDepthShader.setMat4("lightSpaceMatrix", shadow_mvp_matrix);

        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //    //glClearDepthf(1.0f);
        //    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
        //    //glDrawBuffer(GL_NONE);
        //    //glReadBuffer(GL_NONE);
        //    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        //    //glEnable(GL_POLYGON_OFFSET_FILL);//多边形偏移
        //    //glPolygonOffset(2.0f, 4.0f);

        //    int x_size = 10;
        //    int y_size = 10;
        //    for (int i = 0; i < x_size; i++)
        //    {
        //        for (int j = 0; j < y_size; j++)
        //        {
        //            glm::mat4 model = glm::mat4(1.0f);
        //            float x_offset = i * 2.0f;
        //            float z_offset = j * 2.0f;
        //            model = glm::translate(model, glm::vec3(x_offset, 0.0f, z_offset));
        //            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        //            simpleDepthShader.setMat4("model", model);
        //            ourModel.Draw(simpleDepthShader);
        //        }
        //    }
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //    //glDisable(GL_POLYGON_OFFSET_FILL);
        //    
        //    //glDrawBuffer(GL_FRONT_AND_BACK);
        //    //glReadBuffer(GL_FRONT_AND_BACK);
        //    shadowTexShader.use();
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, shadow_tex);
        //    shadowTexShader.SetTex2D("tex_shadow", 0);
        //    RenderShadowTexture();
        //}
        

        //{
        //    // don't forget to enable shader before setting uniforms
        //    shadowShader.use();

        //    shadowShader.SetTex2D("texture_depth", 3);

        //    //// view/projection transformations
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();

        //    shadowShader.setMat4("projection", projection);
        //    shadowShader.setMat4("view", view);
        //    shadowShader.setMat4("lightSpaceMatrix", shadow_matrix);

        //    // render the loaded model
        //    glm::mat4 model = glm::mat4(1.0f);
        //    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

        //    shadowShader.setMat4("model", model);
        //    //shadowShader.setVec3("cameraPos", camera.Position);
        //    //shadowShader.setVec3("lightPos", lightPos);
        //    //shadowShader.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));

        //    //glBindVertexArray(cubeVAO);
        //    //glActiveTexture(GL_TEXTURE0);
        //    //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        //    //颜色遮罩
        //    //glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

        //    ourModel.Draw(shadowShader);

        //    glm::mat4 model2 = glm::mat4(1.0f);
        //    model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 2.0f)); // translate it down so it's at the center of the scene
        //    model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        //    shadowShader.setMat4("model", model2);
        //    ourModel2.Draw(shadowShader);
        //}

        //{
        //    // don't forget to enable shader before setting uniforms
        //    ourShader.use();

        //    //// view/projection transformations
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();

        //    ourShader.setMat4("projection", projection);
        //    ourShader.setMat4("view", view);
        //    //ourShader.setMat4("lightSpaceMatrix", shadow_matrix);

        //    // render the loaded model
        //    glm::mat4 model = glm::mat4(1.0f);
        //    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

        //    //模版测试：写入缓存
        //    //glStencilFunc(GL_ALWAYS, 1, 0xff);
        //    //glStencilMask(0xff);

        //    ourShader.setMat4("model", model);
        //    ourShader.setVec3("cameraPos", camera.Position);
        //    ourShader.setVec3("lightPos", lightPos);
        //    ourShader.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));

        //    //ourShader.SetTex2D("texture_depth", 10);
        //    //glActiveTexture(GL_TEXTURE10);
        //    //glBindTexture(GL_TEXTURE_2D, shadow_tex);

        //    //glBindVertexArray(cubeVAO);
        //    //glActiveTexture(GL_TEXTURE0);
        //    //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        //    //颜色遮罩
        //    //glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

        //    ourModel.Draw(ourShader);

        //    glm::mat4 model2 = glm::mat4(1.0f);
        //    model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 2.0f)); // translate it down so it's at the center of the scene
        //    model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        //    ourShader.setMat4("model", model2);
        //    ourModel2.Draw(ourShader);
        //}

        //阴影渲染
        /*{
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ourShader.use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setVec3("cameraPos", camera.Position);
            ourShader.setVec3("lightPos", lightPos);
            ourShader.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, shadow_tex);
            ourShader.SetTex2D("texture_depth", 3);

            int x_size = 10;
            int y_size = 10;
            for (int i = 0; i < x_size; i++)
            {
                for (int j = 0; j < y_size; j++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    float x_offset = i * 2.0f;
                    float z_offset = j * 2.0f;
                    model = glm::translate(model, glm::vec3(x_offset, 0.0f, z_offset));
                    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
                    ourShader.setMat4("model", model);
                    ourModel.Draw(ourShader);
                }
            }
        }*/

        //晶格 测试模型的效果有点呆 
        //配合曲面细分效果会不错些
        //{
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //    computeShader.use();
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();
        //    computeShader.setMat4("projection", projection);
        //    computeShader.setMat4("view", view);
        //    computeShader.setVec3("cameraPos", camera.Position);
        //    computeShader.setVec3("lightPos", lightPos);
        //    computeShader.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));

        //    glm::mat4 model = glm::mat4(1.0f);
        //    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        //    computeShader.setMat4("model", model);
        //    ourModel.Draw(computeShader);
        //}

        //前向渲染
        /*{
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ourShader.use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setVec3("cameraPos", camera.Position);
            ourShader.setVec3("lightPos", lightPos);
            ourShader.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));

            int x_size = 7;
            int y_size = 7;
            for (int i = -x_size; i < x_size; i++)
            {
                for (int j = -y_size; j < y_size; j++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    float x_offset = i * 2.0f;
                    float z_offset = j * 2.0f;
                    model = glm::translate(model, glm::vec3(x_offset, 0.0f, z_offset));
                    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
                    ourShader.setMat4("model", model);
                    ourModel.Draw(ourShader);
                }
            }
        }*/


        //延迟渲染
        //{
        //    //first pass
        //    delayShader.use();
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();
        //    delayShader.setMat4("projection", projection);
        //    delayShader.setMat4("view", view);
        //    delayShader.setVec3("lightPos", lightPos);

        //    int x_size = 7;
        //    int y_size = 7;
        //    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //    for (int i = 0; i < x_size; i++)
        //    {
        //        for (int j = 0; j < y_size; j++)
        //        {
        //            glm::mat4 model = glm::mat4(1.0f);
        //            float x_offset = i * 2.0f;
        //            float z_offset = j * 2.0f;
        //            model = glm::translate(model, glm::vec3(x_offset, 0.0f, z_offset));
        //            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        //            delayShader.setMat4("model", model);
        //            ourModel.Draw(delayShader);
        //        }
        //    }
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //    //second pass
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //    delayLight.use();
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, gPosition);
        //    delayLight.SetTex2D("gPosition", 0);
        //    glActiveTexture(GL_TEXTURE1);
        //    glBindTexture(GL_TEXTURE_2D, gNormal);
        //    delayLight.SetTex2D("gNormal", 1);
        //    glActiveTexture(GL_TEXTURE2);
        //    glBindTexture(GL_TEXTURE_2D, gDiffuse);
        //    delayLight.SetTex2D("gDiffuse", 2);
        //    glActiveTexture(GL_TEXTURE3);
        //    glBindTexture(GL_TEXTURE_2D, gSpecular);
        //    delayLight.SetTex2D("gSpecular", 3);
        //    delayLight.setVec3("cameraPos", camera.Position);
        //    delayLight.setVec3("lightPos", lightPos);
        //    delayLight.setVec3("lightPos2", glm::vec3(-10.0, 10.0, -10.0));
        //    delayLight.setMat4("view", view);
        //    RenderScreenDelay();
        //}

        //cube render
        //前向+延迟渲染结合
        //{
        //    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        //    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //    cubeShader.use();
        //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glm::mat4 view = camera.GetViewMatrix();
        //    cubeShader.setMat4("projection", projection);
        //    cubeShader.setMat4("view", view);
        //    for (int i = 0; i < 10; i++)
        //    {
        //        float offset;
        //        offset = i * 1.5f;
        //        glm::mat4 model = glm::mat4(1.0f);
        //        model = glm::translate(model, glm::vec3(offset, 5.0f, offset));
        //        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        //        cubeShader.setMat4("model", model);
        //        glm::vec3 cube_color = glm::vec3(i * 0.1f);
        //        cubeShader.setVec3("cube_color", cube_color);
        //        RenderCube();
        //    }
        //}

        //屏幕空间环境光遮蔽 SSAO
        {
            //基本数据采样
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            aoSampleShader.use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            aoSampleShader.setMat4("projection", projection);
            aoSampleShader.setMat4("view", view);
            aoSampleShader.setVec3("cameraPos", camera.Position);
            aoSampleShader.setVec3("lightPos", lightPos);

            int x_size = 1;
            int y_size = 1;
            for (int i = 0; i < x_size; i++)
            {
                for (int j = 0; j < y_size; j++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    float x_offset = i * 2.0f;
                    float z_offset = j * 2.0f;
                    model = glm::translate(model, glm::vec3(x_offset, 1.0f, z_offset));
                    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
                    aoSampleShader.setMat4("model", model);
                    ourModel.Draw(aoSampleShader);
                }
            }

            aoSampleShader.use();
            //std::vector<glm::mat4> transforms = ourAnimator.GetFinalBoneMatrices();
            //for (int i = 0; i < transforms.size(); ++i)
            //{
            //    ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            //}

            aoSampleShader.setMat4("projection", projection);
            aoSampleShader.setMat4("view", view);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.7f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
            aoSampleShader.setMat4("model", model);
            RenderPlane();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            //生成随机分布采样
            //遮蔽因子输出
            std::vector<glm::vec3> sample_vct = GetRandomSample();
            glBindFramebuffer(GL_FRAMEBUFFER, aoBuffer);
            glClear(GL_COLOR_BUFFER_BIT);
            aoComputeShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPositionDepth);
            aoComputeShader.SetTex2D("gPositionDepth", 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            aoComputeShader.SetTex2D("gNormal", 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            aoComputeShader.SetTex2D("noiseTexture", 2);
            for (int i = 0; i < 64; ++i)
            {
                glUniform3fv(glGetUniformLocation(aoComputeShader.ID, ("samples[" + std::to_string(i) + "]").c_str()), 1, &sample_vct[i][0]);
            }
            aoComputeShader.setMat4("projection", projection);
            RenderScreen();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //模糊处理
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
            glClear(GL_COLOR_BUFFER_BIT);
            aoBlurShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoTexture);
            aoBlurShader.SetTex2D("ssaoTexture", 0);
            RenderScreen();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //计算着色
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            aoBaseShader.use();
            //这里只导入遮蔽因子贴图 类延迟渲染方式
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPositionDepth);
            aoBaseShader.SetTex2D("gPositionDepth", 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            aoBaseShader.SetTex2D("gNormal", 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
            aoBaseShader.SetTex2D("aoTexture", 2);

            aoBaseShader.setVec3("cameraPos", camera.Position);
            aoBaseShader.setVec3("lightPos", lightPos);
            aoBaseShader.setMat4("view", view);

            //parameter from main ui
            float ssao_factor = OPUI::GetUIFloat("test_float");
            aoBaseShader.setFloat("ssao_factor", ssao_factor);
            glm::vec4 ssao_color = OPUI::GetUIVec4("test_color");
            aoBaseShader.setVec4("ssao_color", ssao_color);

            RenderScreen();

            RenderTexture(0, noise2d, textureShader);
            //RenderTexture(0, gNormal, textureShader);
        }

        //颜色遮罩
        //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        //暂时隐藏
        //load rock
        //模版测试：遮罩剔除
        //glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        //glStencilMask(0x00);
        //rockShader.use();
        //rockShader.setMat4("projection", projection);
        //rockShader.setMat4("view", view);
        //glm::mat4 model_rock = glm::mat4(1.0f);
        //model_rock = glm::translate(model_rock, glm::vec3(-5.0f, -5.0f, -5.0f));
        //model_rock = glm::scale(model_rock, glm::vec3(1.0f, 1.0f, 1.0f));
        //rockShader.setMat4("model", model_rock);
        //rock.Draw(rockShader);

        //load dragon
        //dragonShader.use();
        //dragonShader.setMat4("projection", projection);
        //dragonShader.setMat4("view", view);
        //dragonShader.setMat4("model", model);
        //dragonModel.Draw(dragonShader);

        //sky box
        //{
        //    // draw skybox as last
        //    glm::mat4 model_sky = glm::mat4(1.0f);
        //    glm::mat4 view_sky = camera.GetViewMatrix();
        //    glm::mat4 projection_sky = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        //    skyShader.use();
        //    view_sky = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        //    skyShader.setMat4("view", view_sky);
        //    skyShader.setMat4("projection", projection_sky);
        //    // skybox cube
        //    glBindVertexArray(skyboxVAO);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //    glBindVertexArray(0);
        //}
        
        //模版测试：恢复写入
        //glStencilMask(0xff);
        //glStencilFunc(GL_ALWAYS, 1, 0xff);

        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        // Rendering
        // (Your code clears your framebuffer, renders your other stuff etc.)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // (Your code calls glfwSwapBuffers() etc.)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

GLuint delayVAO = 0;
GLuint delayVBO;
void RenderScreen()
{
    if (delayVAO == 0)
    {
        GLfloat delayVertex[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &delayVAO);
        glGenBuffers(1, &delayVBO);
        glBindVertexArray(delayVAO);
        glBindBuffer(GL_ARRAY_BUFFER, delayVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(delayVertex), &delayVertex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    }

    glBindVertexArray(delayVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

GLuint shadow_texVAO = 0;
GLuint shadow_texVBO;
void RenderShadowTexture()
{
    if (shadow_texVAO == 0)
    {
        GLfloat shadowVertexs[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &shadow_texVAO);
        glGenBuffers(1, &shadow_texVBO);
        glBindVertexArray(shadow_texVAO);
        glBindBuffer(GL_ARRAY_BUFFER, shadow_texVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(shadowVertexs), &shadowVertexs, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    }
    glBindVertexArray(shadow_texVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

GLuint cubeVAO, cubeVBO;
void RenderCube()
{
    //CubeMap
    GLfloat cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    if (cubeVAO == 0)
    {
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);
    glBindVertexArray(0);
}

GLuint planeVAO;
GLuint planeVBO;
void RenderPlane() 
{
    if (planeVAO == 0)
    {
        GLfloat planeVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &planeVAO);
        glBindVertexArray(planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
    }
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

GLuint textureVAO[4];
GLuint textureVBO[4];
void RenderTexture(int index, const GLuint texture, const Shader textureShader)
{
    glDisable(GL_DEPTH_TEST);

    if (textureVAO[0] == 0)
    {
        //for (int i = 0; i < 2; i++)
        //{
        //    for (int j = 0; j < 2; j++)
        //    {
        //        vertices_group[i][] = {
        //            -1.0f + i * 0.25f, -0.75f + j * 0.25f, 0.0f, 0.0f, 1.0f,
        //            -1.0f + i * 0.25f, -1.0f + j * 0.25f, 0.0f, 0.0f, 0.0f,
        //            -0.75 + i * 0.25f, -0.75 + j * 0.25f, 0.0f, 1.0f, 1.0f,
        //            -0.75f + i * 0.25f, -1.0f + j * 0.25f, 0.0f, 1.0f, 0.0f
        //        };
        //        vertices_group.push_back(planeVertices);
        //    }
        //}

        GLfloat vertices_group1[] = {
            -1.0f, -0.5f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5, -0.5, 0.0f, 1.0f, 1.0f,
            -0.5f, -1.0f, 0.0f, 1.0f, 0.0f
        };

        for (int i = 0; i < 1; i++)
        {
            glGenVertexArrays(1, &textureVAO[i]);
            glBindVertexArray(textureVAO[i]);
            glGenBuffers(1, &textureVBO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, textureVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_group1), vertices_group1, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));
        }
    }
    textureShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.SetTex2D("show_texture", 0);
    glBindVertexArray(textureVAO[index]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

float lerp(float a, float b, float scale)
{
    return a + scale * (b - a);
}

std::vector<glm::vec3> GetRandomSample()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    int sampler_max = 64;//样本基数
    for (int i = 0; i < sampler_max; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / sampler_max;
        scale = lerp(0.1f, 1.0f, scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    return ssaoKernel;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FOR, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}