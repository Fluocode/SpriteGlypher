#include "SGFStrokeEffect.h"

#include <QBuffer>
#include <cmath>

const QString SGFStrokeEffect::kWidthKey = QString("strokeWidth");
const QString SGFStrokeEffect::kFillTypeKey = QString("strokeFillType");
const QString SGFStrokeEffect::kColorKey = QString("strokeColor");
const QString SGFStrokeEffect::kGradientKey = QString("strokeGradient");
const QString SGFStrokeEffect::kPatternElementKey = QString("StrokePattern");
const QString SGFStrokeEffect::kPatternScaleKey = QString("scale");
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
    // Always stroke with an opaque pen into a mask image (we fill with color/gradient/pattern after).
    QPen pen(QColor(0,0,0,255), settings.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setMiterLimit(5);
    QPainterPath path = glyph.path;

    auto fillStrokeMask = [&](QImage &dst, const QImage &mask)
    {
        QPainter p(&dst);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(settings.opacity);
        p.setCompositionMode(compositionForBlendMode(mBlendMode));

        if ( settings.fillType == SGFFillType::Fill_Gradient ) {
            QImage tmp = dst.copy();
            tmp.fill(QColor(0,0,0,0));
            {
                QPainter tp(&tmp);
                tp.setRenderHint(QPainter::Antialiasing);
                settings.gradient.fillRect(tp, QRectF(tmp.rect()));
                tp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                tp.drawImage(0, 0, mask);
                tp.end();
            }
            p.drawImage(0, 0, tmp);
        }
        else if ( settings.fillType == SGFFillType::Fill_Pattern && !settings.pattern.image.isNull() ) {
            QBrush br(settings.pattern.image);
            p.fillRect(dst.rect(), br);
            p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            p.drawImage(0, 0, mask);
        }
        else {
            // Default to solid color
            QImage tmp = dst.copy();
            tmp.fill(settings.color);
            QPainter tp(&tmp);
            tp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            tp.drawImage(0, 0, mask);
            tp.end();
            p.drawImage(0, 0, tmp);
        }

        p.end();
    };

    if ( settings.position == SGFStrokePosition::Stroke_Center )
    {
        glyph.tempImage.fill(QColor(0,0,0,0));
        {
            QPainter mp(&glyph.tempImage);
            mp.setRenderHint(QPainter::Antialiasing);
            mp.strokePath(path, pen);
            mp.end();
        }
        fillStrokeMask(glyph.image, glyph.tempImage);
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

        fillStrokeMask(glyph.image, glyph.tempImage);
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

        fillStrokeMask(glyph.image, glyph.tempImage);
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
    writer.writeAttribute(kFillTypeKey, SGFEffectTypes::FillTypeToString(settings.fillType));
    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kGradientKey, settings.gradient.stringRepresentation());
    writer.writeAttribute(kPositionKey, SGFEffectTypes::StrokePositionToString(settings.position));

    if ( settings.fillType == Fill_Pattern && !settings.pattern.image.isNull() )
    {
        QByteArray imgData;
        QBuffer buffer(&imgData);
        buffer.open(QBuffer::WriteOnly);
        settings.pattern.image.save(&buffer, "PNG");

        writer.writeStartElement(kPatternElementKey);
        writer.writeAttribute(kPatternScaleKey, QString::number(settings.pattern.scale));
        writer.writeCDATA(imgData.toBase64());
        writer.writeEndElement();
    }

    return true;
}


void SGFStrokeEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFStrokeEffectSettings settings = getSettings();

    QString xmlColor = element.attribute(kColorKey);
    QString xmlWidth = element.attribute(kWidthKey);
    QString xmlPosition = element.attribute(kPositionKey);
    QString xmlFillType = element.attribute(kFillTypeKey);
    QString xmlGradient = element.attribute(kGradientKey);

    if ( !xmlColor.isEmpty() )      { settings.color = QColor(xmlColor); }
    if ( !xmlWidth.isEmpty() )      { settings.width = xmlWidth.toFloat(); }
    if ( !xmlPosition.isEmpty() )   { settings.position = SGFEffectTypes::StrokePositionFromString(xmlPosition); }
    if ( !xmlFillType.isEmpty() )   { settings.fillType = SGFEffectTypes::FillTypeFromString(xmlFillType); }
    if ( !xmlGradient.isEmpty() )   { settings.gradient = SGFGradient(xmlGradient); }

    QDomElement patternNode = element.firstChildElement(kPatternElementKey);
    if ( !patternNode.isNull() )
    {
        QByteArray base64Image = patternNode.text().toUtf8();
        QByteArray imgData = QByteArray::fromBase64(base64Image);
        QImage image = QImage::fromData(imgData, "PNG");
        if ( !image.isNull() ) {
            settings.pattern.image = image;
        }
        QString xmlPatternScale = patternNode.attribute(kPatternScaleKey);
        if ( !xmlPatternScale.isEmpty() ) {
            settings.pattern.scale = xmlPatternScale.toFloat();
        }
    }

    setSettings(settings);
}


SGFStrokeEffectSettings SGFStrokeEffect::getSettings()
{
    SGFStrokeEffectSettings settings;

    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;

    settings.width = mParameters[kWidthKey].value<float>();
    settings.fillType = mParameters[kFillTypeKey].value<SGFFillType>();
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.gradient = mParameters[kGradientKey].value<SGFGradient>();
    settings.pattern = mParameters[kPatternElementKey].value<SGFPattern>();
    settings.position = mParameters[kPositionKey].value<SGFStrokePosition>();

    return settings;
}


void SGFStrokeEffect::setSettings(SGFStrokeEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kWidthKey] = QVariant::fromValue<float>(settings.width);
    mParameters[kFillTypeKey] = QVariant::fromValue<SGFFillType>(settings.fillType);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kGradientKey] = QVariant::fromValue<SGFGradient>(settings.gradient);
    mParameters[kPatternElementKey] = QVariant::fromValue<SGFPattern>(settings.pattern);
    mParameters[kPositionKey] = QVariant::fromValue<SGFStrokePosition>(settings.position);
}
