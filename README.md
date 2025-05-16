# Projet Local – Visualisation Anatomique 3D

Projet réalisé par **Anouchka Nguejeannette** dans le cadre du cours **INFO-H502 (3D Graphics in VR)** – Université libre de Bruxelles.

## Description

Ce projet propose une **expérience immersive en 3D** permettant de visualiser les différentes couches anatomiques du corps humain, depuis le squelette jusqu’à la peau. Grâce à une interface interactive et des effets visuels réalistes, il constitue une **application éducative** pour l’apprentissage de l’anatomie humaine. Il a été conçu avec un souci de modularité, d’optimisation des ressources graphiques et de qualité de rendu.

## Fonctionnalités principales

- **Chargement de modèles 3D** via **Assimp** : formats pris en charge (`.obj`, `.fbx`, `.glb`, `.3ds`, etc.).
- **Shadow mapping** avec **Percentage-Closer Filtering (PCF)**.
- **Éclairage dynamique** directionnel et ponctuel (shaders GLSL configurables).
- **Skybox** en textures cubiques (salle d’opération).
- **Caméra interactive** avec zoom, pan et rotation (GLFW + GLM).
- **Interface utilisateur ImGui** : sélection des couches anatomiques, réglages (transparence, échelle, etc.).
- **Chargement asynchrone** des modèles.
- **Gestion des buffers** centralisée avec un `BufferManager`.
- **Effets visuels spécifiques** :
  - Transparence dynamique (peau, système lymphatique).
  - Simulation de respiration (scaling dynamique des poumons).
  - Billboarding pour les annotations et particules.

## Structure du projet

### Fichiers sources

- `main.cpp` : point d’entrée, initialisation OpenGL, ImGui, boucle de rendu.
- `Layer.h/cpp` : gestion des couches anatomiques, visibilités et paramètres.
- `Model.h/cpp` : chargement et rendu des modèles avec Assimp.
- `Mesh.h/cpp` : gestion des données géométriques (vertices, indices, normales, textures).
- `BufferManager.h/cpp` : création et gestion des VAOs, VBOs, EBOs.
- `Camera.h/cpp` : navigation et projection (vue FPS libre).
- `shaders.h/cpp` : chargement et compilation des shaders GLSL.
- `Skybox.h/cpp` : rendu environnemental à 360°.
- `stb_image.cpp` : chargement des textures avec STB.
- `CMakeLists.txt` : configuration du projet (GLFW, GLAD, Assimp, GLM, ImGui).

### Bibliothèques externes

Incluses dans le dossier `libs/` :
- `glad/`
- `glfw/`
- `assimp/`
- `glm/`
- `imgui/`

### Organisation des assets

- `assets/models/` : modèles 3D (`.obj`, `.fbx`, `.glb`, etc.).
- `assets/textures/` : textures (diffuse, normal map, specular, etc.).
- `shaders/` : fichiers `.frag`, `.vert`, etc.

### Répertoires additionnels

- `build/` : fichiers de compilation.
- `docs/` : documentation Doxygen (si générée).

## Prérequis

- `CMake ≥ 3.10`
- Compilateur compatible C++17 (`g++`, `clang`, etc.)
- `OpenGL ≥ 3.3`
- Bibliothèques listées ci-dessus (incluses dans `libs/`)

## Compilation et exécution

```bash
# Étapes pour lancer le projet
rm -rf build
cmake -B build
cmake --build build
./build/main
