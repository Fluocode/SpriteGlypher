#include "SGFSparkleOverlayEffect.h"

#include <QBuffer>
#include <QtMath>
#include <algorithm>

const QString SGFSparkleOverlayEffect::kPatternElementKey = QString("SparklePattern");
const QString SGFSparkleOverlayEffect::kPatternScaleKey = QString("scale");

const QString SGFSparkleOverlayEffect::kOpacityKey2 = QString("sparkleOpacity");
const QString SGFSparkleOverlayEffect::kScaleKey = QString("sparkleScale");
const QString SGFSparkleOverlayEffect::kRotationKey = QString("sparkleRotation");
const QString SGFSparkleOverlayEffect::kOffsetXKey = QString("sparkleOffsetX");
const QString SGFSparkleOverlayEffect::kOffsetYKey = QString("sparkleOffsetY");
const QString SGFSparkleOverlayEffect::kEdgeBleedKey = QString("edgeBleed");

namespace {

/** Expand opaque/ink regions in the alpha channel so overlays can extend past a hard glyph edge. */
QImage dilateAlpha(const QImage &src, int radius)
{
    if ( radius <= 0 || src.isNull() ) {
        return src;
    }

    QImage img = src.convertToFormat(QImage::Format_ARGB32);
    const int w = img.width();
    const int h = img.height();

    QVector<quint32> buf(w * h);
    QVector<quint32> tmp(w * h);

    for ( int y = 0; y < h; ++y ) {
        for ( int x = 0; x < w; ++x ) {
            buf[y * w + x] = static_cast<quint32>(qAlpha(img.pixel(x, y)));
        }
    }

    auto horizPass = [&](const QVector<quint32> &in, QVector<quint32> &out) {
        for ( int y = 0; y < h; ++y ) {
            for ( int x = 0; x < w; ++x ) {
                quint32 best = 0;
                for ( int dx = -radius; dx <= radius; ++dx ) {
                    const int sx = qBound(0, x + dx, w - 1);
                    best = std::max(best, in[y * w + sx]);
                }
                out[y * w + x] = best;
            }
        }
    };

    auto vertPass = [&](const QVector<quint32> &in, QVector<quint32> &out) {
        for ( int y = 0; y < h; ++y ) {
            for ( int x = 0; x < w; ++x ) {
                quint32 best = 0;
                for ( int dy = -radius; dy <= radius; ++dy ) {
                    const int sy = qBound(0, y + dy, h - 1);
                    best = std::max(best, in[sy * w + x]);
                }
                out[y * w + x] = best;
            }
        }
    };

    horizPass(buf, tmp);
    vertPass(tmp, buf);

    QImage outImg(w, h, QImage::Format_ARGB32);
    for ( int y = 0; y < h; ++y ) {
        for ( int x = 0; x < w; ++x ) {
            const int a = static_cast<int>(buf[y * w + x]);
            outImg.setPixel(x, y, qRgba(255, 255, 255, qBound(0, a, 255)));
        }
    }
    return outImg;
}

}

SGFSparkleOverlayEffect::SGFSparkleOverlayEffect()
{
    mEffectType = SGFEffectType::Effect_SparkleOverlay;
}

void SGFSparkleOverlayEffect::scaleEffect(float factor)
{
    SGFSparkleOverlayEffectSettings s = getSettings();
    s.offsetX *= factor;
    s.offsetY *= factor;
    s.edgeBleed *= factor;
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
        // Clip to glyph silhouette; dilate mask slightly so pattern features near the ink edge
        // are not harshly cut (sparkles can "float" a few px past the fill boundary).
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        const int bleed = static_cast<int>(std::round(std::max(0.0f, s.edgeBleed)));
        const QImage clipMask = dilateAlpha(glyph.maskImage, qBound(0, bleed, 48));
        p.drawImage(0, 0, clipMask);
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
    s.edgeBleed = 0.0f;
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
    s.edgeBleed = mParameters.value(kEdgeBleedKey, QVariant::fromValue(0.0f)).value<float>();
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
    mParameters[kEdgeBleedKey] = QVariant::fromValue<float>(settings.edgeBleed);
}

bool SGFSparkleOverlayEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    const SGFSparkleOverlayEffectSettings s = getSettings();
    writer.writeAttribute(kOpacityKey2, QString::number(s.opacity));
    writer.writeAttribute(kScaleKey, QString::number(s.scale));
    writer.writeAttribute(kRotationKey, QString::number(s.rotation));
    writer.writeAttribute(kOffsetXKey, QString::number(s.offsetX));
    writer.writeAttribute(kOffsetYKey, QString::number(s.offsetY));
    writer.writeAttribute(kEdgeBleedKey, QString::number(s.edgeBleed));

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
    const QString xmlBleed = element.attribute(kEdgeBleedKey);

    if ( !xmlOpacity.isEmpty() ) { s.opacity = xmlOpacity.toFloat(); }
    if ( !xmlScale.isEmpty() ) { s.scale = xmlScale.toFloat(); }
    if ( !xmlRotation.isEmpty() ) { s.rotation = xmlRotation.toFloat(); }
    if ( !xmlOffX.isEmpty() ) { s.offsetX = xmlOffX.toFloat(); }
    if ( !xmlOffY.isEmpty() ) { s.offsetY = xmlOffY.toFloat(); }
    if ( !xmlBleed.isEmpty() ) { s.edgeBleed = xmlBleed.toFloat(); }

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

