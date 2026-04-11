#include "SGFStrokeEffect.h"

#include <cmath>


const QString SGFStrokeEffect::kWidthKey = QString("strokeWidth");
const QString SGFStrokeEffect::kColorKey = QString("strokeColor");
const QString SGFStrokeEffect::kPositionKey = QString("strokePosition");


SGFStrokeEffect::SGFStrokeEffect()
{
    mEffectType = SGFEffectType::Effect_Stroke;
}


void SGFStrokeEffect::scaleEffect(float factor)
{
    SGFStrokeEffectSettings settings = getSettings();
    settings.width *= factor;
    setSettings(settings);
}


SGFPadding SGFStrokeEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    SGFStrokeEffectSettings settings = getSettings();

    if ( settings.position == SGFStrokePosition::Stroke_Outside )
    {
        float padding = settings.width;
        return SGFPadding(padding, padding, padding, padding);
    }
    else if ( settings.position == SGFStrokePosition::Stroke_Center )
    {
        float padding = settings.width * 0.5f;
        return SGFPadding(padding, padding, padding, padding);
    }

    return SGFPadding(0,0,0,0);
}


void SGFStrokeEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFStrokeEffectSettings settings = getSettings();
    QPen pen(settings.color, settings.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setMiterLimit(5);
    QPainterPath path = glyph.path;

    if ( settings.position == SGFStrokePosition::Stroke_Center )
    {
        // For center stroke, simply render on the image and carry on
        QPainter painter(&(glyph.image));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(compositionForBlendMode(mBlendMode));
        painter.setOpacity(settings.opacity);
        painter.strokePath(path, pen);
        painter.end();
    }
    else if ( settings.position == SGFStrokePosition::Stroke_Inside )
    {
        // For inner stroke, render a 2x wide stroke, mask with the glyph path and then overlay on the current texture
        glyph.tempImage.fill(QColor(0,0,0,0));
        pen.setWidthF(settings.width * 2);

        QPainterPath rectPath;
        rectPath.addRect(glyph.image.rect());
        QPainterPath mask = rectPath.subtracted(glyph.path);

        // Generate Wide Stroke
        QPainter tempPainter(&glyph.tempImage);
        tempPainter.setRenderHint(QPainter::Antialiasing);
        tempPainter.strokePath(path, pen);

        // We're only interested in the parts overlapping with the original glyph
        tempPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        tempPainter.drawImage(0, 0, glyph.maskImage);

        // Composite with original glyph
        QPainter painter(&(glyph.image));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(compositionForBlendMode(mBlendMode));
        painter.setOpacity(settings.opacity);
        painter.drawImage(0, 0, glyph.tempImage);
    }
    else if ( settings.position == SGFStrokePosition::Stroke_Outside )
    {
        // For outer stroke, render a 2x wide stroke, delete the original glyph and then overlay UNDER the current texture
        pen.setWidthF(settings.width * 2);
        glyph.tempImage.fill(QColor(0,0,0,0));

        // Generate Wide Stroke
        QPainter tempPainter(&glyph.tempImage);
        tempPainter.setRenderHint(QPainter::Antialiasing);
        tempPainter.strokePath(path, pen);

        // Delete original Glyph from temp image
        tempPainter.setCompositionMode(QPainter::CompositionMode_Clear);
        tempPainter.fillPath(path, QColor(0,0,0,255));

        // Composite with original glyph
        QPainter painter(&(glyph.image));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(compositionForBlendMode(mBlendMode));
        painter.setOpacity(settings.opacity);
        painter.drawImage(0, 0, glyph.tempImage);
    }
}


void SGFStrokeEffect::setDefaultParameters()
{
    setSettings(SGFStrokeEffectSettings());
}


bool SGFStrokeEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFStrokeEffectSettings settings = getSettings();

    writer.writeAttribute(kWidthKey, QString::number(settings.width));
    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kPositionKey, SGFEffectTypes::StrokePositionToString(settings.position));

    return true;
}


void SGFStrokeEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFStrokeEffectSettings settings = getSettings();

    QString xmlColor = element.attribute(kColorKey);
    QString xmlWidth = element.attribute(kWidthKey);
    QString xmlPosition = element.attribute(kPositionKey);

    if ( !xmlColor.isEmpty() )      { settings.color = QColor(xmlColor); }
    if ( !xmlWidth.isEmpty() )      { settings.width = xmlWidth.toFloat(); }
    if ( !xmlPosition.isEmpty() )   { settings.position = SGFEffectTypes::StrokePositionFromString(xmlPosition); }

    setSettings(settings);
}


SGFStrokeEffectSettings SGFStrokeEffect::getSettings()
{
    SGFStrokeEffectSettings settings;

    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;

    settings.width = mParameters[kWidthKey].value<float>();
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.position = mParameters[kPositionKey].value<SGFStrokePosition>();

    return settings;
}


void SGFStrokeEffect::setSettings(SGFStrokeEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kWidthKey] = QVariant::fromValue<float>(settings.width);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kPositionKey] = QVariant::fromValue<SGFStrokePosition>(settings.position);
}
