#ifndef THUMBNAILSMASTER_H
#define THUMBNAILSMASTER_H

#include "../../Render/headers/zs-shader.h"
#include "../../Render/headers/zs-texture.h"
#include "../../Render/headers/zs-mesh.h"
#include "../../Render/headers/MatShaderProps.h"
#include "../../Render/headers/zs-pipeline.h"
#include "EditorManager.h"
#include <map>
#include <QPixmap>

class ThumbnailsMaster : public EditorComponentManager{
public:

    std::map<std::string, QImage*> texture_thumbnails;

    ZSPIRE::Shader texture_shader;
    //virtual void init();
    //virtual void updateWindowSize(int W, int H);

    void initShader();
    void createTexturesThumbnails();
    void createMaterialThumbnails();

    void DrawTexture(ZSPIRE::Texture* texture);
    void DrawMaterial(Material* material);
    void DrawMesh(ZSPIRE::Mesh* mesh);

    ThumbnailsMaster();
    ~ThumbnailsMaster();
};

#endif // THUMBNAILSMASTER_H
