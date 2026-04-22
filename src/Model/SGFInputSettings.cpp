#include "SGFInputSettings.h"


const QString SGFInputSettings::kCharacterSetNehe = QString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
const QString SGFInputSettings::kCharacterSetAscii = QString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ");

const QString SGFInputSettings::kRootKey = QString("InputSettings");
const QString SGFInputSettings::kFontFamilyKey = QString("fontFamily");
const QString SGFInputSettings::kFontStyleKey = QString("fontStyle");
const QString SGFInputSettings::kFontSizeKey = QString("fontSize");
const QString SGFInputSettings::kCharactersKey = QString("characters");
const QString SGFInputSettings::kInputSourceKey = QString("inputSource");
const QString SGFInputSettings::kFontFilePathKey = QString("fontFilePath");
const QString SGFInputSettings::kPngFaceNameKey = QString("pngFaceName");
const QString SGFInputSettings::kPngSlotKey = QString("PngSlot");
const QString SGFInputSettings::kPngSlotCharKey = QString("char");
const QString SGFInputSettings::kPngSlotPathKey = QString("path");


SGFInputSettings::SGFInputSettings() :
    pngFontFaceName(QStringLiteral("PNG Font")),
    fontFamily("Arial"),
    fontSize(64),
    characters(kCharacterSetNehe)
{

}


SGFInputSettings::SGFInputSettings(const QDomElement &element) :
    pngFontFaceName(QStringLiteral("PNG Font")),
    fontFamily("Arial"),
    fontSize(64),
    characters(kCharacterSetNehe)
{
    readFromXmlNode(element);
}


QVector<QChar> SGFInputSettings::uniqueCharacters()
{
    QVector<QChar> results;

    for( int i = 0, iSize = characters.size(); i < iSize; ++i )
    {
        QChar c = characters[i];

        if (results.contains(c) == false)
        {
            results.append(c);
        }
    }

    return results;
}


bool SGFInputSettings::writeToXmlStream(QXmlStreamWriter &writer)
{
    if ( writer.hasError() ) {
        return false;
    }

    writer.writeStartElement(kRootKey);
    writer.writeAttribute(kFontFamilyKey, fontFamily);
    writer.writeAttribute(kFontStyleKey, fontStyle);
    writer.writeAttribute(kFontSizeKey, QString::number(fontSize));
    writer.writeAttribute(kCharactersKey, characters);
    writer.writeAttribute(
        kInputSourceKey,
        inputSource == SGFInputSource::PngSprites
            ? QStringLiteral("png")
            : (inputSource == SGFInputSource::FontFile ? QStringLiteral("file") : QStringLiteral("font")));
    if ( inputSource == SGFInputSource::FontFile && !fontFilePath.isEmpty() ) {
        writer.writeAttribute(kFontFilePathKey, fontFilePath);
    }
    writer.writeAttribute(kPngFaceNameKey, pngFontFaceName);

    for ( const SGFPngGlyphSlot &slot : pngGlyphs ) {
        writer.writeEmptyElement(kPngSlotKey);
        writer.writeAttribute(kPngSlotCharKey, QString::number(static_cast<uint>(slot.character.unicode())));
        writer.writeAttribute(kPngSlotPathKey, slot.imagePath);
    }

    writer.writeEndElement();

    return true;
}


void SGFInputSettings::readFromXmlNode(const QDomElement &element)
{
    QString xmlFontFamily = element.attribute(kFontFamilyKey);
    QString xmlFontStyle = element.attribute(kFontStyleKey);
    QString xmlFontSize = element.attribute(kFontSizeKey);
    QString xmlCharacters = element.attribute(kCharactersKey);
    QString xmlFontFilePath = element.attribute(kFontFilePathKey);

    if ( !xmlFontFamily.isEmpty() ) {
        fontFamily = xmlFontFamily;
    }

    if ( !xmlFontStyle.isEmpty() ) {
        fontStyle = xmlFontStyle;
    }

    if ( !xmlFontSize.isEmpty() ) {
        fontSize = xmlFontSize.toFloat();
    }

    if ( !xmlCharacters.isEmpty() ) {
        characters = xmlCharacters;
    }

    inputSource = SGFInputSource::SystemFont;
    const QString srcAttr = element.attribute(kInputSourceKey);
    if ( srcAttr.compare(QLatin1String("png"), Qt::CaseInsensitive) == 0 ) {
        inputSource = SGFInputSource::PngSprites;
    } else if ( srcAttr.compare(QLatin1String("file"), Qt::CaseInsensitive) == 0 ) {
        inputSource = SGFInputSource::FontFile;
    }

    if ( !xmlFontFilePath.isEmpty() ) {
        fontFilePath = xmlFontFilePath;
    }

    const QString faceAttr = element.attribute(kPngFaceNameKey);
    if ( !faceAttr.isEmpty() ) {
        pngFontFaceName = faceAttr;
    }

    pngGlyphs.clear();
    for ( QDomNode node = element.firstChild(); !node.isNull(); node = node.nextSibling() ) {
        if ( !node.isElement() ) {
            continue;
        }
        QDomElement child = node.toElement();
        if ( child.tagName() != kPngSlotKey ) {
            continue;
        }
        SGFPngGlyphSlot slot;
        const QString uc = child.attribute(kPngSlotCharKey);
        if ( !uc.isEmpty() ) {
            slot.character = QChar(uc.toUShort());
        }
        slot.imagePath = child.attribute(kPngSlotPathKey);
        pngGlyphs.append(slot);
    }
}
