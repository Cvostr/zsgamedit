#include "headers/GlyphManager.h"

#include <GL/glew.h>
#include <iostream>

extern RenderPipeline* renderer;

GlyphManager::GlyphManager(){
    if (FT_Init_FreeType(&this->ftlib))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

CharacterGlyph::CharacterGlyph(){

}

FT_Library GlyphManager::getFreetypeLibraryInstance(){
    return this->ftlib;
}

void GlyphManager::addFontContainer(GlyphFontContainer* ptr){
    this->fonts.push_back(ptr);
}
GlyphFontContainer* GlyphManager::getFontContainer(std::string path){
    for(unsigned int i = 0; i < this->fonts.size(); i ++){
        if(this->fonts[i]->path.compare(path) == 0){
            return fonts[i];
        }
    }
    return nullptr;
}

GlyphFontContainer::GlyphFontContainer(std::string path, unsigned int size, GlyphManager* manager){
    std::cout << "FREETYPE: Loading font " << path << std::endl;

    manager_ptr = manager;

    for(unsigned int i = static_cast<unsigned int>(this->manager_ptr->project_struct_ptr->root_path.size() + 1); i < path.size() ; i ++){
        this->path.push_back(path[i]);
    }
    //Load font by freetype
    FT_New_Face(manager->getFreetypeLibraryInstance(), path.c_str(), 0, &this->font);
    FT_Set_Pixel_Sizes(this->font, 0, size);

}

void GlyphFontContainer::loadGlyphs(){
    //Common ASCII symbols
    for(unsigned int i = 0; i < 255; i ++){
        loadGlyph(i);
    }
    //Cyrillic Russian letters
    for(unsigned int i = 1040; i < 1103; i ++){
        loadGlyph(i);
    }
    //Additional Cyrillic Ukrainian letters
    loadGlyph(1168);
    loadGlyph(1169);
    loadGlyph(1031);
    loadGlyph(1111);
    loadGlyph(1028);
    loadGlyph(1108);
    loadGlyph(1110);
    loadGlyph(1030);
}

void GlyphFontContainer::loadGlyph(unsigned int index){
    //use freetype to load char
    FT_Load_Char(this->font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);
    //Allocating new Character Glyph
    CharacterGlyph* character = new CharacterGlyph;
    character->glyph_size.X = font->glyph->bitmap.width;
    character->glyph_size.Y = font->glyph->bitmap.rows;

    character->glyph_advance.X = font->glyph->advance.x;
    character->glyph_advance.Y = font->glyph->advance.y;

    character->glyph_bearing.X = font->glyph->bitmap_left;
    character->glyph_bearing.Y = font->glyph->bitmap_top;
    //Allocate image buffer
    character->texture_buffer = new unsigned char[font->glyph->bitmap.width * font->glyph->bitmap.rows];
    //Copy Image data
    for(unsigned int i = 0; i < font->glyph->bitmap.width * font->glyph->bitmap.rows; i ++){
        character->texture_buffer[i] = font->glyph->bitmap.buffer[i];
    }

    glGenTextures(1, &character->gl_texture_id);
    glBindTexture(GL_TEXTURE_2D, character->gl_texture_id);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            static_cast<int>(font->glyph->bitmap.width),
            static_cast<int>(font->glyph->bitmap.rows),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font->glyph->bitmap.buffer
        );
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->characters.insert(std::pair<unsigned int, CharacterGlyph*>(index, character));
}

void GlyphFontContainer::DrawChar(int _char, ZSVECTOR2 pos, unsigned int* char_length, ZSRGBCOLOR color){
    CharacterGlyph* glyph = this->characters.at(static_cast<unsigned int>(_char));
    *char_length = static_cast<unsigned int>(glyph->glyph_bearing.X + glyph->glyph_size.X);

    renderer->renderGlyph(glyph->gl_texture_id, static_cast<int>(pos.X), static_cast<int>(pos.Y - (glyph->glyph_size.Y - glyph->glyph_bearing.Y)), static_cast<int>(glyph->glyph_size.X), static_cast<int>(glyph->glyph_size.Y), color);
}
void GlyphFontContainer::DrawString(int* string, unsigned int len, ZSVECTOR2 pos, ZSRGBCOLOR color){
    unsigned int xpos_offset = static_cast<unsigned int>(pos.X);
    for(unsigned int i = 0; i < len; i ++){
        unsigned int char_len = 0;
        DrawChar(string[i], ZSVECTOR2(xpos_offset, pos.Y), &char_len, color);
        xpos_offset += char_len;
    }
}
