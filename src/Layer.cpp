#include "Layer.h"
#include <iostream>

Layer::Layer(const std::string& name, const std::string& description) : name(name), description(description) {}

void Layer::AddModel(const std::string& modelPath, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath) {
    models.emplace_back(modelPath, bufferManager, position, scale, texturePath);
    mainModel = Model(modelPath, bufferManager, position, scale, texturePath);
}

void Layer::AddSubModel(const std::string& name, const std::string& modelPath, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath) {
    if (!std::filesystem::exists(modelPath)) {
        std::cout << "Error: Submodel file " << modelPath << " does not exist!" << std::endl;
        return;
    }
    subModels[name] = Model(modelPath, bufferManager, position, scale, texturePath);
}

void Layer::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMatrix, bool shadowPass) {
    for (auto& model : models) {
        model.Draw(shader, transparency, view, projection, modelMatrix, shadowPass);
    }
    mainModel.Draw(shader, transparency, view, projection, modelMatrix, shadowPass);
}

void Layer::DrawSubModel(const std::string& subModelName, Shader& shader) {
    auto it = subModels.find(subModelName);
    if (it != subModels.end()) {
        it->second.Draw(shader, transparency);
    } else {
        std::cout << "Error: Submodel " << subModelName << " not found!" << std::endl;
    }
}

void Layer::SetTransparency(float transparency) {
    this->transparency = transparency;
    for (auto& model : models) {
        model.SetTransparency(transparency);
    }
    mainModel.SetTransparency(transparency);
    for (auto& subModel : subModels) {
        subModel.second.SetTransparency(transparency);
    }
}

void Layer::SetScale(const glm::vec3& scale) {
    for (auto& model : models) {
        model.scale = scale;
    }
    mainModel.scale = scale;
    for (auto& subModel : subModels) {
        subModel.second.scale = scale;
    }
}