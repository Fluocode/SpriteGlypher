#ifndef SGFGENERATIONSETTINGS_H
#define SGFGENERATIONSETTINGS_H

#include <QtCore>
#include <QColor>

#include "SGFFile/SGFDocumentElement.h"

class SGFGenerationSettings : SGFDocumentElement
{
public:
    static const QString kRootKey;

private:
    static const QString kWidthKey;
    static const QString kHeightKey;
    static const QString kColorKey;
    static const QString kPaddingKey;
    static const QString kSpacingKey;
    static const QString kPaginateKey;

public:
    SGFGenerationSettings();
    SGFGenerationSettings(const QDomElement & element);

public:
    virtual bool writeToXmlStream(QXmlStreamWriter &writer);
    virtual void readFromXmlNode(const QDomElement &element);

public:
    int width;          //< -1 for auto, otherwise measured in pixels.
    int height;         //< -1 for auto, otherwise measured in pixels.
    QColor color;       //< Preview-only backdrop in the atlas view (contrast for strokes/shadows). Never baked into the exported PNG.
    int padding;        //< Pixel padding around the edge of the texture.
    int spacing;        //< Minimum pixel spacing between glyphs.
    bool paginate;      //< When true, spill into multiple atlas pages (exports multiple PNGs).

};

Q_DECLARE_METATYPE(SGFGenerationSettings)

#endif // SGFGENERATIONSETTINGS_H
