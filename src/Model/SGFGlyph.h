#ifndef SGFGLYPH_H
#define SGFGLYPH_H

#include <QChar>
#include <QFont>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QPair>
#include <QRect>
#include <QSize>
#include <QVector>

#include "SGFTypes.h"


// Forward Declaration
class SGFEffect;


class SGFGlyph
{
public:
    typedef QPair<QChar, int> KerningPair;

public:
    SGFGlyph();

    SGFGlyph(QFont aFont, QChar aChar);

public:
    void setCharacter(QFont aFont, QChar aChar);

    /** Raster glyph from PNG (or any QImage); skips vector path / compose pipeline. */
    void setImportedFromPng(QChar aChar, const QImage &source);

    void expandSizeForEffect(SGFEffect * effect);

    void prepareImages();

    void composeFinalImage();

public:
    QFont font;         //< Font
    QChar character;    //< Input value

    QPainterPath path;  //< Used during the generation process
    QRect minSize;      //< Used during the generation process
    QImage backgroundImage;     //< Used during the generation process to render background shadows
    QImage maskImage;           //< Original glyph painted black, to be used as an exclusion mask for some effects
    QImage tempImage;           //< Available for all effects to use as a buffer (so we don't have to keep creating/destroying images).
    QImage image;       //< Used during the generation process

    QSize baseSize;    //< Size of just the glyph, so we can later calculate difference between the
    SGFPadding effectPadding;   //< How much additional padding have we added on top of the original glyph?
    SGFPadding advancePadding;  //< Padding that should contribute to spacing (excludes pure shadows).
    QRect atlasRect;        //< Output from the generation process

    QVector<KerningPair> kerningPairs;  //< Vector of kerning pairs

private:
    bool mHasPreparedImages = false;    //< Has the prepareImages method been called?
    bool mImportedRaster = false;
};

#endif // SGFGLYPH_H
