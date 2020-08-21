#ifndef THUMBNAILSMASTER_H
#define THUMBNAILSMASTER_H

#include <render/zs-shader.h>
#include <render/zs-texture.h>
#include <render/zs-mesh.h>
#include <render/zs-materials.h>
#include <engine/loader.h>
#include "../../Render/headers/zs-pipeline.h"
#include "EditorManager.h"
#include <map>
#include <vector>
#include <QPixmap>

class ThumbnailsMaster : public EngineComponentManager{
public:

    std::map<std::string, QImage*> texture_thumbnails;
    std::vector<unsigned char*> memory_buffers;

    Engine::Shader* texture_shader;
    Engine::Shader* mesh_shader;

    //virtual void init();
    //virtual void updateWindowSize(int W, int H);

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

#endif // THUMBNAILSMASTER_H