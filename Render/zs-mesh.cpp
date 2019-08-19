#include "headers/zs-mesh.h"

#include <iostream>
#include <GL/glew.h>
#include <math.h>

#define SPHERE_SECTORS 36
#define SPHERE_STACKS 18

static ZSPIRE::Mesh* picked_mesh = nullptr;

static ZSVERTEX plane_verts[] = {
	// positions              // texture coords
	ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, 0.0f),		ZSVECTOR2(1.0f, 1.0f)),   // top right
	ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, 0.0f),		ZSVECTOR2(1.0f, 0.0f)),   // bottom right
	ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, 0.0f),		ZSVECTOR2(0.0f, 0.0f)),   // bottom left
	ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, 0.0f),		ZSVECTOR2(0.0f, 1.0f))   // top left 
};

static ZSVERTEX ui_sprite_vertices[] = {
	// positions // texture coords 
	ZSVERTEX(ZSVECTOR3(1.0f, 1.0f, 0.0f), ZSVECTOR2(1.0f, 0.0f)), // top right 
	ZSVERTEX(ZSVECTOR3(1.0f, 0.0f, 0.0f), ZSVECTOR2(1.0f, 1.0f)), // bottom right 
	ZSVERTEX(ZSVECTOR3(0.0f, 0.0f, 0.0f), ZSVECTOR2(0.0f, 1.0f)), // bottom left 
	ZSVERTEX(ZSVECTOR3(0.0f, 1.0f, 0.0f), ZSVECTOR2(0.0f, 0.0f)) // top left 
};

static ZSVERTEX iso_tile_vertices[] = {
	// positions              // texture coords
	ZSVERTEX(ZSVECTOR3(0.0f,  1.0f, 0.0f),		ZSVECTOR2(0.5f, 1.0f)), 
	ZSVERTEX(ZSVECTOR3(1.0f, 0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.55f)),   
	ZSVERTEX(ZSVECTOR3(1.0f, -0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.45f)),  
	ZSVERTEX(ZSVECTOR3(0.0f,  -1.0f, 0.0f),		ZSVECTOR2(0.5f, 0.0f)),   
	ZSVERTEX(ZSVECTOR3(-1.0f,  -0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.45f)),
	ZSVERTEX(ZSVECTOR3(-1.0f,  0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.55f))   // top left 
};
static float cube_vertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
    1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
    1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // right face
    1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
    1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
    1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
    1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
    1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
    1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
};

float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

static unsigned int plane_inds[] = { 0,1,2, 0,2,3 };
static unsigned int isotile_ind[] = { 0,1,2, 2,3,4, 2,4,5, 2,5,0 };

static ZSPIRE::Mesh plane2Dmesh;
static ZSPIRE::Mesh uiSprite2Dmesh;
static ZSPIRE::Mesh iso_tile2Dmesh;
static ZSPIRE::Mesh cube3Dmesh;
static ZSPIRE::Mesh skyboxMesh;
static ZSPIRE::Mesh sphereMesh;

ZSPIRE::Mesh::Mesh() {
	this->alive = false;
}

void ZSPIRE::Mesh::Init() {
    glGenVertexArrays(1, &this->meshVAO);
    glGenBuffers(1, &this->meshVBO);
    glGenBuffers(1, &this->meshEBO);
}

void ZSPIRE::setupDefaultMeshes() {
    picked_mesh = nullptr;

    plane2Dmesh.Init(); //Initialize mesh for GL
    plane2Dmesh.setMeshData(plane_verts, plane_inds, 4, 6); //Send plane data

    uiSprite2Dmesh.Init();
    uiSprite2Dmesh.setMeshData(ui_sprite_vertices, plane_inds, 4, 6);

    iso_tile2Dmesh.Init();
    iso_tile2Dmesh.setMeshData(iso_tile_vertices, isotile_ind, 6, 12);

    cube3Dmesh.Init();
    cube3Dmesh.vertices_num = 36;
    cube3Dmesh.indices_num = 0;
    glBindBuffer(GL_ARRAY_BUFFER, cube3Dmesh.meshVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindVertexArray(cube3Dmesh.meshVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    skyboxMesh.Init();
    skyboxMesh.vertices_num = 36;
    skyboxMesh.indices_num = 0;
    glBindVertexArray(skyboxMesh.meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxMesh.meshVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    sphereMesh.Init();
    std::vector<ZSVERTEX> sphere_v;
    std::vector<unsigned int> sphere_indices;
    float radius = 1.f;
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * ZS_PI / SPHERE_SECTORS;
    float stackStep = ZS_PI / SPHERE_STACKS;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= SPHERE_STACKS; ++i)
    {
        stackAngle = ZS_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= SPHERE_SECTORS; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            ZSVERTEX v;

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            v.pos = ZSVECTOR3(x, y, z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            v.normal = ZSVECTOR3(nx, ny, nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = static_cast<float>(j) / SPHERE_SECTORS;
            t = static_cast<float>(i) / SPHERE_STACKS;
            v.uv = ZSVECTOR2(s, t);
            sphere_v.push_back(v);
        }
    }
    unsigned int k1, k2;
    for(unsigned int i = 0; i < SPHERE_STACKS; ++i)
    {
        k1 = i * (SPHERE_SECTORS + 1);     // beginning of current stack
        k2 = k1 + SPHERE_SECTORS + 1;      // beginning of next stack

        for(int j = 0; j < SPHERE_SECTORS; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                sphere_indices.push_back(k1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (SPHERE_STACKS-1))
            {
                sphere_indices.push_back(k1 + 1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k2 + 1);
            }
        }
    }
    sphereMesh.setMeshData(sphere_v.data(), sphere_indices.data(), static_cast<uint>(sphere_v.size()), static_cast<uint>(sphere_indices.size())); //Send plane data
}

void ZSPIRE::freeDefaultMeshes(){
    plane2Dmesh.Destroy();
    uiSprite2Dmesh.Destroy();
    iso_tile2Dmesh.Destroy();
    cube3Dmesh.Destroy();
    skyboxMesh.Destroy();
    sphereMesh.Destroy();
}

ZSPIRE::Mesh* ZSPIRE::getPlaneMesh2D() {
	return &plane2Dmesh;
}

ZSPIRE::Mesh* ZSPIRE::getUiSpriteMesh2D() {
	return &uiSprite2Dmesh;
}
ZSPIRE::Mesh* ZSPIRE::getSphereMesh(){
    return &sphereMesh;
}
ZSPIRE::Mesh* ZSPIRE::getIsoTileMesh2D(){
	return &iso_tile2Dmesh;
}
ZSPIRE::Mesh* ZSPIRE::getCubeMesh3D(){
    return &cube3Dmesh;
}
ZSPIRE::Mesh* ZSPIRE::getSkyboxMesh(){
    return &skyboxMesh;
}
void ZSPIRE::Mesh::Destroy() {
	this->alive = false;

    glDeleteVertexArrays(1, &this->meshVAO); //Removevertex arrays
    glDeleteBuffers(1, &this->meshVBO); //Remove vertex buffer
    glDeleteBuffers(1, &this->meshEBO); //Remove index buffer

}

void ZSPIRE::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num) {
	this->vertices_num = vertices_num;
	this->indices_num = indices_num;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices_num) * static_cast<int>(sizeof(ZSVERTEX)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(indices_num) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

	//Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), nullptr);
    glEnableVertexAttribArray(0);
	//Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
	//Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

	//Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
	//Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

}
void ZSPIRE::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num) {
	this->vertices_num = vertices_num;
	this->indices_num = NO_INDICES;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(ZSMATRIX4x4), vertices, GL_STATIC_DRAW); //send vertices to buffer

	//Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), nullptr);
    glEnableVertexAttribArray(0);
	//Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
	//Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

	//Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
	//Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);
}

void ZSPIRE::Mesh::Draw(){

    if(picked_mesh != this){ //if this mesh wasn't picked
        picked_mesh = this; //Setting pointer of this obj
        glBindVertexArray(this->meshVAO);
        if(this->indices_num != NO_INDICES) //if object uses indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);
    }

	if (this->indices_num == NO_INDICES) {
		//Draw without indices
        glDrawArrays(GL_TRIANGLES, 0, this->vertices_num);
	}
	else {
		//Indexed draw
        glDrawElements(GL_TRIANGLES, this->indices_num, GL_UNSIGNED_INT, 0);
	}
}

void ZSPIRE::Mesh::DrawLines(){

    if(picked_mesh != this){ //if this mesh wasn't picked
        picked_mesh = this; //Setting pointer of this obj
        glBindVertexArray(this->meshVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO);
        if(this->indices_num != NO_INDICES) //if object uses indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);
    }

    if (this->indices_num == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_LINE_LOOP, 0, this->vertices_num);
    }
    else {
        //Indexed draw
        glDrawElements(GL_LINE_LOOP, this->indices_num, GL_UNSIGNED_INT, nullptr);
    }
}

void ZSPIRE::processTangentSpace(ZSVERTEX* vert_array, unsigned int* indices_array, int indices_num, int vertex_num){
    for(int ind_i = 0; ind_i < indices_num; ind_i += 3){
        ZSVERTEX v1 = vert_array[ind_i];
        ZSVERTEX v2 = vert_array[ind_i + 1];
        ZSVERTEX v3 = vert_array[ind_i + 2];

        ZSVECTOR3 edge1 = v2.pos - v1.pos;
        ZSVECTOR3 edge2 = v3.pos - v1.pos;
        ZSVECTOR2 deltaUV1 = v2.uv - v1.uv;
        ZSVECTOR2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

        ZSVECTOR3 tangent, bitangent;
        tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
        tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
        tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
        vNormalize(&tangent);

        bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
        bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
        bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
        vNormalize(&bitangent);
        for(int i = 0; i < 3; i ++){
            vert_array[ind_i + i].tangent = tangent;
            vert_array[ind_i + i].bitangent = bitangent;
        }
    }

    for(int ind_i = 0; ind_i < vertex_num; ind_i += 3){
        ZSVERTEX v1 = vert_array[ind_i];
        ZSVERTEX v2 = vert_array[ind_i + 1];
        ZSVERTEX v3 = vert_array[ind_i + 2];

        ZSVECTOR3 edge1 = v2.pos - v1.pos;
        ZSVECTOR3 edge2 = v3.pos - v1.pos;
        ZSVECTOR2 deltaUV1 = v2.uv - v1.uv;
        ZSVECTOR2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

        ZSVECTOR3 tangent, bitangent;
        tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
        tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
        tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
        vNormalize(&tangent);

        bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
        bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
        bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
        vNormalize(&bitangent);
        for(int i = 0; i < 3; i ++){
            vert_array[ind_i + i].tangent = tangent;
            vert_array[ind_i + i].bitangent = bitangent;
        }
    }
}
