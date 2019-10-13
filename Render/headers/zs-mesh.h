#include <render/zs-math.h>
#include "../../triggers.h"
#include "../../Misc/headers/zs_types.h"
#include <string>

#define NO_INDICES 0

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef zs_mesh
#define zs_mesh

namespace ZSPIRE {

    class Bone{
    public:
        std::string bone_name;
        ZSMATRIX4x4 offset;

        uint vertices_affected;

        Bone(std::string name, uint vertices);
        Bone(std::string name);
    };

	class Mesh {
	public:
		bool alive; //Non destroyed
        std::string mesh_label;

		unsigned int meshVAO;
		unsigned int meshVBO;
		unsigned int meshEBO;

		unsigned int vertices_num;
		unsigned int indices_num;

        ZSVERTEX* vertices_arr;
        unsigned int* indices_arr;

        std::vector<Bone> bones;
        bool hasBones();

        void Init();
        void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
        void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
        void setMeshOffsets();

        void Draw();
        void DrawLines();
        void Destroy();

		Mesh();
	};

    void setupDefaultMeshes();
    void freeDefaultMeshes();

	Mesh* getPlaneMesh2D();
	Mesh* getUiSpriteMesh2D();
	Mesh* getIsoTileMesh2D();
    Mesh* getCubeMesh3D();
    Mesh* getSphereMesh();
    Mesh* getSkyboxMesh();

    void processTangentSpace(ZSVERTEX* vert_array, unsigned int* indices_array, int indices_num);
    void processTangentSpace(ZSVERTEX* vert_array, int vertices_num);
}


#endif
