#include "SGFSparkleOverlayEffect.h"

#include <QBuffer>
#include <QtMath>

const QString SGFSparkleOverlayEffect::kPatternElementKey = QString("SparklePattern");
const QString SGFSparkleOverlayEffect::kPatternScaleKey = QString("scale");

const QString SGFSparkleOverlayEffect::kOpacityKey2 = QString("sparkleOpacity");
const QString SGFSparkleOverlayEffect::kScaleKey = QString("sparkleScale");
const QString SGFSparkleOverlayEffect::kRotationKey = QString("sparkleRotation");
const QString SGFSparkleOverlayEffect::kOffsetXKey = QString("sparkleOffsetX");
const QString SGFSparkleOverlayEffect::kOffsetYKey = QString("sparkleOffsetY");

SGFSparkleOverlayEffect::SGFSparkleOverlayEffect()
{
    mEffectType = SGFEffectType::Effect_SparkleOverlay;
}

void SGFSparkleOverlayEffect::scaleEffect(float factor)
{
    SGFSparkleOverlayEffectSettings s = getSettings();
    s.offsetX *= factor;
    s.offsetY *= factor;
    setSettings(s);
}

SGFPadding SGFSparkleOverlayEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    // Overlay only, no extra padding needed beyond existing glyph size.
    return SGFPadding(0,0,0,0);
}

void SGFSparkleOverlayEffect::applyToGlyph(SGFGlyph& glyph)
{
    const SGFSparkleOverlayEffectSettings s = getSettings();
    if ( s.pattern.image.isNull() ) {
        return;
    }

    QImage pat = s.pattern.image;
    if ( pat.format() != QImage::Format_ARGB32 ) {
        pat = pat.convertToFormat(QImage::Format_ARGB32);
    }

    // Build a transformed brush for the pattern.
    QTransform xf;
    xf.translate(s.offsetX, s.offsetY);
    if ( s.rotation != 0.0f ) {
        xf.rotate(s.rotation);
    }
    const float sc = (s.scale <= 0.0f) ? 1.0f : s.scale;
    xf.scale(sc, sc);

    QBrush br(pat);
    br.setTransform(xf);

    QImage overlay = glyph.image.copy();
    overlay.fill(QColor(0,0,0,0));

    {
        QPainter p(&overlay);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(overlay.rect(), br);
        // Mask inside glyph only.
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawImage(0, 0, glyph.maskImage);
        p.end();
    }

    {
        QPainter p(&glyph.image);
        p.setOpacity(s.opacity);
        p.setCompositionMode(compositionForBlendMode(mBlendMode));
        p.drawImage(0, 0, overlay);
        p.end();
    }
}

void SGFSparkleOverlayEffect::setDefaultParameters()
{
    SGFSparkleOverlayEffectSettings s;
    s.blendMode = SGFBlendMode::Blend_Overlay;
    s.scale = 0.10f;
    QImage img(QStringLiteral(":/sparks.png"));
    if ( !img.isNull() ) {
        s.pattern.image = img;
        s.pattern.scale = 1.0f;
    }
    setSettings(s);
}

SGFSparkleOverlayEffectSettings SGFSparkleOverlayEffect::getSettings()
{
    SGFSparkleOverlayEffectSettings s;
    s.enabled = mEnabled;
    s.blendMode = mBlendMode;
    s.opacity = mOpacity;
    s.pattern = mParameters[kPatternElementKey].value<SGFPattern>();
    s.scale = mParameters[kScaleKey].value<float>();
    s.rotation = mParameters[kRotationKey].value<float>();
    s.offsetX = mParameters[kOffsetXKey].value<float>();
    s.offsetY = mParameters[kOffsetYKey].value<float>();
    return s;
}

void SGFSparkleOverlayEffect::setSettings(SGFSparkleOverlayEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;
    mParameters[kPatternElementKey] = QVariant::fromValue<SGFPattern>(settings.pattern);
    mParameters[kScaleKey] = QVariant::fromValue<float>(settings.scale);
    mParameters[kRotationKey] = QVariant::fromValue<float>(settings.rotation);
    mParameters[kOffsetXKey] = QVariant::fromValue<float>(settings.offsetX);
    mParameters[kOffsetYKey] = QVariant::fromValue<float>(settings.offsetY);
}

bool SGFSparkleOverlayEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    const SGFSparkleOverlayEffectSettings s = getSettings();
    writer.writeAttribute(kOpacityKey2, QString::number(s.opacity));
    writer.writeAttribute(kScaleKey, QString::number(s.scale));
    writer.writeAttribute(kRotationKey, QString::number(s.rotation));
    writer.writeAttribute(kOffsetXKey, QString::number(s.offsetX));
    writer.writeAttribute(kOffsetYKey, QString::number(s.offsetY));

    if ( !s.pattern.image.isNull() )
    {
        QByteArray imgData;
        QBuffer buffer(&imgData);
        buffer.open(QBuffer::WriteOnly);
        s.pattern.image.save(&buffer, "PNG");

        writer.writeStartElement(kPatternElementKey);
        writer.writeAttribute(kPatternScaleKey, QString::number(s.pattern.scale));
        writer.writeCDATA(imgData.toBase64());
        writer.writeEndElement();
    }

    return true;
}

void SGFSparkleOverlayEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFSparkleOverlayEffectSettings s = getSettings();

    const QString xmlOpacity = element.attribute(kOpacityKey2);
    const QString xmlScale = element.attribute(kScaleKey);
    const QString xmlRotation = element.attribute(kRotationKey);
    const QString xmlOffX = element.attribute(kOffsetXKey);
    const QString xmlOffY = element.attribute(kOffsetYKey);

    if ( !xmlOpacity.isEmpty() ) { s.opacity = xmlOpacity.toFloat(); }
    if ( !xmlScale.isEmpty() ) { s.scale = xmlScale.toFloat(); }
    if ( !xmlRotation.isEmpty() ) { s.rotation = xmlRotation.toFloat(); }
    if ( !xmlOffX.isEmpty() ) { s.offsetX = xmlOffX.toFloat(); }
    if ( !xmlOffY.isEmpty() ) { s.offsetY = xmlOffY.toFloat(); }

    QDomElement patNode = element.firstChildElement(kPatternElementKey);
    if ( !patNode.isNull() )
    {
        QByteArray base64Image = patNode.text().toUtf8();
        QByteArray imgData = QByteArray::fromBase64(base64Image);
        QImage image = QImage::fromData(imgData, "PNG");
        if ( !image.isNull() ) {
            s.pattern.image = image;
        }
        const QString xmlPatternScale = patNode.attribute(kPatternScaleKey);
        if ( !xmlPatternScale.isEmpty() ) {
            s.pattern.scale = xmlPatternScale.toFloat();
        }
    }

    setSettings(s);
}

