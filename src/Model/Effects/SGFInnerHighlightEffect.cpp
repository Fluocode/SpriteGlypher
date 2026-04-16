#include "SGFInnerHighlightEffect.h"

#include <QtMath>

const QString SGFInnerHighlightEffect::kFillTypeKey = QString("highlightFillType");
const QString SGFInnerHighlightEffect::kColorKey = QString("highlightColor");
const QString SGFInnerHighlightEffect::kGradientKey = QString("highlightGradient");
const QString SGFInnerHighlightEffect::kInsetKey = QString("highlightInset");
const QString SGFInnerHighlightEffect::kHeightFracKey = QString("highlightHeightFrac");
const QString SGFInnerHighlightEffect::kSoftenKey = QString("highlightSoften");

SGFInnerHighlightEffect::SGFInnerHighlightEffect()
{
    mEffectType = SGFEffectType::Effect_InnerHighlight;
}

void SGFInnerHighlightEffect::scaleEffect(float factor)
{
    SGFInnerHighlightEffectSettings s = getSettings();
    s.inset = qRound(static_cast<float>(s.inset) * factor);
    setSettings(s);
}

SGFPadding SGFInnerHighlightEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    return SGFPadding(0,0,0,0);
}

void SGFInnerHighlightEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFInnerHighlightEffectSettings s = getSettings();

    QPainterPath path = glyph.path;
    if ( s.inset > 0 ) {
        path = shrinkPath(path, static_cast<float>(s.inset));
    }

    // Build a soft top mask using a vertical gradient in tempImage.
    glyph.tempImage.fill(QColor(0,0,0,0));
    const QRectF bounds = path.boundingRect();
    if ( bounds.isEmpty() ) {
        return;
    }

    const float hf = qBound(0.0f, s.heightFrac, 1.0f);
    const float soften = qBound(0.0f, s.soften, 1.0f);
    const float h = static_cast<float>(bounds.height());
    const float cutoffY = static_cast<float>(bounds.top()) + h * hf;
    const float softenPx = h * hf * soften;

    QLinearGradient alphaGrad(bounds.left(), bounds.top(), bounds.left(), cutoffY + softenPx);
    alphaGrad.setColorAt(0.0, QColor(0,0,0,255));
    alphaGrad.setColorAt(1.0, QColor(0,0,0,0));

    {
        QPainter p(&glyph.tempImage);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillPath(path, alphaGrad); // alpha-only mask
        p.end();
    }

    // Color the highlight into temp, then apply alpha mask and glyph mask.
    QImage colored = glyph.image.copy();
    colored.fill(QColor(0,0,0,0));
    {
        QPainter p(&colored);
        p.setRenderHint(QPainter::Antialiasing);
        if ( s.fillType == SGFFillType::Fill_Gradient ) {
            s.gradient.fillPath(p, path);
        } else {
            p.fillPath(path, s.color);
        }
        p.end();
    }

    {
        QPainter p(&colored);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawImage(0, 0, glyph.tempImage);   // top mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawImage(0, 0, glyph.maskImage);   // inside glyph
        p.end();
    }

    {
        QPainter p(&glyph.image);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(s.opacity);
        p.setCompositionMode(compositionForBlendMode(mBlendMode));
        p.drawImage(0, 0, colored);
        p.end();
    }
}

void SGFInnerHighlightEffect::setDefaultParameters()
{
    setSettings(SGFInnerHighlightEffectSettings());
}

SGFInnerHighlightEffectSettings SGFInnerHighlightEffect::getSettings()
{
    SGFInnerHighlightEffectSettings s;
    s.enabled = mEnabled;
    s.blendMode = mBlendMode;
    s.opacity = mOpacity;
    s.fillType = mParameters[kFillTypeKey].value<SGFFillType>();
    s.color = mParameters[kColorKey].value<QColor>();
    s.gradient = mParameters[kGradientKey].value<SGFGradient>();
    s.inset = mParameters[kInsetKey].toInt();
    s.heightFrac = mParameters[kHeightFracKey].value<float>();
    s.soften = mParameters[kSoftenKey].value<float>();
    return s;
}

void SGFInnerHighlightEffect::setSettings(SGFInnerHighlightEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    // Only Color/Gradient supported.
    if ( settings.fillType != SGFFillType::Fill_Gradient ) {
        settings.fillType = SGFFillType::Fill_Color;
    }

    mParameters[kFillTypeKey] = QVariant::fromValue<SGFFillType>(settings.fillType);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kGradientKey] = QVariant::fromValue<SGFGradient>(settings.gradient);
    mParameters[kInsetKey] = QVariant::fromValue<int>(settings.inset);
    mParameters[kHeightFracKey] = QVariant::fromValue<float>(settings.heightFrac);
    mParameters[kSoftenKey] = QVariant::fromValue<float>(settings.soften);
}

bool SGFInnerHighlightEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFInnerHighlightEffectSettings s = getSettings();
    SGFFillType ft = s.fillType;
    if ( ft != SGFFillType::Fill_Gradient ) { ft = SGFFillType::Fill_Color; }
    writer.writeAttribute(kFillTypeKey, SGFEffectTypes::FillTypeToString(ft));
    writer.writeAttribute(kColorKey, s.color.name(QColor::HexArgb));
    writer.writeAttribute(kGradientKey, s.gradient.stringRepresentation());
    writer.writeAttribute(kInsetKey, QString::number(s.inset));
    writer.writeAttribute(kHeightFracKey, QString::number(s.heightFrac));
    writer.writeAttribute(kSoftenKey, QString::number(s.soften));
    return true;
}

void SGFInnerHighlightEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFInnerHighlightEffectSettings s = getSettings();
    const QString xmlFillType = element.attribute(kFillTypeKey);
    const QString xmlColor = element.attribute(kColorKey);
    const QString xmlGradient = element.attribute(kGradientKey);
    const QString xmlInset = element.attribute(kInsetKey);
    const QString xmlHeight = element.attribute(kHeightFracKey);
    const QString xmlSoften = element.attribute(kSoftenKey);

    if ( !xmlFillType.isEmpty() ) { s.fillType = SGFEffectTypes::FillTypeFromString(xmlFillType); }
    if ( s.fillType != SGFFillType::Fill_Gradient ) { s.fillType = SGFFillType::Fill_Color; }
    if ( !xmlColor.isEmpty() ) { s.color = QColor(xmlColor); }
    if ( !xmlGradient.isEmpty() ) { s.gradient = SGFGradient(xmlGradient); }
    if ( !xmlInset.isEmpty() ) { s.inset = xmlInset.toInt(); }
    if ( !xmlHeight.isEmpty() ) { s.heightFrac = xmlHeight.toFloat(); }
    if ( !xmlSoften.isEmpty() ) { s.soften = xmlSoften.toFloat(); }

    setSettings(s);
}

