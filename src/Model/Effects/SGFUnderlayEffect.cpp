#include "SGFUnderlayEffect.h"

#include <cmath>
#include <QtMath>

const QString SGFUnderlayEffect::kFillTypeKey = QString("underlayFillType");
const QString SGFUnderlayEffect::kColorKey = QString("underlayColor");
const QString SGFUnderlayEffect::kGradientKey = QString("underlayGradient");
const QString SGFUnderlayEffect::kAngleKey = QString("underlayAngle");
const QString SGFUnderlayEffect::kDistanceKey = QString("underlayDistance");
const QString SGFUnderlayEffect::kSizeKey = QString("underlaySize");

SGFUnderlayEffect::SGFUnderlayEffect()
{
    mEffectType = SGFEffectType::Effect_Underlay;
}

void SGFUnderlayEffect::scaleEffect(float factor)
{
    SGFUnderlayEffectSettings settings = getSettings();
    settings.distance *= factor;
    settings.size *= factor;
    // blur intentionally not present
    setSettings(settings);
}

SGFPadding SGFUnderlayEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    SGFUnderlayEffectSettings settings = getSettings();

    const float padding = settings.size; // no blur
    const float radians = qDegreesToRadians(settings.angle);
    const float dx = std::cos(radians) * settings.distance;
    const float dy = std::sin(radians) * settings.distance;

    // Same convention as ShadowEffect outer.
    return SGFPadding(-dy + padding, dx + padding, dy + padding, -dx + padding);
}

void SGFUnderlayEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFUnderlayEffectSettings settings = getSettings();
    QPainterPath path = glyph.path;

    if ( settings.size > 0 ) {
        path = growPath(path, settings.size);
    }

    if ( settings.distance != 0 ) {
        const float radians = qDegreesToRadians(settings.angle);
        const float dx = std::cos(radians) * settings.distance;
        const float dy = std::sin(radians) * settings.distance;
        path.translate(dx, dy);
    }

    QPainter painter(&glyph.backgroundImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(settings.opacity);
    painter.setCompositionMode(compositionForBlendMode(mBlendMode));

    if ( settings.fillType == SGFFillType::Fill_Gradient ) {
        settings.gradient.fillPath(painter, path);
    } else {
        painter.fillPath(path, settings.color);
    }
    painter.end();
}

void SGFUnderlayEffect::setDefaultParameters()
{
    setSettings(SGFUnderlayEffectSettings());
}

SGFUnderlayEffectSettings SGFUnderlayEffect::getSettings()
{
    SGFUnderlayEffectSettings settings;
    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;
    settings.fillType = mParameters[kFillTypeKey].value<SGFFillType>();
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.gradient = mParameters[kGradientKey].value<SGFGradient>();
    settings.angle = mParameters[kAngleKey].value<float>();
    settings.distance = mParameters[kDistanceKey].value<float>();
    settings.size = mParameters[kSizeKey].value<float>();
    return settings;
}

void SGFUnderlayEffect::setSettings(SGFUnderlayEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kFillTypeKey] = QVariant::fromValue<SGFFillType>(settings.fillType);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kGradientKey] = QVariant::fromValue<SGFGradient>(settings.gradient);
    mParameters[kAngleKey] = QVariant::fromValue<float>(settings.angle);
    mParameters[kDistanceKey] = QVariant::fromValue<float>(settings.distance);
    mParameters[kSizeKey] = QVariant::fromValue<float>(settings.size);
}

bool SGFUnderlayEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFUnderlayEffectSettings settings = getSettings();

    // Persist only Color/Gradient; pattern is not supported.
    SGFFillType ft = settings.fillType;
    if ( ft != SGFFillType::Fill_Gradient ) {
        ft = SGFFillType::Fill_Color;
    }

    writer.writeAttribute(kFillTypeKey, SGFEffectTypes::FillTypeToString(ft));
    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kGradientKey, settings.gradient.stringRepresentation());
    writer.writeAttribute(kAngleKey, QString::number(settings.angle));
    writer.writeAttribute(kDistanceKey, QString::number(settings.distance));
    writer.writeAttribute(kSizeKey, QString::number(settings.size));
    return true;
}

void SGFUnderlayEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFUnderlayEffectSettings settings = getSettings();

    const QString xmlFillType = element.attribute(kFillTypeKey);
    const QString xmlColor = element.attribute(kColorKey);
    const QString xmlGradient = element.attribute(kGradientKey);
    const QString xmlAngle = element.attribute(kAngleKey);
    const QString xmlDistance = element.attribute(kDistanceKey);
    const QString xmlSize = element.attribute(kSizeKey);

    if ( !xmlFillType.isEmpty() ) { settings.fillType = SGFEffectTypes::FillTypeFromString(xmlFillType); }
    if ( settings.fillType != SGFFillType::Fill_Gradient ) {
        settings.fillType = SGFFillType::Fill_Color;
    }
    if ( !xmlColor.isEmpty() ) { settings.color = QColor(xmlColor); }
    if ( !xmlGradient.isEmpty() ) { settings.gradient = SGFGradient(xmlGradient); }
    if ( !xmlAngle.isEmpty() ) { settings.angle = xmlAngle.toFloat(); }
    if ( !xmlDistance.isEmpty() ) { settings.distance = xmlDistance.toFloat(); }
    if ( !xmlSize.isEmpty() ) { settings.size = xmlSize.toFloat(); }

    setSettings(settings);
}

