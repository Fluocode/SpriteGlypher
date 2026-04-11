#include "SGFEffect.h"

#include "SGFFillEffect.h"
#include "SGFShadowEffect.h"
#include "SGFStrokeEffect.h"
#include "SGFShadedMaterialEffect.h"


const QString SGFEffect::kRootKey = QString("Effect");
const QString SGFEffect::kEffectTypeKey = QString("effectType");
const QString SGFEffect::kEnabledKey = QString("effectEnabled");
const QString SGFEffect::kBlendModeKey = QString("effectBlendMode");
const QString SGFEffect::kOpacityKey = QString("effectOpacity");


SGFEffect::Ptr SGFEffect::CreateEffectFromType(const SGFEffectType &effectType)
{
    switch(effectType)
    {
        case SGFEffectType::Effect_Fill: return SGFEffect::Ptr(new SGFFillEffect());
        case SGFEffectType::Effect_Shadow: return SGFEffect::Ptr(new SGFShadowEffect());
        case SGFEffectType::Effect_Stroke: return SGFEffect::Ptr(new SGFStrokeEffect());
        case SGFEffectType::Effect_ShadedMaterial: return SGFEffect::Ptr(new SGFShadedMaterialEffect());
        default: return SGFEffect::Ptr(nullptr);
    }
}


SGFEffect::Ptr SGFEffect::CreateEffectFromXmlNode(const QDomElement &element)
{
    QString xmlEffectType = element.attribute(kEffectTypeKey);
    SGFEffectType effectType = SGFEffectTypes::EffectTypeFromString(xmlEffectType);

    if ( effectType == SGFEffectType::Effect_Unknown ) {
        return SGFEffect::Ptr(nullptr);
    }

    SGFEffect::Ptr effect = CreateEffectFromType(effectType);

    if ( effect == nullptr ) {
        return effect;
    }

    effect->readFromXmlNode(element);
    return effect;
}


SGFEffect::SGFEffect() :
    mEnabled(true),
    mBlendMode(SGFBlendMode::Blend_Normal),
    mOpacity(1.0f),
    mInitialized(false),
    mEffectType(SGFEffectType::Effect_Unknown)
{

}


void SGFEffect::initialize()
{
    if ( mInitialized ) {
        return;
    }

    setDefaultParameters();

    mInitialized = true;
}


//
// Optional Virtual Method Stubs
//


void SGFEffect::willApplyEffectToGlyphs(const QVector<SGFGlyph> & /*glyphs*/)
{

}


void SGFEffect::didApplyEffectToGlyphs()
{

}


//
// XML Stream Manipulation
//

bool SGFEffect::writeToXmlStream(QXmlStreamWriter &writer)
{
    if ( writer.hasError() ) {
        return false;
    }

    writer.writeStartElement(kRootKey);
    writer.writeAttribute(kEffectTypeKey, SGFEffectTypes::EffectTypeToString(mEffectType));
    writer.writeAttribute(kEnabledKey, QString::number((mEnabled) ? 1 : 0));
    writer.writeAttribute(kBlendModeKey, SGFEffectTypes::BlendModeToString(mBlendMode));
    writer.writeAttribute(kOpacityKey, QString::number(mOpacity));

    writeSubclassToXmlStream(writer);

    writer.writeEndElement();   // </Root>
    return true;
}


void SGFEffect::readFromXmlNode(const QDomElement &element)
{
    initialize();

    QString xmlEnabled = element.attribute(kEnabledKey);
    QString xmlBlendMode = element.attribute(kBlendModeKey);
    QString xmlOpacity = element.attribute(kOpacityKey);

    mEnabled = (xmlEnabled.toInt() == 1);
    mBlendMode = SGFEffectTypes::BlendModeFromString(xmlBlendMode);
    mOpacity = xmlOpacity.toFloat();

    if ( mOpacity < 0 ) { mOpacity = 0.0f; }
    if ( mOpacity > 1.0f ) { mOpacity = 1.0f; }

    readSubclassFromXmlNode(element);
}


//
// Utility Methods
//


QString SGFEffect::getTitle()
{
    return SGFEffectTypes::EffectTypeToString(mEffectType);
}


QPixmap SGFEffect::getPreview(int width, int height, int pointSize)
{
    SGFGlyph glyph = SGFGlyph(QFont("Arial", pointSize), 'A');
    glyph.expandSizeForEffect(this);
    glyph.prepareImages();
    this->applyToGlyph(glyph);
    glyph.composeFinalImage();

    QPixmap icon = QPixmap(width, height);
    icon.fill(QColor(0,0,0,0));

    int x = (width * 0.5f) - (glyph.image.width() * 0.5f);
    int y = (height * 0.5f) - (glyph.image.height() * 0.5f);

    QPainter painter;
    painter.begin(&icon);
    painter.drawImage(x, y, glyph.image);
    painter.end();

    return icon;
}


//
// Parameters
//

QVariant SGFEffect::getParameter(const QString& key)
{
    if ( !mInitialized ) {
        initialize();
    }

    return mParameters[key];
}


void SGFEffect::setParameter(const QString& key, QVariant& value)
{
    if ( !mInitialized ) {
        initialize();
    }

   mParameters[key] = value;
   mHasUnsavedData = true;
}


SGFEffectType SGFEffect::getEffectType() const
{
    return mEffectType;
}


bool SGFEffect::needsSaving() const
{
    return mHasUnsavedData;
}


void SGFEffect::setEnabled(bool value)
{
    mEnabled = value;
    mHasUnsavedData = true;
}


bool SGFEffect::isEnabled() const
{
    return mEnabled;
}


void SGFEffect::setBlendMode(SGFBlendMode blendMode)
{
    mBlendMode = blendMode;
    mHasUnsavedData = true;
}


SGFBlendMode SGFEffect::blendMode() const
{
    return mBlendMode;
}


QPainterPath SGFEffect::shrinkPath(const QPainterPath &path, float amount)
{
    QPainterPathStroker stroker;
    stroker.setWidth(2 * amount);

    QPainterPath strokePath = stroker.createStroke(path);
    return path.subtracted(strokePath);
}


QPainterPath SGFEffect::growPath(const QPainterPath &path, float amount)
{
    QPainterPathStroker stroker;
    stroker.setWidth(2 * amount);

    QPainterPath strokePath = stroker.createStroke(path);
    return path.united(strokePath);
}


QPainter::CompositionMode SGFEffect::compositionForBlendMode(const SGFBlendMode& mode)
{
    switch(mode)
    {
    case SGFBlendMode::Blend_Unknown: return QPainter::CompositionMode_SourceOver;
    case SGFBlendMode::Blend_Normal: return QPainter::CompositionMode_SourceOver;
    case SGFBlendMode::Blend_Multiply: return QPainter::CompositionMode_Multiply;
    case SGFBlendMode::Blend_Screen: return QPainter::CompositionMode_Screen;
    case SGFBlendMode::Blend_Overlay: return QPainter::CompositionMode_Overlay;
    case SGFBlendMode::Blend_Lighten: return QPainter::CompositionMode_Lighten;
    case SGFBlendMode::Blend_Darken: return QPainter::CompositionMode_Darken;
    case SGFBlendMode::Blend_ColorDodge: return QPainter::CompositionMode_ColorDodge;
    case SGFBlendMode::Blend_ColorBurn: return QPainter::CompositionMode_ColorBurn;
    case SGFBlendMode::Blend_SoftLight: return QPainter::CompositionMode_SoftLight;
    case SGFBlendMode::Blend_HardLight: return QPainter::CompositionMode_HardLight;
    case SGFBlendMode::Blend_Difference: return QPainter::CompositionMode_Difference;
    case SGFBlendMode::Blend_Exclusion: return QPainter::CompositionMode_Exclusion;
    }

    return QPainter::CompositionMode_SourceOver;
}


void SGFEffect::setNeedsSavingToFalse()
{
    mHasUnsavedData = false;
}
