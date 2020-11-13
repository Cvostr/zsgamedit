#pragma once

#include <render/Shader.hpp>
#include <render/Texture.h>
#include <render/Mesh.hpp>
#include <render/Material.hpp>
#include <engine/loader.h>
#include "../../Render/headers/zs-pipeline.h"
#include "EditorManager.h"
#include <map>
#include <vector>
#include <QPixmap>

class ThumbnailsMaster : public IEngineComponent{
public:

    std::map<std::string, QImage*> texture_thumbnails;
    std::vector<unsigned char*> memory_buffers;

    Engine::Shader* texture_shader;
    Engine::Shader* mesh_shader;

    void prepareMaterialThumbnailPipeline();

    void initShader();
    void createTexturesThumbnails();
    void createMaterialThumbnails();
    void createMaterialThumbnail(std::string name);
    void createMeshesThumbnails();

    void DrawTexture(Engine::TextureResource* texture);
    void DrawMaterial(Material* material);
    void DrawMesh(Engine::MeshResource* mesh);

    bool isAvailable(std::string name);

    ThumbnailsMaster();
    ~ThumbnailsMaster();
};