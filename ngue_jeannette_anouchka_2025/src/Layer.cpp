#include "Layer.h"
#include "Model.h"
#include "shaders.h"
#include <iostream>
#include <fstream> // For std::ifstream
#include <glm/gtc/matrix_transform.hpp>

Layer::Layer(const std::string& name, const std::string& modelPath, BufferManager& bufferManager)
    : name(name), description("Default description"), transparency(1.0f), scale(1.0f, 1.0f, 1.0f), visible(true), bufferManager_(bufferManager) {
    std::ifstream file(modelPath);
    if (!file.good()) {
        std::cerr << "Error: Model file not found at " << modelPath << std::endl;
        return;
    }
    file.close();
    mainModel = Model(modelPath, bufferManager_);
}

void Layer::AddSubModel(const std::string& subModelName, const std::string& modelPath, const glm::vec3& position, const glm::vec3& scale, const std::string& shaderName) {
    std::ifstream file(modelPath);
    if (!file.good()) {
        std::cerr << "Error: SubModel file not found at " << modelPath << std::endl;
        return;
    }
    file.close();
    Model subModel(modelPath, bufferManager_, position, scale, shaderName.empty() ? "default_texture.png" : shaderName);
    subModels[subModelName] = subModel;
}

void Layer::DrawSubModel(const std::string& subModelName, Shader& shader) {
    auto it = subModels.find(subModelName);
    if (it != subModels.end()) {
        it->second.Render(shader);
    } else {
        std::cerr << "SubModel " << subModelName << " not found in layer " << name << std::endl;
    }
}

void Layer::SetTransparency(float transparency) {
    this->transparency = transparency;
}

void Layer::Draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, bool useTransparency) {
    if (!visible) return;
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);
    if (useTransparency) {
        shader.setFloat("transparency", transparency);
    }
    mainModel.Render(shader);
    for (auto& subModel : subModels) {
        subModel.second.Render(shader);
    }
}

void Layer::AddModel(const std::string& modelPath, const glm::vec3& position, const glm::vec3& scale, const std::string& shaderName) {
    std::ifstream file(modelPath);
    if (!file.good()) {
        std::cerr << "Error: Model file not found at " << modelPath << std::endl;
        return;
    }
    file.close();
    Model newModel(modelPath, bufferManager_, position, scale, shaderName.empty() ? "default_texture.png" : shaderName);
    models.push_back(newModel);
}

void Layer::SetScale(const glm::vec3& scale) {
    this->scale = scale;
}

std::string Layer::GetName() const {
    return name;
}

std::string Layer::GetDescription() const {
    return description;
}

std::map<std::string, Model> Layer::GetSubModels() const {
    return subModels;
}

float Layer::GetTransparency() const {
    return transparency;
}