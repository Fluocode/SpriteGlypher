#include "SGFShadowEffect.h"

#include <QGraphicsDropShadowEffect>
#include <cmath>

const QString SGFShadowEffect::kColorKey = QString("shadowColor");
const QString SGFShadowEffect::kShadowTypeKey = QString("shadowType");
const QString SGFShadowEffect::kAngleKey = QString("shadowAngle");
const QString SGFShadowEffect::kDistanceKey = QString("shadowDistance");
const QString SGFShadowEffect::kBlurKey = QString("shadowBlur");
const QString SGFShadowEffect::kSizeKey = QString("shadowSize");


SGFShadowEffect::SGFShadowEffect()
{
    mEffectType = SGFEffectType::Effect_Shadow;
}


void SGFShadowEffect::scaleEffect(float factor)
{
    SGFShadowEffectSettings settings = getSettings();
    settings.distance *= factor;
    settings.blur *= factor;
    settings.size *= factor;
    setSettings(settings);
}


SGFPadding SGFShadowEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    SGFShadowEffectSettings settings = getSettings();

    if ( settings.shadowType == SGFShadowType::Shadow_Outer )
    {
        float padding = settings.size + settings.blur * 2;
        float radians = qDegreesToRadians(settings.angle);
        float dx = cos(radians) * settings.distance;
        float dy = sin(radians) * settings.distance;

        return SGFPadding(-dy+padding, dx+padding, dy+padding, -dx+padding);
    }

    return SGFPadding(0,0,0,0);
}


void SGFShadowEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFShadowEffectSettings settings = getSettings();
    QPainterPath path = glyph.path;

    if ( settings.shadowType == SGFShadowType::Shadow_Inner )
    {
        if ( settings.size > 0 ) {
            path = shrinkPath(path, settings.size);
        }

        if ( settings.distance > 0 ) {
            float radians = qDegreesToRadians(settings.angle);
            float dx = cos(radians) * settings.distance;
            float dy = sin(radians) * settings.distance;
            path.translate(dx, dy);
        }

        // Blur is much faster on just a black image as we can just blur on alpha.
        // So work with a black image and use DestinationIn to mask with the colour.
        glyph.tempImage.fill(QColor(0,0,0,255));

        QPainter painter(&(glyph.tempImage));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(QColor(0,0,0,255));
        painter.fillPath(path, settings.color);
        painter.end();        

        QImage blurred;

        if ( settings.blur > 0 ) {
            blurred = blurImage(glyph.tempImage, settings.blur, true);
        } else {
            blurred = glyph.tempImage.copy();
        }

        glyph.tempImage.fill(settings.color);

        painter.begin(&glyph.tempImage);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawImage(0, 0, blurred);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawImage(0, 0, glyph.maskImage);
        painter.end();

        painter.begin(&glyph.image);
        painter.setCompositionMode(compositionForBlendMode(mBlendMode));
        painter.setOpacity(settings.opacity);
        painter.drawImage(0, 0, glyph.tempImage);
        painter.end();
    }
    else
    {
        if ( settings.size > 0 ) {
            path = growPath(path, settings.size);
        }

        if ( settings.distance > 0 ) {
            float radians = qDegreesToRadians(settings.angle);
            float dx = cos(radians) * settings.distance;
            float dy = sin(radians) * settings.distance;
            path.translate(dx, dy);
        }

        glyph.tempImage.fill(QColor(0,0,0,0));

        // Blur is much faster on just a black image as we can just blur on alpha.
        // So work with a black image and use DestinationIn to mask with the colour.

        QPainter painter(&glyph.tempImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillPath(path, QColor(0,0,0,255));
        painter.end();

        QImage blurred;

        if ( settings.blur > 0 ) {
            blurred = blurImage(glyph.tempImage, settings.blur, true);
        } else {
            blurred = glyph.tempImage.copy();
        }

        glyph.tempImage.fill(settings.color);

        painter.begin(&glyph.tempImage);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawImage(0, 0, blurred);
        painter.end();

        painter.begin(&glyph.backgroundImage);
        painter.setCompositionMode(compositionForBlendMode(mBlendMode));
        painter.setOpacity(settings.opacity);
        painter.drawImage(0, 0, glyph.tempImage);
    }
}


void SGFShadowEffect::setDefaultParameters()
{
    setSettings(SGFShadowEffectSettings());
}


bool SGFShadowEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFShadowEffectSettings settings = getSettings();

    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kShadowTypeKey, SGFEffectTypes::ShadowTypeToString(settings.shadowType));
    writer.writeAttribute(kAngleKey, QString::number(settings.angle));
    writer.writeAttribute(kDistanceKey, QString::number(settings.distance));
    writer.writeAttribute(kBlurKey, QString::number(settings.blur));
    writer.writeAttribute(kSizeKey, QString::number(settings.size));

    return true;
}


void SGFShadowEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFShadowEffectSettings settings = getSettings();

    QString xmlColor = element.attribute(kColorKey);
    QString xmlShadowType = element.attribute(kShadowTypeKey);
    QString xmlAngle = element.attribute(kAngleKey);
    QString xmlDistance = element.attribute(kDistanceKey);
    QString xmlBlur = element.attribute(kBlurKey);
    QString xmlSize = element.attribute(kSizeKey);

    if ( !xmlColor.isEmpty() )      { settings.color = QColor(xmlColor); }
    if ( !xmlShadowType.isEmpty() ) { settings.shadowType = SGFEffectTypes::ShadowTypeFromString(xmlShadowType); }
    if ( !xmlAngle.isEmpty() )      { settings.angle = xmlAngle.toFloat(); }
    if ( !xmlDistance.isEmpty() )   { settings.distance = xmlDistance.toFloat(); }
    if ( !xmlBlur.isEmpty() )       { settings.blur = xmlBlur.toFloat(); }
    if ( !xmlSize.isEmpty() )       { settings.size = xmlSize.toFloat(); }

    setSettings(settings);
}


SGFShadowEffectSettings SGFShadowEffect::getSettings()
{
    SGFShadowEffectSettings settings;

    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.shadowType = mParameters[kShadowTypeKey].value<SGFShadowType>();
    settings.angle = mParameters[kAngleKey].value<float>();
    settings.distance = mParameters[kDistanceKey].value<float>();
    settings.blur = mParameters[kBlurKey].value<float>();
    settings.size = mParameters[kSizeKey].value<float>();

    return settings;
}


void SGFShadowEffect::setSettings(SGFShadowEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kShadowTypeKey] = QVariant::fromValue<SGFShadowType>(settings.shadowType);
    mParameters[kAngleKey] = QVariant::fromValue<float>(settings.angle);
    mParameters[kDistanceKey] = QVariant::fromValue<float>(settings.distance);
    mParameters[kBlurKey] = QVariant::fromValue<float>(settings.blur);
    mParameters[kSizeKey] = QVariant::fromValue<float>(settings.size);
}


// From: http://stackoverflow.com/questions/3903223/qt4-how-to-blur-qpixmap-image
// In turn from: http://qtplus.info/index.php/2011/10/gausssian-blur-effect-for-qimage/
QImage SGFShadowEffect::blurImage(const QImage& image, int radius, bool alphaOnly)
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


/*
QImage SGFShadowEffect::blurImage(const QImage& image, const QRect& rect, int radius, bool alphaOnly)
{
    QGraphicsBlurEffect blur;
    blur.setBlurRadius(radius);

    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

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
*/
