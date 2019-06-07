#include "headers/GlyphManager.h"

#include <iostream>

GlyphManager::GlyphManager(){
    if (FT_Init_FreeType(&this->ftlib))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

CharacterGlyph::CharacterGlyph(){

}

FT_Library GlyphManager::getFreetypeLibraryInstance(){
    return this->ftlib;
}

GlyphFontContainer::GlyphFontContainer(std::string path, unsigned int size, GlyphManager* manager){
    FT_New_Face(manager->getFreetypeLibraryInstance(), path.c_str(), 0, &this->font);
    FT_Set_Pixel_Sizes(this->font, 0, size);

}

void GlyphFontContainer::loadGlyphs(){
    for(unsigned int i = 0; i < 255; i ++){
        loadGlyph(i);
    }
}

void GlyphFontContainer::loadGlyph(unsigned int index){
    //use freetype to load char
    FT_Load_Char(this->font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);

    //std::cout << (index) << " " << font->glyph->bitmap.width << std::endl;

    CharacterGlyph* character = new CharacterGlyph;
    character->glyph_size.X = font->glyph->bitmap.width;
    character->glyph_size.Y = font->glyph->bitmap.rows;

    character->glyph_advance.X = font->glyph->advance.x;
    character->glyph_advance.Y = font->glyph->advance.y;

    character->glyph_bearing.X = font->glyph->bitmap_left;
    character->glyph_bearing.Y = font->glyph->bitmap_top;

    this->characters.insert(std::pair<CharacterGlyph*, unsigned int>(character, index));
}
