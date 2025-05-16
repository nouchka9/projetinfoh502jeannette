#ifndef LAYER_H
#define LAYER_H

#include <string>
#include <map>
#include "Model.h"
#include "BufferManager.h"
#include "shaders.h"
#include <glm/glm.hpp>

class Layer {
public:
    Layer(const std::string& name, const std::string& description);
    void AddModel(const std::string& modelPath, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath);
    void AddSubModel(const std::string& name, const std::string& modelPath, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath);
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMatrix, bool shadowPass = false);
    void DrawSubModel(const std::string& subModelName, Shader& shader);
    void SetTransparency(float transparency);
    void SetScale(const glm::vec3& scale);
    float GetTransparency() const { return transparency; }
    std::string GetName() const { return name; }
    std::string GetDescription() const { return description; }
    std::map<std::string, Model>& GetSubModels() const { return subModels; }}

    std::vector<Model> models; // Legacy support for old structure

private:
    std::string name;
    std::string description;
    Model mainModel;
    std::map<std::string, Model> subModels;
    float transparency = 1.0f;
;

#endif