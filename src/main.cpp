#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "shaders.h"
#include "camera.h"
#include "Model.h"
#include "Skybox.h"
#include "BufferManager.h"
#include "Layer.h"
#include "ParticleSystem.h"
#include <mach-o/dyld.h>
#include <unistd.h>

std::string getExecutableDir() {
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
        std::cout << "Error: Unable to get executable path." << std::endl;
        return "";
    }
    char resolvedPath[1024];
    if (realpath(path, resolvedPath) == nullptr) {
        std::cout << "Error: Unable to resolve executable path." << std::endl;
        return "";
    }
    std::string pathStr(resolvedPath);
    size_t lastSlash = pathStr.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return pathStr.substr(0, lastSlash) + "/";
    }
    return "";
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessInput(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessInput(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessInput(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessInput(window, deltaTime);
}

// Structure pour associer les parties à identifier
struct TargetPart {
    std::string system;
    std::string subModel;
    std::string part;
};

int main() {
    std::cout << "Program started." << std::endl << std::flush;
    std::cerr.rdbuf(std::cout.rdbuf());

    std::string exeDir = getExecutableDir();
    if (exeDir.empty()) {
        std::cout << "Failed to determine executable directory." << std::endl;
        return -1;
    }
    std::cout << "Executable directory: " << exeDir << std::endl;

    std::string shaderBasePath = exeDir + "shaders/";
    std::string modelVertexPath = shaderBasePath + "model_vertex.glsl";
    std::string modelFragmentPath = shaderBasePath + "model_fragment.glsl";
    std::string skyboxVertexPath = shaderBasePath + "skybox_vertex.glsl";
    std::string skyboxFragmentPath = shaderBasePath + "skybox_fragment.glsl";
    std::string depthVertexPath = shaderBasePath + "depth_vertex.glsl";
    std::string depthFragmentPath = shaderBasePath + "depth_fragment.glsl";
    std::string particleVertexPath = shaderBasePath + "particle_vertex.glsl";
    std::string particleFragmentPath = shaderBasePath + "particle_fragment.glsl";
    std::string postprocessVertexPath = shaderBasePath + "postprocess_vertex.glsl";
    std::string postprocessFragmentPath = shaderBasePath + "postprocess_fragment.glsl";

    if (!glfwInit()) {
        std::cout << "Erreur : échec de l'initialisation de GLFW." << std::endl;
        const char* description;
        int code = glfwGetError(&description);
        if (description) std::cout << "GLFW Error Code: " << code << ", Description: " << description << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int windowWidth = 800;
    int windowHeight = 600;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Visualisation Anatomique", NULL, NULL);
    if (window == NULL) {
        std::cout << "Erreur : échec de création de la fenêtre GLFW." << std::endl;
        const char* description;
        int code = glfwGetError(&description);
        if (description) std::cout << "GLFW Error Code: " << code << ", Description: " << description << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Erreur : échec d'initialisation de GLAD." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BufferManager bufferManager;
    Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
    float lastFrame = 0.0f;

    Shader shader(modelVertexPath.c_str(), modelFragmentPath.c_str());
    Shader skyboxShader(skyboxVertexPath.c_str(), skyboxFragmentPath.c_str());
    Shader depthShader(depthVertexPath.c_str(), depthFragmentPath.c_str());
    Shader particleShader(particleVertexPath.c_str(), particleFragmentPath.c_str());
    Shader postProcessShader(postprocessVertexPath.c_str(), postprocessFragmentPath.c_str());

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int postProcessFBO;
    glGenFramebuffers(1, &postProcessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
    unsigned int postProcessTexture;
    glGenTextures(1, &postProcessTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessTexture, 0);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Post-process framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    std::vector<std::string> faces = {
        "assets/environment/textures/px.png",
        "assets/environment/textures/nx.png",
        "assets/environment/textures/py.png",
        "assets/environment/textures/ny.png",
        "assets/environment/textures/pz.png",
        "assets/environment/textures/nz.png"
    };
    Skybox skybox(faces);

    std::vector<Layer> layers;
    layers.emplace_back("Squelette", "Base structurale du corps");
    layers.back().AddModel("assets/skeleton/models/SkeletonBody.obj", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/skeleton/textures/01_BONE_0.jpeg");
    layers.back().AddSubModel("Squelette complet 1", "assets/skeleton/models/SkeletonBody.obj", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/skeleton/textures/01_BONE_0.jpeg");
    layers.back().AddSubModel("Squelette complet 2", "assets/skeleton/models/Skeleton02(SKETCHFAB).glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Squelette complet 3", "assets/skeleton/models/uploads-files-3263880-Skelet.obj", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Crâne 1", "assets/skeleton/models/Skull.obj", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Crâne 2", "assets/skeleton/models/Skull-2.obj", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Fémur", "assets/skeleton/models/Femur1_copy.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Tibia", "assets/skeleton/models/Tibia1_1.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Rotule", "assets/skeleton/models/Patella 1.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Péroné", "assets/skeleton/models/Fibula1.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Respiratoire", "Système respiratoire");
    layers.back().AddModel("assets/lungs/models/211119 Lungs.3ds", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Poumons 1", "assets/lungs/models/211119 Lungs.3ds", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Poumons 2", "assets/lungs/models/Lungs With Texture.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/lungs/textures/all_low_lunghs_BaseColor.lungh_part01.jpeg");
    layers.back().AddSubModel("Trachée et larynx 1", "assets/lungs/models/human_lungs_trachea_and_larynx.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Trachée et larynx 2", "assets/lungs/models/human_lungs_trachea_larynx_hyoid_bone.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Diaphragme", "assets/diaphragm/models/diaphragm.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/diaphragm/textures/diaphragm_AlbedoTransparency.png");
    layers.back().SetTransparency(0.8f);
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Digestif", "Système digestif");
    layers.back().AddModel("assets/digestive/models/stomach.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/digestive/textures/stomach_Div_01_BaseColor.stomach.jpeg");
    layers.back().AddSubModel("Estomac", "assets/digestive/models/stomach.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/digestive/textures/stomach_Div_01_BaseColor.stomach.jpeg");
    layers.back().AddSubModel("Système digestif", "assets/digestive/models/digestive-system.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Colon", "assets/digestive/models/colon.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Nerveux", "Système nerveux");
    layers.back().AddModel("assets/brain/models/Brain.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cerveau 1", "assets/brain/models/Brain.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cerveau 2", "assets/brain/models/Rotten Brain.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cerveau 3", "assets/brain/models/woman_brain.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cerveau 4", "assets/brain/models/woman_brain.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cerveau et moelle épinière", "assets/brain/models/brain__spinal_cord.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Système nerveux 1", "assets/nerves/models/nervousSystem.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Système nerveux 2", "assets/nerves/models/nerves.gltf", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Lymphatique", "Système lymphatique");
    layers.back().AddModel("assets/lymphatic/models/oveview lymphatic system.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Système lymphatique", "assets/lymphatic/models/oveview lymphatic system.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetTransparency(0.8f);
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Muscles", "Système musculaire");
    layers.back().AddModel("assets/muscles/models/male_full_body_ecorche.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/muscles/textures/anatomy_body_2.jpeg");
    layers.back().AddSubModel("Adductor Magnus", "assets/muscles/models/Adductor magnus.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Biceps Femoris", "assets/muscles/models/Biceps femoris (long).OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Gastrocnemius", "assets/muscles/models/Gastrocnemius.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Gracilis", "assets/muscles/models/Gracilis.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Rectus Femoris", "assets/muscles/models/Rectus femoris.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Sartorius", "assets/muscles/models/Sartorius.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Écorché complet", "assets/muscles/models/male_full_body_ecorche.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/muscles/textures/anatomy_body_2.jpeg");
    layers.back().AddSubModel("Sternocleidomastoid", "assets/muscles/models/sternocleidomastoid_muscle.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetTransparency(0.7f);
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Peau", "Couche externe");
    layers.back().AddModel("assets/skin/models/Human bodies_regular.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/skin/textures/HumanBase__color.png");
    layers.back().AddSubModel("Corps humain 1", "assets/skin/models/Human bodies_regular.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/skin/textures/HumanBase__color.png");
    layers.back().AddSubModel("Corps humain 2", "assets/skin/models/uploads-files-4141441-HumanAnatomy_woman_test+model.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetTransparency(0.5f);
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Cardiovasculaire", "Système cardiovasculaire");
    layers.back().AddModel("assets/heart/models/HumanHeart_FBX.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cœur 1", "assets/heart/models/HumanHeart_FBX.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cœur 2", "assets/heart/models/heart-animated-and-bronchial-airways.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Cœur 3", "assets/heart/models/model hart in thorax met kleppen versie 9 december.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Urinaire", "Système urinaire");
    layers.back().AddModel("assets/kidneys/models/Kidney_FBX.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/kidneys/textures/Kidney_LF_KidneyM_Diffuse.1001.jpg");
    layers.back().AddSubModel("Reins 1", "assets/kidneys/models/Kidney_FBX.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/kidneys/textures/Kidney_LF_KidneyM_Diffuse.1001.jpg");
    layers.back().AddSubModel("Reins 2", "assets/kidneys/models/Kidney for sketchfab.OBJ", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/kidneys/textures/bitmap_kidney.jpg");
    layers.back().AddSubModel("Reins 3", "assets/kidneys/models/kidneysfbx.fbx", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/kidneys/textures/bitmap_kidney.jpg");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Endocrinien", "Système endocrinien");
    layers.back().AddModel("assets/endocrine/models/thyroid_gland.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Thyroïde", "assets/endocrine/models/thyroid_gland.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Surrénales", "assets/endocrine/models/adrenal_glands.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    layers.emplace_back("Reproducteur", "Système reproducteur");
    layers.back().AddModel("assets/genital/models/reproductive_system.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().AddSubModel("Vessie et uretères 1", "assets/genital/models/mesane ve üreterler.stl", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "assets/genital/textures/bitmap_bladder.jpg");
    layers.back().AddSubModel("Vessie et uretères 2", "assets/genital/models/reproductive_system.glb", bufferManager, glm::vec3(0.0f), glm::vec3(0.1f), "");
    layers.back().SetScale(glm::vec3(0.1f));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ParticleSystem particleSystem(100);

    std::string selectedSystem = "Squelette";
    std::string selectedSubModel = "Squelette complet 1";
    float globalTransparency = 1.0f;
    float zoom = 1.0f;
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    ImVec4 tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    int score = 0;
    TargetPart target = {"Cardiovasculaire", "Cœur 1"};
    bool showDetailed = true;
    std::vector<bool> layerVisibility(layers.size(), true);

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 lightSpaceMatrix;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, camera, deltaTime);

        // Shadow mapping pass
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 20.0f);
        glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        for (size_t i = 0; i < layers.size(); ++i) {
            if (layerVisibility[i]) {
                auto& layer = layers[i];
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                if (layer.GetName() == selectedSystem && !selectedSubModel.empty()) {
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                depthShader.setMat4("model", modelMatrix);
                layer.Draw(depthShader, lightView, lightProjection, modelMatrix, true);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Main render pass
        glViewport(0, 0, windowWidth, windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        skybox.Draw(skyboxShader, view, projection);

        glEnable(GL_BLEND);
        particleSystem.Draw(particleShader, view, projection, camera.Position);
        glDisable(GL_BLEND);

        for (size_t i = 0; i < layers.size(); ++i) {
            if (layerVisibility[i]) {
                auto& layer = layers[i];
                layer.SetTransparency(globalTransparency);
                shader.use();
                shader.setVec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
                shader.setVec3("lightDirColor", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.setVec3("lightPointPos", glm::vec3(0.0f, 2.0f, 2.0f));
                shader.setVec3("lightPointColor", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.setFloat("lightPointConstant", 1.0f);
                shader.setFloat("lightPointLinear", 0.09f);
                shader.setFloat("lightPointQuadratic", 0.032f);
                shader.setVec3("viewPos", camera.Position);
                shader.setFloat("alpha", globalTransparency);
                shader.setVec4("tintColor", glm::vec4(tintColor.x, tintColor.y, tintColor.z, tintColor.w));
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthMap);
                shader.setInt("shadowMap", 1);
                shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(zoom * 0.1f));
                shader.setMat4("view", view);
                shader.setMat4("projection", projection);
                shader.setMat4("model", modelMatrix);

                if (layer.GetName() == selectedSystem) {
                    if (showDetailed) {
                        layer.Draw(shader, view, projection, modelMatrix);
                        for (const auto& subModel : layer.GetSubModels()) {
                            if (subModel.first == selectedSubModel) {
                                layer.DrawSubModel(subModel.first, shader);
                            }
                        }
                    }
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        postProcessShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessTexture);
        postProcessShader.setInt("screenTexture", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(windowWidth - 350, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(340, windowHeight - 20), ImGuiCond_Always);
        ImGui::Begin("Visualisation Anatomique", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Sélectionnez une couche pour les détails :");
        if (ImGui::BeginCombo("##SystemCombo", selectedSystem.c_str())) {
            for (const auto& layer : layers) {
                bool isSelected = (selectedSystem == layer.GetName());
                if (ImGui::Selectable(layer.GetName().c_str(), isSelected)) {
                    selectedSystem = layer.GetName();
                    for (auto& l : layers) {
                        l.SetTransparency(l.GetName() == selectedSystem ? 1.0f : 0.0f);
                    }
                    selectedSubModel = layer.GetSubModels().empty() ? layer.GetName() : layer.GetSubModels().begin()->first;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Visibilité des couches:");
        for (size_t i = 0; i < layers.size(); ++i) {
            bool visible = layerVisibility[i];
            if (ImGui::Checkbox(("Afficher " + layers[i].GetName()).c_str(), &visible)) {
                layerVisibility[i] = visible;
            }
        }

        ImGui::Text("Transparence globale :");
        ImGui::SliderFloat("##Transparency", &globalTransparency, 0.0f, 1.0f, "%.2f");
        for (auto& layer : layers) {
            layer.SetTransparency(globalTransparency * (layer.GetTransparency() > 0.0f ? 1.0f : 0.0f));
        }

        ImGui::Checkbox("Afficher la couche détaillée", &showDetailed);

        ImGui::Text("Sélectionnez un sous-modèle :");
        if (ImGui::BeginCombo("##SubModelCombo", selectedSubModel.c_str())) {
            for (const auto& layer : layers) {
                if (layer.GetName() == selectedSystem) {
                    for (const auto& subModel : layer.GetSubModels()) {
                        bool isSelected = (selectedSubModel == subModel.first);
                        if (ImGui::Selectable(subModel.first.c_str(), isSelected)) {
                            selectedSubModel = subModel.first;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Zoom");
        ImGui::SliderFloat("##Zoom", &zoom, 0.5f, 10.0f, "x%.1f");

        ImGui::Text("Rotation du sous-modèle :");
        ImGui::SliderFloat3("##Rotation", &rotation[0], -180.0f, 180.0f, "%.0f deg");

        ImGui::Text("Teinte du sous-modèle :");
        ImGui::ColorEdit4("##Tint", (float*)&tintColor);

        ImGui::Text("Score: %d", score);
        if (ImGui::Button("Identifier le Cœur")) {
            if (selectedSystem == target.system && selectedSubModel == target.part) {
                score += 10;
                std::cout << "Correct ! Score : " << score << std::endl;
            } else {
                std::cout << "Incorrect ! Essayez encore." << std::endl;
            }
        }
        if (ImGui::Button("Réinitialiser le score")) {
            score = 0;
        }

        for (const auto& layer : layers) {
            if (layer.GetName() == selectedSystem) {
                ImGui::Text("Description : %s", layer.GetDescription().c_str());
                break;
            }
        }

        if (ImGui::Button("Réinitialiser")) {
            zoom = 1.0f;
            rotation = glm::vec3(0.0f);
            globalTransparency = 1.0f;
            tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            std::fill(layerVisibility.begin(), layerVisibility.end(), true);
            selectedSystem = "Squelette";
            selectedSubModel = "Squelette complet 1";
            showDetailed = true;
            score = 0;
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}