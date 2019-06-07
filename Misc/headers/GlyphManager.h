#ifndef zs_glyphmanager
#define zs_glyphmanager

#include <map>
#include <string>

#include "../../Render/headers/zs-math.h"

#include "ft2build.h"
#include FT_FREETYPE_H

class GlyphManager;

class CharacterGlyph{
private:

public:
    ZSVECTOR2 glyph_size;
    ZSVECTOR2 glyph_bearing;
    ZSVECTOR2 glyph_advance;
    CharacterGlyph();
};

class GlyphFontContainer{
private:
    FT_Face font;
    std::map<CharacterGlyph*, unsigned int> characters;


public:
    GlyphFontContainer(std::string path, unsigned int size, GlyphManager* manager);
    void loadGlyphs();
    void loadGlyph(unsigned int index);
};

class GlyphManager{
private:
    FT_Library ftlib;
public:
    FT_Library getFreetypeLibraryInstance();
    GlyphManager();
};

#endif
