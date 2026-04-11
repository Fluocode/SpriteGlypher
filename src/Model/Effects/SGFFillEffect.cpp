#include "SGFFillEffect.h"

#include <cmath>

const QString SGFFillEffect::kPatternElementKey = QString("FillPattern");

const QString SGFFillEffect::kFillTypeKey = QString("fillType");
const QString SGFFillEffect::kColorKey = QString("fillColor");
const QString SGFFillEffect::kGradientKey = QString("fillGradient");
const QString SGFFillEffect::kPatternKey = QString("fillPattern");
const QString SGFFillEffect::kPatternScaleKey = QString("scale");
const QString SGFFillEffect::kInsetKey = QString("fillInset");


SGFFillEffect::SGFFillEffect()
{
    mEffectType = SGFEffectType::Effect_Fill;
}


void SGFFillEffect::scaleEffect(float factor)
{
    SGFFillEffectSettings settings = getSettings();
    settings.inset *= factor;
    setSettings(settings);
}


SGFPadding SGFFillEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    return SGFPadding(0,0,0,0);
}


void SGFFillEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFFillEffectSettings settings = getSettings();
    QPainterPath path = glyph.path;

    if ( settings.inset > 0 )
    {
        path = shrinkPath(path, settings.inset);
    }

    QPainter painter;
    painter.begin(&(glyph.image));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(settings.opacity);
    painter.setCompositionMode(compositionForBlendMode(mBlendMode));

    if ( settings.fillType == SGFFillType::Fill_Color )
    {
        painter.fillPath(path, settings.color);
    }
    else if ( settings.fillType == SGFFillType::Fill_Gradient )
    {
        settings.gradient.fillPath(painter, path);
    }
    else if ( settings.fillType == SGFFillType::Fill_Pattern )
    {
        painter.fillPath(path, QBrush(mCachedPatternImage));
    }

    painter.end();
}


void SGFFillEffect::willApplyEffectToGlyphs(const QVector<SGFGlyph> &glyphs)
{
    SGFFillEffectSettings settings = getSettings();

    if ( settings.fillType == SGFFillType::Fill_Pattern && !settings.pattern.image.isNull() )
    {
        QSize maxSize = QSize(0,0);

        for( auto it = glyphs.constBegin(); it != glyphs.constEnd(); ++it )
        {
            const SGFGlyph & glyph = (*it);

            if ( glyph.minSize.width() > maxSize.width() ) {
                maxSize.setWidth(glyph.minSize.width());
            }

            if ( glyph.minSize.height() > maxSize.height() ) {
                maxSize.setHeight(glyph.minSize.height());
            }
        }

        mCachedPatternImage = settings.pattern.image.scaled(maxSize);
    }
}


void SGFFillEffect::didApplyEffectToGlyphs()
{
    mCachedPatternImage = QImage();
}


void SGFFillEffect::setDefaultParameters()
{
    setSettings(SGFFillEffectSettings());
}


SGFFillEffectSettings SGFFillEffect::getSettings()
{
    SGFFillEffectSettings settings;
    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;
    settings.fillType = mParameters[kFillTypeKey].value<SGFFillType>();
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.gradient = mParameters[kGradientKey].value<SGFGradient>();
    settings.pattern = mParameters[kPatternKey].value<SGFPattern>();
    settings.inset = mParameters[kInsetKey].toInt();

    return settings;
}


bool SGFFillEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFFillEffectSettings settings = getSettings();

    writer.writeAttribute(kFillTypeKey, SGFEffectTypes::FillTypeToString(settings.fillType));
    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kGradientKey, settings.gradient.stringRepresentation());
    writer.writeAttribute(kInsetKey, QString::number(settings.inset));

    if ( settings.fillType == Fill_Pattern )
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


void SGFFillEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFFillEffectSettings settings = getSettings();

    QString xmlFillType = element.attribute(kFillTypeKey);
    QString xmlColor = element.attribute(kColorKey);
    QString xmlGradient = element.attribute(kGradientKey);
    QString xmlInset = element.attribute(kInsetKey);

    if ( !xmlFillType.isEmpty() )   { settings.fillType = SGFEffectTypes::FillTypeFromString(xmlFillType); }
    if ( !xmlColor.isEmpty() )      { settings.color = QColor(xmlColor); }
    if ( !xmlGradient.isEmpty() )   { settings.gradient = SGFGradient(xmlGradient); }
    if ( !xmlInset.isEmpty() )      { settings.inset = xmlInset.toInt(); }

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


void SGFFillEffect::setSettings(SGFFillEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kFillTypeKey] = QVariant::fromValue<SGFFillType>(settings.fillType);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kGradientKey] = QVariant::fromValue<SGFGradient>(settings.gradient);
    mParameters[kPatternKey] = QVariant::fromValue<SGFPattern>(settings.pattern);
    mParameters[kInsetKey] = QVariant::fromValue<int>(settings.inset);
}

