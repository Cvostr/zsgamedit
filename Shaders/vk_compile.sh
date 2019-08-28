
glslangValidator -V ./2d_tile/tile2d.vert
glslangValidator -V ./2d_tile/tile2d.frag

mv vert.spv ./2d_tile/tile2d.vert_vk
mv frag.spv ./2d_tile/tile2d.frag_vk
