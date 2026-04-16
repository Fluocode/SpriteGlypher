#include "SGFOuterGlowEffect.h"

#include <cmath>

const QString SGFOuterGlowEffect::kColorKey = QString("glowColor");
const QString SGFOuterGlowEffect::kSizeKey = QString("glowSize");
const QString SGFOuterGlowEffect::kBlurKey = QString("glowBlur");

SGFOuterGlowEffect::SGFOuterGlowEffect()
{
    mEffectType = SGFEffectType::Effect_OuterGlow;
}

void SGFOuterGlowEffect::scaleEffect(float factor)
{
    SGFOuterGlowEffectSettings settings = getSettings();
    settings.size *= factor;
    settings.blur *= factor;
    setSettings(settings);
}

SGFPadding SGFOuterGlowEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    const SGFOuterGlowEffectSettings s = getSettings();
    const float pad = s.size + s.blur * 2.0f;
    return SGFPadding(pad, pad, pad, pad);
}

void SGFOuterGlowEffect::applyToGlyph(SGFGlyph& glyph)
{
    const SGFOuterGlowEffectSettings s = getSettings();

    QPainterPath path = glyph.path;
    if ( s.size > 0 ) {
        path = growPath(path, s.size);
    }

    // Render a solid mask into temp, blur it, tint, then composite into background.
    glyph.tempImage.fill(QColor(0,0,0,0));
    {
        QPainter p(&glyph.tempImage);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillPath(path, QColor(0,0,0,255));
        p.end();
    }

    QImage blurred = glyph.tempImage;
    const int blurPx = static_cast<int>(std::round(std::max(0.0f, s.blur)));
    if ( blurPx > 0 ) {
        blurred = blurImage(glyph.tempImage, blurPx, true);
    }

    glyph.tempImage.fill(s.color);
    {
        QPainter p(&glyph.tempImage);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawImage(0, 0, blurred);
        p.end();
    }

    {
        QPainter p(&glyph.backgroundImage);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(s.opacity);
        p.setCompositionMode(compositionForBlendMode(mBlendMode));
        p.drawImage(0, 0, glyph.tempImage);
        p.end();
    }
}

void SGFOuterGlowEffect::setDefaultParameters()
{
    setSettings(SGFOuterGlowEffectSettings());
}

SGFOuterGlowEffectSettings SGFOuterGlowEffect::getSettings()
{
    SGFOuterGlowEffectSettings s;
    s.enabled = mEnabled;
    s.blendMode = mBlendMode;
    s.opacity = mOpacity;
    s.color = mParameters[kColorKey].value<QColor>();
    s.size = mParameters[kSizeKey].value<float>();
    s.blur = mParameters[kBlurKey].value<float>();
    return s;
}

void SGFOuterGlowEffect::setSettings(SGFOuterGlowEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kSizeKey] = QVariant::fromValue<float>(settings.size);
    mParameters[kBlurKey] = QVariant::fromValue<float>(settings.blur);
}

bool SGFOuterGlowEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    const SGFOuterGlowEffectSettings s = getSettings();
    writer.writeAttribute(kColorKey, s.color.name(QColor::HexArgb));
    writer.writeAttribute(kSizeKey, QString::number(s.size));
    writer.writeAttribute(kBlurKey, QString::number(s.blur));
    return true;
}

void SGFOuterGlowEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFOuterGlowEffectSettings s = getSettings();
    const QString xmlColor = element.attribute(kColorKey);
    const QString xmlSize = element.attribute(kSizeKey);
    const QString xmlBlur = element.attribute(kBlurKey);
    if ( !xmlColor.isEmpty() ) { s.color = QColor(xmlColor); }
    if ( !xmlSize.isEmpty() ) { s.size = xmlSize.toFloat(); }
    if ( !xmlBlur.isEmpty() ) { s.blur = xmlBlur.toFloat(); }
    setSettings(s);
}

// Simple separable blur, copied from ShadowEffect (kept local so glow doesn't depend on shadow).
QImage SGFOuterGlowEffect::blurImage(const QImage& image, int radius, bool alphaOnly)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = image.rect().top();
    int r2 = image.rect().bottom();
    int c1 = image.rect().left();
    int c2 = image.rect().right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    if (alphaOnly)
        i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}

