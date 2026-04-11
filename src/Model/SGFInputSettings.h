#ifndef SGFINPUTSETTINGS_H
#define SGFINPUTSETTINGS_H

#include <QtCore>

#include "SGFFile/SGFDocumentElement.h"

enum class SGFInputSource
{
    SystemFont,
    PngSprites
};

struct SGFPngGlyphSlot
{
    QChar character;
    QString imagePath;
};

class SGFInputSettings : public SGFDocumentElement
{
public:
    static const QString kCharacterSetNehe;
    static const QString kCharacterSetAscii;

    static const QString kRootKey;

private:
    static const QString kFontFamilyKey;
    static const QString kFontStyleKey;
    static const QString kFontSizeKey;
    static const QString kCharactersKey;
    static const QString kInputSourceKey;
    static const QString kPngFaceNameKey;
    static const QString kPngSlotKey;
    static const QString kPngSlotCharKey;
    static const QString kPngSlotPathKey;

public:
    SGFInputSettings();
    SGFInputSettings(const QDomElement & element);

    QVector<QChar> uniqueCharacters();

public:
    virtual bool writeToXmlStream(QXmlStreamWriter &writer);
    virtual void readFromXmlNode(const QDomElement & element);

public:
    SGFInputSource inputSource = SGFInputSource::SystemFont;

    /** BMFont `face` when using PNG sprites. */
    QString pngFontFaceName;

    /** Row order → character + image path (transparency preserved). */
    QVector<SGFPngGlyphSlot> pngGlyphs;

    QString fontFamily;     //< Name of the font with which to generate the atlas.
    QString fontStyle;      //< Text name of the style (Bold, Italic, Black, Condensed, etc)
    float fontSize;         //< Size of the font to generate.
    QString characters;     //< Which characters will be included?
};

Q_DECLARE_METATYPE(SGFInputSettings)

#endif // SGFINPUTSETTINGS_H
