#include "../headers/obj_properties.h"
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../headers/Misc.h"

//selected terrain
static TerrainProperty* current_terrain_prop;
extern ZSGAME_DATA* game_data;
extern Project* project_ptr;
extern InspectorWin* _inspector_win;

TerrainProperty::TerrainProperty(){
    type = GO_PROPERTY_TYPE_TERRAIN;

    this->Width = 500;
    this->Length = 500;
    this->MaxHeight = 500;
    GrassDensity = 1.0f;
    castShadows = true;
    textures_size = 0;
    grassType_size = 0;

    this->range = 15;
    this->editHeight = 10;
    this->textureid = 1;
    this->vegetableid = 1;

    edit_mode = 1;

    rigidBody = nullptr;
}

void onClearTerrain(){
    current_terrain_prop->getTerrainData()->alloc(current_terrain_prop->Width, current_terrain_prop->Length);
    current_terrain_prop->getTerrainData()->generateGLMesh();
}

void TerrainProperty::addPropertyInterfaceToInspector(){
    current_terrain_prop = this;

    IntPropertyArea* HWidth = new IntPropertyArea; //New property area
    HWidth->setLabel("Heightmap Width"); //Its label
    HWidth->value = &this->Width; //Ptr to our vector
    HWidth->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(HWidth);

    IntPropertyArea* HLength = new IntPropertyArea; //New property area
    HLength->setLabel("Heightmap Length"); //Its label
    HLength->value = &this->Length; //Ptr to our vector
    HLength->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(HLength);

    IntPropertyArea* MHeight = new IntPropertyArea; //New property area
    MHeight->setLabel("Max Height"); //Its label
    MHeight->value = &this->MaxHeight; //Ptr to our vector
    MHeight->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MHeight);

    BoolCheckboxArea* castShdws = new BoolCheckboxArea;
    castShdws->setLabel("Cast Shadows ");
    castShdws->go_property = static_cast<void*>(this);
    castShdws->bool_ptr = &this->castShadows;
    _inspector_win->addPropertyArea(castShdws);

    //Add button to add objects
    AreaButton* clear_btn = new AreaButton;
    clear_btn->onPressFuncPtr = &onClearTerrain;
    clear_btn->button->setText("Clear"); //Setting text to qt button
    _inspector_win->getContentLayout()->addWidget(clear_btn->button);
    clear_btn->insp_ptr = _inspector_win; //Setting inspector pointer
    _inspector_win->registerUiObject(clear_btn);

    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->edit_mode);
    group->go_property = static_cast<void*>(this);

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
    EditRange->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(EditRange);
    //If selected mode is Height paint
    if(edit_mode == 1){

        FloatPropertyArea* EditHeight = new FloatPropertyArea; //New property area
        EditHeight->setLabel("brush height"); //Its label
        EditHeight->value = &this->editHeight; //Ptr to our vector
        EditHeight->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(EditHeight);
    }
    //if selected mode is texture paint
    if(edit_mode == 2){

        AreaRadioGroup* texturegroup_pick = new AreaRadioGroup; //allocate button layout
        texturegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->textureid);
        texturegroup_pick->go_property = static_cast<void*>(this);

        IntPropertyArea* tSize = new IntPropertyArea; //New property area
        tSize->setLabel("Textures"); //Its label
        tSize->value = &this->textures_size; //Ptr to our vector
        tSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(tSize);

        for(int i = 0; i < this->textures_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Group " + QString::number(i));
            if(textureid == i + 1)
                group_radio->setChecked(true);
            //add created radio button
            texturegroup_pick->addRadioButton(group_radio);
            _inspector_win->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = static_cast<void*>(this);
            diffuse_area->rel_path_std = &textures[static_cast<unsigned int>(i)].diffuse_relpath;
            _inspector_win->addPropertyArea(diffuse_area);

            PickResourceArea* normal_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            normal_area->setLabel("Normal");
            normal_area->go_property = static_cast<void*>(this);
            normal_area->rel_path_std = &textures[static_cast<unsigned int>(i)].normal_relpath;
            _inspector_win->addPropertyArea(normal_area);
        }
        //Add texture picker UI elements
        _inspector_win->registerUiObject(texturegroup_pick);

    }
    //if selected mode is vegetable paint
    if(edit_mode == 3){

        AreaRadioGroup* vegetablegroup_pick = new AreaRadioGroup; //allocate button layout
        vegetablegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->vegetableid);
        vegetablegroup_pick->go_property = static_cast<void*>(this);

        IntPropertyArea* vSize = new IntPropertyArea; //New property area
        vSize->setLabel("Grass variants"); //Its label
        vSize->value = &this->grassType_size; //Ptr to our vector
        vSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(vSize);

        FloatPropertyArea* fDensity = new FloatPropertyArea; //New property area
        fDensity->setLabel("Grass density"); //Its label
        fDensity->value = &this->GrassDensity; //Ptr to our vector
        fDensity->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        _inspector_win->addPropertyArea(fDensity);

        for(int i = 0; i < this->grassType_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Veg " + QString::number(i));
            if(vegetableid == i)
                group_radio->setChecked(true);
            //add created radio button
            vegetablegroup_pick->addRadioButton(group_radio);
            _inspector_win->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = static_cast<void*>(this);
            diffuse_area->rel_path_std = &this->grass[static_cast<unsigned int>(i)].diffuse_relpath;
            _inspector_win->addPropertyArea(diffuse_area);

            Float2PropertyArea* GrassSize = new Float2PropertyArea; //New property area
            GrassSize->setLabel("Heightmap Length"); //Its label
            GrassSize->vector = &this->grass[static_cast<unsigned int>(i)].scale; //Ptr to our vector
            GrassSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(GrassSize);

        }

        //Add texture picker UI elements
        _inspector_win->registerUiObject(vegetablegroup_pick);
    }
}

void TerrainProperty::DrawMesh(RenderPipeline* pipeline){
    data.Draw(false);

    float delta = 1.0f / GrassDensity;

    pipeline->grass_shader->Use();
    pipeline->transformBuffer->bind();
    for(float texelZ = 0; texelZ < data.W; texelZ += delta){
        for(float texelX = 0; texelX < data.H; texelX += delta){

            int texelXi = floor(texelX);
            int texelZi = floor(texelZ);

            Engine::TransformProperty* t_ptr = ((GameObject*)(this->go_link.ptr))->getPropertyPtr<Engine::TransformProperty>();

            HeightmapTexel* texel_ptr = &this->data.data[texelZi * data.W + texelXi];
            if(texel_ptr->grass > 0){
                HeightmapGrass* grass = &this->grass[static_cast<unsigned int>(texel_ptr->grass - 1)];
                ZSVECTOR3 pos = t_ptr->translation + ZSVECTOR3(texelZ, texel_ptr->height, texelX);
                ZSMATRIX4x4 m = getScaleMat(ZSVECTOR3(grass->scale.X, grass->scale.Y, grass->scale.X)) * getTranslationMat(pos);

                grass->diffuse->Use(0);

                pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &m);
                Engine::getGrassMesh()->Draw();
            }
        }
    }
}

void TerrainProperty::onUpdate(float deltaTime){
    if(data.hasPhysicShapeChanged){
        //data.initPhysics();
        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

        //Declare start transform
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3( btScalar(transform->abs_translation.X), btScalar(transform->abs_translation.Y),
                                                    btScalar(transform->abs_translation.Z)));


         //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
         btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

         btRigidBody::btRigidBodyConstructionInfo cInfo(0, myMotionState, data.shape, btVector3(0,0,0));

         if(rigidBody != nullptr){
             world_ptr->physical_world->removeRidigbodyFromWorld(rigidBody);
             delete rigidBody;
         }
         rigidBody = new btRigidBody(cInfo);

         rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
         //add rigidbody to world
         go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);

         data.hasPhysicShapeChanged = false;
    }
}

void TerrainProperty::onValueChanged(){
    //check for limitation of texture groups
    if(this->textures_size > 12)
        textures_size = 12;

    if(_last_edit_mode != edit_mode){
        _last_edit_mode = edit_mode;
        _inspector_win->updateRequired = true;
    }
    //if amount of texture pairs changed
    if(static_cast<unsigned int>(this->textures_size) != textures.size()){
        textures.resize(static_cast<unsigned int>(this->textures_size));
        _inspector_win->updateRequired = true;
    }
    //if amount of vegetables changed
    if(static_cast<unsigned int>(this->grassType_size) != grass.size()){
        grass.resize(static_cast<unsigned int>(this->grassType_size));
        _inspector_win->updateRequired = true;
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i ++){
        HeightmapTexturePair* pair = &this->textures[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
        pair->normal = game_data->resources->getTextureByLabel(pair->normal_relpath);
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->grassType_size); i ++){
        HeightmapGrass* pair = &this->grass[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
    }
}

void TerrainProperty::onAddToObject(){
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

void TerrainProperty::getPickedVertexId(int posX, int posY, int screenY, unsigned char* data){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //get pointer to material property
    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
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

void TerrainProperty::modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold){
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

void TerrainProperty::onMouseClick(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        getPickedVertexId(posX, posY, screenY, &_data[0]);
        //find picked texel
        modifyTerrainVertex(&_data[0], isCtrlHold);
    }
}

void TerrainProperty::onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        getPickedVertexId(posX, posY, screenY, &_data[0]);
        //find picked texel
        modifyTerrainVertex(&_data[0], isCtrlHold);
    }
}

TerrainData* TerrainProperty::getTerrainData(){
    return &data;
}

void TerrainProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    TerrainProperty* _dest = static_cast<TerrainProperty*>(dest);
    _dest->Width = this->Width;
    _dest->Length = this->Length;
    _dest->MaxHeight = this->MaxHeight;
    _dest->GrassDensity = this->GrassDensity;
    _dest->file_label = this->file_label;
    _dest->castShadows = this->castShadows;
    _dest->textures_size = this->textures_size;
    _dest->grassType_size = this->grassType_size;
    //Copying terrain data
    data.copyTo(&_dest->data);
    //Copy textures data
    for(unsigned int t_i = 0; t_i < this->textures.size(); t_i ++)
        _dest->textures.push_back(textures[t_i]);

    for(unsigned int g_i = 0; g_i < this->grass.size(); g_i ++)
        _dest->grass.push_back(grass[g_i]);
}
