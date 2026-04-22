#ifndef SGFSPRITEFONT_H
#define SGFSPRITEFONT_H

#include <QVector>
#include <QImage>
#include "SGFGlyph.h"

struct SGFSpriteFont
{
    QVector<SGFGlyph> glyphs;
    QImage textureAtlas;
    QVector<QImage> textureAtlases; // Page 0..N-1, page0 mirrors textureAtlas.
    bool doGlyphsFit;
    bool isNull;
    double generationTime;

    SGFSpriteFont() :
        textureAtlas(),
        textureAtlases(),
        doGlyphsFit(false),
        isNull(true)
    { }
};

#endif // SGFSPRITEFONT_H
