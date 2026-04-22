#include "SGFDocument.h"

#include <algorithm>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QtCore>
#include <QtConcurrent>
#include <QtXml>
#include <cmath>
#include <QFontDatabase>

namespace {

QString resolveGlyphImagePath(SGFDocument *doc, const QString &path)
{
    if ( path.isEmpty() ) {
        return path;
    }
    if ( QFileInfo(path).isAbsolute() ) {
        return path;
    }
    const QString dp = doc->documentPath();
    if ( dp.isEmpty() ) {
        return path;
    }
    const QString candidate = QFileInfo(dp).dir().filePath(path);
    if ( QFile::exists(candidate) ) {
        return candidate;
    }
    return path;
}

}

//
// Prepare Glyph Operation is a Functor for easy multi-tasking
//

void SGFPrepareGlyphFunctor::operator()(SGFGlyph& glyph)
{
    glyph.prepareImages();
}


void SGFComposeFinalGlyphFunctor::operator ()(SGFGlyph& glyph)
{
    glyph.composeFinalImage();
}


void SGFApplyEffectFunctor::operator ()(SGFGlyph& glyph)
{
    if ( glyph.image.width() == 0 || glyph.image.height() == 0 ) {
        return;
    }

    mEffect->applyToGlyph(glyph);
}


SGFDocument::SGFDocument(QObject *parent) :
    QObject(parent)
    , mIsDirty(true)
    , mHasUnsavedData(false)
{

}


void SGFDocument::initDefaultDocument()
{
    SGFEffect::Ptr fill1 = SGFEffect::Ptr(new SGFFillEffect());
    addEffect(fill1);

    SGFShadowEffect::Ptr shadow1 = SGFShadowEffect::Ptr(new SGFShadowEffect());
    addEffect(shadow1);

    SGFEffect::Ptr stroke = SGFEffect::Ptr(new SGFStrokeEffect());
    addEffect(stroke);

    SGFEffect::Ptr shadedMaterial = SGFEffect::Ptr(new SGFShadedMaterialEffect());
    addEffect(shadedMaterial);
    shadedMaterial->setEnabled(false);
}


SGFDocument::Ptr SGFDocument::clone()
{
    SGFDocument::Ptr copy = SGFDocument::Ptr(new SGFDocument());
    copy->setInputSettings(getInputSettings());
    copy->setGenerationSettings(getGenerationSettings());
    copy->setExportSettings(getExportSettings());

    for( SGFEffect::Ptr effect : mEffects )
    {
        copy->addEffect(effect->clone());
    }

    return copy;
}


const SGFSpriteFont & SGFDocument::getSpriteFont()
{
    if ( mSpriteFont.isNull || mIsDirty ) {
        generateSpriteFont();
    }

    return mSpriteFont;
}


SGFInputSettings SGFDocument::getInputSettings()
{
    return mInputSettings;
}


void SGFDocument::setInputSettings(const SGFInputSettings& settings)
{
    mInputSettings = settings;
    mIsDirty = true;
    mHasUnsavedData = true;
}


SGFGenerationSettings SGFDocument::getGenerationSettings()
{
    return mGenerationSettings;
}


void SGFDocument::setGenerationSettings(const SGFGenerationSettings& settings)
{
    mGenerationSettings = settings;
    mIsDirty = true;
    mHasUnsavedData = true;
}


SGFExportSettings SGFDocument::getExportSettings()
{
    return mExportSettings;
}


void SGFDocument::setExportSettings(const SGFExportSettings &settings)
{
    mExportSettings = settings;
    //mIsDirty = true;          // Export settings don't affect the atlas
    mHasUnsavedData = true;
}


int SGFDocument::getEffectCount()
{
    return mEffects.count();
}


SGFEffect::Ptr SGFDocument::getEffectAtIndex(int index)
{
    return mEffects[index];
}


void SGFDocument::addEffect(SGFEffect::Ptr effect)
{
    effect->initialize();
    mEffects.push_back(effect);
    mIsDirty = true;
}


void SGFDocument::insertEffect(int index, SGFEffect::Ptr effect)
{
    effect->initialize();
    mEffects.insert(index, effect);
    mIsDirty = true;
    mHasUnsavedData = true;
}


void SGFDocument::reorderEffects(const QVector<SGFEffect::Ptr> &effects)
{
    if ( effects.size() != mEffects.size() ) {
        return;
    }
    mEffects = effects;
    mIsDirty = true;
    mHasUnsavedData = true;
}


void SGFDocument::removeEffect(int index)
{
    mEffects.removeAt(index);
    mIsDirty = true;
    mHasUnsavedData = true;
}


void SGFDocument::removeEffect(SGFEffect::Ptr effect)
{
    int count = mEffects.count();
    int index = -1;

    for( int i = 0; i < count; ++i ) {
        if ( mEffects[i].get() == effect.get() ) {
           index = i;
        }
    }

    if ( index >= 0 ) {
        removeEffect(index);
    }

    mHasUnsavedData = true;
}


void SGFDocument::scale(float scale)
{
    if ( mInputSettings.inputSource != SGFInputSource::PngSprites ) {
        mInputSettings.fontSize *= scale;
    }

    for( SGFEffect::Ptr effect : mEffects )
    {
        effect->scaleEffect(scale);
    }
    mIsDirty = true;
}


bool SGFDocument::needsSaving()
{
    return mHasUnsavedData;
}


void SGFDocument::generateSpriteFont()
{
    mSpriteFont = SGFSpriteFont();
    mSpriteFont.isNull = false;

    if ( !generateGlyphs() ) {
        mSpriteFont.isNull = true;
        return;
    }

    if ( !layoutGlyphsOnAtlas() ) {
        mSpriteFont.isNull = true;
        return;
    }

    mIsDirty = false;
    emit spriteFontUpdated(mSpriteFont);
}


void SGFDocument::setDocumentPath(const QString &path)
{
    mDocumentPath = path;
}


QString SGFDocument::documentPath()
{
    return mDocumentPath;
}


QSize SGFDocument::calcTextureSizeForGlyphs(QVector<SGFGlyph>& glyphs, int fixedWidth = 0, int fixedHeight = 0)
{
    Q_UNUSED(glyphs);
    // Special case - if no auto axis then we do nothing
    if ( fixedWidth > 0 && fixedHeight > 0 ) {
        return QSize(fixedWidth, fixedHeight);
    }

    // Just use a trial and error system for now, there are only a few options to try
    QSize atlasSize;
    atlasSize.setWidth((fixedWidth > 0) ? fixedWidth : 128);
    atlasSize.setHeight((fixedHeight > 0) ? fixedHeight : 128);

    int maxWidth = 4096;
    int maxHeight = 4096;

    // Must use a local copy of the input settings so we can change the size
    SGFGenerationSettings generationSettings = mGenerationSettings;

    bool didFit = false;

    while( didFit == false )
    {
        generationSettings.width = atlasSize.width();
        generationSettings.height = atlasSize.height();
        didFit = layoutGlyphs(mSpriteFont, generationSettings, false);

        if ( didFit == false )
        {
            if ( fixedWidth > 0 )
            {
                atlasSize.setHeight(atlasSize.height() * 2);
            }
            else if ( fixedHeight > 0 )
            {
                atlasSize.setWidth(atlasSize.width() * 2);
            }
            else if ( (atlasSize.width() < maxWidth) && (atlasSize.width() <= atlasSize.height()) )
            {
                atlasSize.setWidth(atlasSize.width() * 2);
            }
            else if ( (atlasSize.height() < maxHeight) && (atlasSize.height() <= atlasSize.height()) )
            {
                atlasSize.setHeight(atlasSize.height() * 2);
            }
            else
            {
                break;
            }
        }
    }

    return atlasSize;
}


bool SGFDocument::generateGlyphs()
{
    //qDebug() << "Generating Glyphs: " << QTime::currentTime();
    mSpriteFont.glyphs.clear();

    // Let's see how long this process takes!
    QElapsedTimer timer;
    timer.start();

    if ( mInputSettings.inputSource == SGFInputSource::PngSprites ) {
        for ( const SGFPngGlyphSlot &slot : mInputSettings.pngGlyphs ) {
            if ( slot.character.isNull() || slot.imagePath.isEmpty() ) {
                continue;
            }
            const QString loadPath = resolveGlyphImagePath(this, slot.imagePath);
            QImage img;
            if ( !img.load(loadPath) ) {
                return false;
            }
            SGFGlyph g;
            g.setImportedFromPng(slot.character, img);
            mSpriteFont.glyphs.push_back(g);
        }
        if ( mSpriteFont.glyphs.isEmpty() ) {
            return false;
        }
        mSpriteFont.generationTime = static_cast<double>(timer.nsecsElapsed()) / 1000000000.0;
        return true;
    }

    // Variables for the process
    QFont glyphFont;
    QString resolvedFamily = mInputSettings.fontFamily;

    // Font file mode: load the font into the application at runtime and use its family name.
    if ( mInputSettings.inputSource == SGFInputSource::FontFile )
    {
        const QString loadPath = resolveGlyphImagePath(this, mInputSettings.fontFilePath);
        const int fontId = QFontDatabase::addApplicationFont(loadPath);
        const QStringList fams = QFontDatabase::applicationFontFamilies(fontId);
        if ( !fams.isEmpty() ) {
            resolvedFamily = fams.first();
        }
    }

    if ( mInputSettings.fontStyle.isEmpty() )
    {
        glyphFont = QFont(resolvedFamily, mInputSettings.fontSize);
    }
    else
    {
        glyphFont = QFontDatabase::font(resolvedFamily, mInputSettings.fontStyle, mInputSettings.fontSize);
    }

    QVector<QChar> characters = mInputSettings.uniqueCharacters();

    for( int i = 0, iSize = characters.size(); i < iSize; ++i )
    {
        SGFGlyph glyph(glyphFont, characters[i]);
        mSpriteFont.glyphs.push_back(glyph);
    }

    // Calculate Glyph Size to fit all effects
    for( SGFGlyph & glyph : mSpriteFont.glyphs )
    {
        for( int j = 0; j < mEffects.size(); ++j )
        {
            if ( mEffects[j]->isEnabled() == false ) {
                continue;
            }

            glyph.expandSizeForEffect(mEffects[j].get());
        }
    }    

    // Prepare all glyphs concurrently
    SGFPrepareGlyphFunctor prepareGlyphFunctor;
    QtConcurrent::blockingMap(mSpriteFont.glyphs, prepareGlyphFunctor);

    // Apply Effects
    for( auto it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        SGFEffect::Ptr effect = (*it);

        if ( effect->isEnabled() == false ) {
            continue;
        }

        effect->willApplyEffectToGlyphs(mSpriteFont.glyphs);

        SGFApplyEffectFunctor applyEffect(effect.get());
        QtConcurrent::blockingMap(mSpriteFont.glyphs, applyEffect);

        effect->didApplyEffectToGlyphs();
    }

    // Compose from background and foreground layers
    SGFComposeFinalGlyphFunctor composeGlyphFunctor;
    QtConcurrent::blockingMap(mSpriteFont.glyphs, composeGlyphFunctor);

    // Calculate kerning info in a really dodgy way!
    QPainter painter;
    glyphFont.setKerning(true);
    glyphFont.setHintingPreference(QFont::PreferNoHinting);
    QFontInfo fontInfo = QFontInfo(glyphFont);
    QFontMetricsF fontMetrics = QFontMetricsF(glyphFont);
    int kerningCount = 0;

    for( SGFGlyph & glyph : mSpriteFont.glyphs )
    {
        for( QChar & c : mInputSettings.characters )
        {
            if ( glyph.character == c ) {
                continue;
            }

            QString charPair = QString(glyph.character) + QString(c);
            float expectedWidth = fontMetrics.horizontalAdvance(glyph.character) + fontMetrics.horizontalAdvance(c);
            float actualWidth = fontMetrics.horizontalAdvance(charPair);

            if ( fabsf(expectedWidth - actualWidth) > 1.0f ) {
                kerningCount++;
                SGFGlyph::KerningPair pair = SGFGlyph::KerningPair(c, (int)(actualWidth - expectedWidth));
                glyph.kerningPairs.append(pair);
            }
        }
    }

    //qDebug() << "Kerning Count: " << kerningCount;
    Q_UNUSED(kerningCount);

    mSpriteFont.generationTime = (double)timer.nsecsElapsed() / 1000000000.0;
    return true;
}


bool SGFDocument::layoutGlyphsOnAtlas()
{
    mSpriteFont.doGlyphsFit = layoutGlyphs(mSpriteFont, mGenerationSettings, true);
    return true;
}


void SGFDocument::setNeedsSavingToFalse()
{
    mHasUnsavedData = false;

    for( auto it = mEffects.begin(); it != mEffects.end(); ++it )
    {
        (*it)->setNeedsSavingToFalse();
    }
}


bool SGFDocument::layoutGlyphs(SGFSpriteFont & spriteFont, const SGFGenerationSettings & settings, bool doPaint)
{
    int padding = settings.padding;
    QSize atlasSize(settings.width, settings.height);
    QPainter painter;

    //QVector<SGFGlyph> sortedGlyphs(spriteFont.glyphs);
    std::sort(spriteFont.glyphs.begin(), spriteFont.glyphs.end(), [] (const SGFGlyph &g1, const SGFGlyph &g2) { return g1.minSize.height() > g2.minSize.height(); });

    if ( doPaint && (settings.width == 0 || settings.height == 0) )
    {
        atlasSize = calcTextureSizeForGlyphs(spriteFont.glyphs, settings.width, settings.height);
    }

    if ( doPaint )
    {
        spriteFont.textureAtlas = QImage(atlasSize.width(), atlasSize.height(), QImage::Format_ARGB32);
        // GenerationSettings::color is preview-only (MainWindow atlas view). Exported atlas is always
        // cleared transparent; glyph pixels bring their own alpha.
        spriteFont.textureAtlas.fill(Qt::transparent);

        painter.begin(&(spriteFont.textureAtlas));
    }

    QRect paddedRect = QRect(0, 0, atlasSize.width(), atlasSize.height()).adjusted(padding, padding, -padding, -padding);
    QPoint cursor = QPoint(paddedRect.x(),paddedRect.y());
    int rowHeight = 0;
    bool didFit = true;

    // Cursor starts top left and glyphs are printed "down,right" from the cursor point.

    for(auto it = spriteFont.glyphs.begin(); it != spriteFont.glyphs.end(); ++it)
    {
        SGFGlyph &glyph = (*it);

        if ( cursor.x() + glyph.image.width() > paddedRect.right() )
        {
            cursor.setY(cursor.y() + rowHeight + settings.spacing);
            cursor.setX(paddedRect.x());
            rowHeight = 0;
        }

        QRect destRect = QRect(cursor.x(), cursor.y(), glyph.image.width(), glyph.image.height());

        if ( doPaint )
        {
            painter.drawImage(destRect, glyph.image);
            glyph.atlasRect = destRect;
        }

        cursor.setX(cursor.x() + glyph.image.width() + settings.spacing);

        if ( glyph.image.height() > rowHeight ) {
            rowHeight = glyph.image.height();
        }
    }

    cursor.setY(cursor.y() + rowHeight);

    if ( cursor.y() > paddedRect.bottom() )
    {
        didFit = false;
    }

    if ( doPaint )
    {
        painter.end();
    }

    return didFit;
}
