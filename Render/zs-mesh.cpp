#include "headers/zs-mesh.h"

#include <iostream>
#include <GL/glew.h>

ZSPIRE::Mesh* picked_mesh = 0x0;

ZSVERTEX plane_verts[] = {
	// positions              // texture coords
	ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, 0.0f),		ZSVECTOR2(1.0f, 1.0f)),   // top right
	ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, 0.0f),		ZSVECTOR2(1.0f, 0.0f)),   // bottom right
	ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, 0.0f),		ZSVECTOR2(0.0f, 0.0f)),   // bottom left
	ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, 0.0f),		ZSVECTOR2(0.0f, 1.0f))   // top left 
};

ZSVERTEX ui_sprite_vertices[] = {
	// positions // texture coords 
	ZSVERTEX(ZSVECTOR3(1.0f, 1.0f, 0.0f), ZSVECTOR2(1.0f, 0.0f)), // top right 
	ZSVERTEX(ZSVECTOR3(1.0f, 0.0f, 0.0f), ZSVECTOR2(1.0f, 1.0f)), // bottom right 
	ZSVERTEX(ZSVECTOR3(0.0f, 0.0f, 0.0f), ZSVECTOR2(0.0f, 1.0f)), // bottom left 
	ZSVERTEX(ZSVECTOR3(0.0f, 1.0f, 0.0f), ZSVECTOR2(0.0f, 0.0f)) // top left 
};

ZSVERTEX iso_tile_vertices[] = {
	// positions              // texture coords
	ZSVERTEX(ZSVECTOR3(0.0f,  1.0f, 0.0f),		ZSVECTOR2(0.5f, 1.0f)), 
	ZSVERTEX(ZSVECTOR3(1.0f, 0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.55f)),   
	ZSVERTEX(ZSVECTOR3(1.0f, -0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.45f)),  
	ZSVERTEX(ZSVECTOR3(0.0f,  -1.0f, 0.0f),		ZSVECTOR2(0.5f, 0.0f)),   
	ZSVERTEX(ZSVECTOR3(-1.0f,  -0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.45f)),
	ZSVERTEX(ZSVECTOR3(-1.0f,  0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.55f))   // top left 
};

ZSVERTEX cube_vertices[] = {
        // positions          // normals           // texture coords
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)),

        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)),

        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f,  0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f, -0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f,  0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f,  0.5f), ZSVECTOR2(-1.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),

        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f, -0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f, -0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f, -0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f,  0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(1.0f,  0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),

        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f,  0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f, -0.5f, -0.5f), ZSVECTOR2(0.0f, -1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),

        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  1.0f,  1.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f,  0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  0.0f,  0.0f)),
        ZSVERTEX(ZSVECTOR3(-0.5f,  0.5f, -0.5f), ZSVECTOR2(0.0f,  1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f))
};

unsigned int plane_inds[] = { 0,1,2, 0,2,3 };
unsigned int isotile_ind[] = { 0,1,2, 2,3,4, 2,4,5, 2,5,0 };

ZSPIRE::Mesh plane2Dmesh;
ZSPIRE::Mesh uiSprite2Dmesh;
ZSPIRE::Mesh iso_tile2Dmesh;
ZSPIRE::Mesh cube3Dmesh;

unsigned int vert_size = sizeof(ZSVERTEX);
unsigned int offset = 0;

Assimp::Importer importer;

unsigned int loadflags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs;

unsigned int processed_meshes = 0;
ZSPIRE::Mesh result;

ZSPIRE::Mesh::Mesh() {
	this->alive = false;

}

void ZSPIRE::Mesh::Init() {
    glGenVertexArrays(1, &this->meshVAO);
    glGenBuffers(1, &this->meshVBO);
    glGenBuffers(1, &this->meshEBO);
}

void ZSPIRE::setupDefaultMeshes() {
    picked_mesh = 0x0;

    plane2Dmesh.Init(); //Initialize mesh for GL
    plane2Dmesh.setMeshData(plane_verts, plane_inds, 4, 6); //Send plane data

    uiSprite2Dmesh.Init();
    uiSprite2Dmesh.setMeshData(ui_sprite_vertices, plane_inds, 4, 6);

    iso_tile2Dmesh.Init();
    iso_tile2Dmesh.setMeshData(iso_tile_vertices, isotile_ind, 6, 12);

    cube3Dmesh.Init();
    cube3Dmesh.setMeshData(cube_vertices, 36);
}

void ZSPIRE::freeDefaultMeshes(){
    plane2Dmesh.Destroy();
    uiSprite2Dmesh.Destroy();
    iso_tile2Dmesh.Destroy();
    cube3Dmesh.Destroy();
}

ZSPIRE::Mesh* ZSPIRE::getPlaneMesh2D() {
	return &plane2Dmesh;
}

ZSPIRE::Mesh* ZSPIRE::getUiSpriteMesh2D() {
	return &uiSprite2Dmesh;
}

ZSPIRE::Mesh* ZSPIRE::getIsoTileMesh2D(){
	return &iso_tile2Dmesh;
}
ZSPIRE::Mesh* ZSPIRE::getCubeMesh3D(){
    return &cube3Dmesh;
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
    glBufferData(GL_ARRAY_BUFFER, (int)vertices_num * sizeof(ZSVERTEX), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)indices_num * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

	//Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)0);
    glEnableVertexAttribArray(0);
	//Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
	//Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

	//Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
	//Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

}
void ZSPIRE::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num) {
	this->vertices_num = vertices_num;
	this->indices_num = NO_INDICES;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(ZSMATRIX4x4), vertices, GL_STATIC_DRAW); //send vertices to buffer

	//Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)0);
    glEnableVertexAttribArray(0);
	//Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
	//Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

	//Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
	//Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), (void*)(sizeof(float) * 11));
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
        if(this->indices_num != NO_INDICES) //if object uses indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);
    }

    if (this->indices_num == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_LINE_LOOP, 0, this->vertices_num);
    }
    else {
        //Indexed draw
        glDrawElements(GL_LINE_LOOP, this->indices_num, GL_UNSIGNED_INT, 0);
    }
}


void ZSPIRE::Mesh::processMesh(aiMesh* mesh, const aiScene* scene) {
	unsigned int vertices = mesh->mNumVertices;
	unsigned int faces = mesh->mNumFaces;

	ZSVERTEX* vertices_arr = (ZSVERTEX*)malloc(sizeof(ZSVERTEX) * vertices);
	unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * faces * 3);

	for (unsigned int v = 0; v < vertices; v++) {
		aiVector3D vertex_pos = mesh->mVertices[v];
		aiVector3D vertex_normal = mesh->mNormals[v];
		aiVector3D vertex_tangent = mesh->mTangents[v];
		aiVector3D vertex_bitangent = mesh->mBitangents[v];

		float U = mesh->mTextureCoords[0][v].x;
		float V = mesh->mTextureCoords[0][v].y;

		vertices_arr[v] = ZSVERTEX(ZSVECTOR3(vertex_pos.x, vertex_pos.y, vertex_pos.z), ZSVECTOR2(U, V),
			ZSVECTOR3(vertex_normal.x, vertex_normal.y, vertex_normal.z), ZSVECTOR3(vertex_tangent.x, vertex_tangent.y, vertex_tangent.z),
			ZSVECTOR3(vertex_bitangent.x, vertex_bitangent.y, vertex_bitangent.z)
		);

		vNormalize(&vertices_arr[v].normal);

	}

	for (unsigned int i = 0; i < faces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices[i * 3 + j] = face.mIndices[j];
	}

    Init();
    setMeshData(vertices_arr, indices, vertices, faces * 3);
}

void processNode(aiNode* node, const aiScene* scene) {
	unsigned int child_nodes_amount = node->mNumChildren;
	unsigned int meshes_amount = node->mNumMeshes;

	//Iterate child nodes
	for (unsigned int i = 0; i < child_nodes_amount; i++) {
        processNode(node->mChildren[i], scene);
	}
	//Iterate meshes
	for (unsigned int i = 0; i < meshes_amount; i++) {
       // processMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

}



void ZSPIRE::Mesh::LoadMeshesFromFileASSIMP(const char* file_path) {
	std::cout << "ASSIMP: Loading mesh from file : " << file_path << std::endl;

	const aiScene* scene = importer.ReadFile(file_path, loadflags);

    processMesh(scene->mMeshes[0], scene);

}

void ZSPIRE::Mesh::DestroyResource() {
    (this)->Destroy(); //Destroy next to this pointer object
}
