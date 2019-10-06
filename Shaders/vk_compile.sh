#build 2d tile shader
glslangValidator -V ./2d_tile/tile2d.vert
glslangValidator -V ./2d_tile/tile2d.frag

mv vert.spv ./2d_tile/tile2d.vert_vk
mv frag.spv ./2d_tile/tile2d.frag_vk
#build default 3d shader
glslangValidator -V ./3d/3d.vert
glslangValidator -V ./3d/3d.frag

mv vert.spv ./3d/3d.vert_vk
mv frag.spv ./3d/3d.frag_vk
#build default heightmap shader
glslangValidator -V ./heightmap/heightmap.vert
glslangValidator -V ./heightmap/heightmap.frag

mv vert.spv ./heightmap/heightmap.vert_vk
mv frag.spv ./heightmap/heightmap.frag_vk