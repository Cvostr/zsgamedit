#ifndef THUMBNAILSMASTER_H
#define THUMBNAILSMASTER_H

#include "../../Render/headers/zs-shader.h"
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

    ThumbnailsMaster();
    ~ThumbnailsMaster();
};

#endif // THUMBNAILSMASTER_H
