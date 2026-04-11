#ifndef SGFSPRITEFONT_H
#define SGFSPRITEFONT_H

#include <QVector>
#include <QImage>
#include "SGFGlyph.h"

struct SGFSpriteFont
{
    QVector<SGFGlyph> glyphs;
    QImage textureAtlas;
    bool doGlyphsFit;
    bool isNull;
    double generationTime;

    SGFSpriteFont() :
        textureAtlas(),
        doGlyphsFit(false),
        isNull(true)
    { }
};

#endif // SGFSPRITEFONT_H
