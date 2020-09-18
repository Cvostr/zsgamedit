#include <world/go_properties.h>
#include "../headers/terrain.h"
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../headers/Misc.h"

//selected terrain
static Engine::TerrainProperty* current_terrain_prop;
extern ZSGAME_DATA* game_data;
extern Project* project_ptr;
extern InspectorWin* _inspector_win;

void onClearTerrain(){
    current_terrain_prop->getTerrainData()->alloc(current_terrain_prop->Width, current_terrain_prop->Length);
    current_terrain_prop->getTerrainData()->generateGLMesh();
}

void Engine::TerrainProperty::addPropertyInterfaceToInspector(){
    current_terrain_prop = this;

    IntPropertyArea* HWidth = new IntPropertyArea; //New property area
    HWidth->setLabel("Heightmap Width"); //Its label
    HWidth->value = &this->Width; //Ptr to our vector
    HWidth->go_property = (this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(HWidth);

    IntPropertyArea* HLength = new IntPropertyArea; //New property area
    HLength->setLabel("Heightmap Length"); //Its label
    HLength->value = &this->Length; //Ptr to our vector
    HLength->go_property = (this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(HLength);

    IntPropertyArea* MHeight = new IntPropertyArea; //New property area
    MHeight->setLabel("Max Height"); //Its label
    MHeight->value = &this->MaxHeight; //Ptr to our vector
    MHeight->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MHeight);

    BoolCheckboxArea* castShdws = new BoolCheckboxArea;
    castShdws->setLabel("Cast Shadows ");
    castShdws->go_property = this;
    castShdws->bool_ptr = &this->castShadows;
    _inspector_win->addPropertyArea(castShdws);

    //Add button to add objects
    AreaButton* clear_btn = new AreaButton;
    clear_btn->onPressFuncPtr = &onClearTerrain;
    clear_btn->button->setText("Clear"); //Setting text to qt button
    _inspector_win->getContentLayout()->addWidget(clear_btn->button);
    _inspector_win->registerUiObject(clear_btn);

    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->edit_mode);
    group->go_property = this;
    group->updateInspectorOnChange = true;

    QRadioButton* directional_radio = new QRadioButton; //allocate first radio
    directional_radio->setText("Map");
    QRadioButton* point_radio = new QRadioButton;
    point_radio->setText("Texture");
    QRadioButton* veg_radio = new QRadioButton;
    veg_radio->setText("Vegetables");

    group->addRadioButton(directional_radio);
    group->addRadioButton(point_radio);
    group->addRadioButton(veg_radio);
    _inspector_win->registerUiObject(group);
    _inspector_win->getContentLayout()->addLayout(group->btn_layout);

    IntPropertyArea* EditRange = new IntPropertyArea; //New property area
    EditRange->setLabel("brush range"); //Its label
    EditRange->value = &this->range; //Ptr to our vector
    EditRange->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(EditRange);
    //If selected mode is Height paint
    if(edit_mode == 1){

        FloatPropertyArea* EditHeight = new FloatPropertyArea; //New property area
        EditHeight->setLabel("brush height"); //Its label
        EditHeight->value = &this->editHeight; //Ptr to our vector
        EditHeight->go_property = this; //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(EditHeight);
    }
    //if selected mode is texture paint
    if(edit_mode == 2){

        AreaRadioGroup* texturegroup_pick = new AreaRadioGroup; //allocate button layout
        texturegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->textureid);
        texturegroup_pick->go_property = this;

        IntPropertyArea* tSize = new IntPropertyArea; //New property area
        tSize->setLabel("Textures"); //Its label
        tSize->value = &this->textures_size; //Ptr to our vector
        tSize->go_property = this; //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(tSize);

        for(int i = 0; i < this->textures_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Group " + QString::number(i));
            group_radio->setChecked(false);
            if(textureid == i + 1)
                group_radio->setChecked(true);
            //add created radio button
            texturegroup_pick->addRadioButton(group_radio);
            _inspector_win->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = this;
            diffuse_area->rel_path_std = &textures[static_cast<unsigned int>(i)].diffuse_relpath;
            diffuse_area->isShowNoneItem = true;
            _inspector_win->addPropertyArea(diffuse_area);

            PickResourceArea* normal_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            normal_area->setLabel("Normal");
            normal_area->go_property = this;
            normal_area->rel_path_std = &textures[static_cast<unsigned int>(i)].normal_relpath;
            normal_area->isShowNoneItem = true;
            _inspector_win->addPropertyArea(normal_area);
        }
        //Add texture picker UI elements
        _inspector_win->registerUiObject(texturegroup_pick);

    }
    //if selected mode is vegetable paint
    if(edit_mode == 3){

        AreaRadioGroup* vegetablegroup_pick = new AreaRadioGroup; //allocate button layout
        vegetablegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->vegetableid);
        vegetablegroup_pick->go_property = this;

        IntPropertyArea* vSize = new IntPropertyArea; //New property area
        vSize->setLabel("Grass variants"); //Its label
        vSize->value = &this->grassType_size; //Ptr to our vector
        vSize->go_property = this; //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(vSize);

        FloatPropertyArea* fDensity = new FloatPropertyArea; //New property area
        fDensity->setLabel("Grass density"); //Its label
        fDensity->value = &data.GrassDensity; //Ptr to our vector
        fDensity->go_property = this; //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(fDensity);

        for(int i = 0; i < this->grassType_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Veg " + QString::number(i));
            group_radio->setChecked(false);
            if(vegetableid == i)
                group_radio->setChecked(true);
            //add created radio button
            vegetablegroup_pick->addRadioButton(group_radio);
            _inspector_win->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = this;
            diffuse_area->rel_path_std = &data.grass[static_cast<unsigned int>(i)].diffuse_relpath;
            diffuse_area->isShowNoneItem = true;
            _inspector_win->addPropertyArea(diffuse_area);

            Float2PropertyArea* GrassSize = new Float2PropertyArea; //New property area
            GrassSize->setLabel("Size "); //Its label
            GrassSize->vector = &data.grass[static_cast<unsigned int>(i)].scale; //Ptr to our vector
            GrassSize->go_property = this; //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(GrassSize);

        }

        //Add texture picker UI elements
        _inspector_win->registerUiObject(vegetablegroup_pick);
    }
}

void Engine::TerrainProperty::onValueChanged(){
    //check for limitation of texture groups
    if(this->textures_size > 12)
        textures_size = 12;

    //if amount of texture pairs changed
    if(static_cast<unsigned int>(this->textures_size) != textures.size()){
        textures.resize(static_cast<unsigned int>(this->textures_size));
        _inspector_win->updateRequired = true;
    }
    //if amount of vegetables changed
    if(static_cast<unsigned int>(this->grassType_size) != data.grass.size()){
        data.grass.resize(static_cast<unsigned int>(this->grassType_size));
        _inspector_win->updateRequired = true;
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i ++){
        HeightmapTexturePair* pair = &this->textures[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
        pair->normal = game_data->resources->getTextureByLabel(pair->normal_relpath);
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->grassType_size); i ++){
        HeightmapGrass* pair = &data.grass[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
    }
}

void Engine::TerrainProperty::onAddToObject(){
    std::string terrain_random_prefix;
    genRandomString(&terrain_random_prefix, 4);

    //relative path to terrain file
    this->file_label = *this->go_link.updLinkPtr()->label_ptr + "_" + terrain_random_prefix + ".terrain";
    //Allocate terrain
    data.alloc(this->Width, this->Length);
    //Generate opengl mesh to draw
    data.generateGLMesh();
    //absolute path to terrain file
    std::string fpath = project_ptr->root_path + "/" + this->file_label;
    //Create and save file
    data.saveToFile(fpath.c_str());
}

void Engine::TerrainProperty::getPickedVertexId(int posX, int posY, int screenY, unsigned char* data){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //get pointer to material property
    Engine::MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<Engine::MaterialProperty>();
    Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    if(mat == nullptr || mat->material_ptr == nullptr) return;
    //Apply material shader
    mat->material_ptr->applyMatToPipeline();
    //Bind terrain buffer and set isPicking to true
    terrainUniformBuffer->bind();
    int dtrue = 1;
    terrainUniformBuffer->writeData(16 * 12 * 2, 4, &dtrue);
    //Bind transform
    transformBuffer->bind();
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform->transform_mat);
    //Render terrain mesh without textures
    this->data.Draw(true);
    //read picked pixel
    glReadPixels(posX, screenY - posY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
}

void Engine::TerrainProperty::modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold){
    if((gl_data[0] + gl_data[1] + gl_data[2]) == 0) return;
    for(int i = 0; i < Width; i ++){
        for(int y = 0; y < Length; y ++){
            if(i == static_cast<int>(gl_data[0]) * 2 && y == static_cast<int>(gl_data[2]) * 2){
                int mul = 1;
                if(isCtrlHold)
                    mul *= -1;
                //apply change
                HeightmapModifyRequest* req = new HeightmapModifyRequest;
                req->terrain = &data;
                if(edit_mode == 1){
                    req->modify_type = TMT_HEIGHT;
                    req->originX = y;
                    req->originY = i;
                    req->originHeight = editHeight;
                    req->range = range;
                    req->multiplyer = mul;
                }else if(edit_mode == 2){
                    req->modify_type = TMT_TEXTURE;
                    req->originX = i;
                    req->originY = y;
                    req->range = range;
                    req->texture = static_cast<unsigned char>(textureid - 1);
                }else if(edit_mode == 3){
                    req->modify_type = TMT_GRASS;
                    req->originX = y;
                    req->originY = i;
                    req->range = range;
                    req->grass = vegetableid;
                    if(mul < 0)
                        req->grass = 0;
                }
                queryTerrainModifyRequest(req);
                return;
            }
        }
    }
}

void Engine::TerrainProperty::onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        getPickedVertexId(posX, posY, screenY, &_data[0]);
        //find picked texel
        modifyTerrainVertex(&_data[0], isCtrlHold);
    }
}

void Engine::TerrainProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeString(file_label);//Write material relpath
    //write dimensions
    stream->writeBinaryValue(&Width);
    stream->writeBinaryValue(&Length);
    stream->writeBinaryValue(&MaxHeight);
    stream->writeBinaryValue(&castShadows);
    stream->writeBinaryValue(&textures_size);
    stream->writeBinaryValue(&grassType_size);

    *stream << "\n";

    std::string fpath = project_ptr->root_path + "/" + file_label;
    getTerrainData()->saveToFile(fpath.c_str());
    //Write textures relative pathes
    for (int texture_i = 0; texture_i < textures_size; texture_i++) {
        HeightmapTexturePair* texture_pair = &textures[static_cast<unsigned int>(texture_i)];
        stream->writeString(texture_pair->diffuse_relpath);
        stream->writeString(texture_pair->normal_relpath);
    }
    //Write info about all vegetable types
    for (int grass_i = 0; grass_i < grassType_size; grass_i++) {
        HeightmapGrass* grass_ptr = &getTerrainData()->grass[static_cast<unsigned int>(grass_i)];
        //Write grass diffuse texture
        stream->writeString(grass_ptr->diffuse_relpath);
        //Write grass size
        stream->writeBinaryValue(&grass_ptr->scale.X);
        stream->writeBinaryValue(&grass_ptr->scale.Y);
        *stream << "\n";
    }
}