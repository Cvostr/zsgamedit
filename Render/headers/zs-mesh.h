#ifdef __linux__
#include <stdlib.h>
#endif

#include "zs-math.h"

#define NO_INDICES 0

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef zs_mesh
#define zs_mesh

namespace ZSPIRE {

	class Mesh {
	public:
		bool alive; //Non destroyed

		unsigned int meshVAO;
		unsigned int meshVBO;
		unsigned int meshEBO;

		unsigned int vertices_num;
		unsigned int indices_num;

        void Init();
        void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
        void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
        void Draw();
        void DrawLines();
        void Destroy();
        void DestroyResource();

        void LoadMeshesFromFileASSIMP(const char* file_path);
        void processMesh(aiMesh* mesh, const aiScene* scene);

		Mesh();
	};

    void setupDefaultMeshes();
    void freeDefaultMeshes();

	Mesh* getPlaneMesh2D();
	Mesh* getUiSpriteMesh2D();
	Mesh* getIsoTileMesh2D();
    Mesh* getCubeMesh3D();
   // Mesh LoadMeshesFromFile(const char* file_path);

}


#endif
