#ifndef SGFDOCUMENT_H
#define SGFDOCUMENT_H

#include <QObject>
#include "SGFTypes.h"
#include "SGFGlyph.h"
#include "SGFSpriteFont.h"
#include "Effects/Effects.h"
#include "SGFExportSettings.h"


struct SGFPrepareGlyphFunctor
{
    void operator()(SGFGlyph& glyph);
};


struct SGFComposeFinalGlyphFunctor
{
    void operator()(SGFGlyph& glyph);
};


struct SGFApplyEffectFunctor
{
    SGFApplyEffectFunctor(SGFEffect* effect) :
        mEffect(effect) { }

    void operator()(SGFGlyph& glyph);

    private:
        SGFEffect* mEffect;
};


class SGFDocument : public QObject
{
    friend class SGFFileWriter;
    friend class SGFFileReader;

    Q_OBJECT

public:
    typedef std::shared_ptr<SGFDocument> Ptr;


public:
    explicit SGFDocument(QObject *parent = 0);

    void initDefaultDocument();

    SGFDocument::Ptr clone();

    // Texture Atlas Generation
public:
    const SGFSpriteFont & getSpriteFont();

    SGFInputSettings getInputSettings();
    void setInputSettings(const SGFInputSettings &settings);

    SGFGenerationSettings getGenerationSettings();
    void setGenerationSettings(const SGFGenerationSettings &settings);

    SGFExportSettings getExportSettings();
    void setExportSettings(const SGFExportSettings &settings);

    int getEffectCount();
    SGFEffect::Ptr getEffectAtIndex(int index);
    void addEffect(SGFEffect::Ptr effect);
    void insertEffect(int index, SGFEffect::Ptr effect);
    /** Replace effect order (same count, typically a permutation). Paint order remains index 0 .. n-1. */
    void reorderEffects(const QVector<SGFEffect::Ptr> &effects);
    void removeEffect(SGFEffect::Ptr effect);
    void removeEffect(int index);

    /** Scale the font size and the relevant settings on each effect. */
    void scale(float scale);

    /** Does this sprite font have unsaved data that? */
    bool needsSaving();

    /** Force (re-)generation of the current sprite font, even if the current sprite font is not dirty. */
    void generateSpriteFont();

    /** Set document path. */
    void setDocumentPath(const QString &path);

    /** Get the current document's path. */
    QString documentPath();


protected:

    /** Calculate the required texture size for the given glyphs and the provided fixed width/height values. */
    QSize calcTextureSizeForGlyphs(QVector<SGFGlyph>& glyphs, int fixedWidth, int fixedHeight);

    /** Generate internal glyphs vector from the given settings. */
    bool generateGlyphs();

    /** Layout the current set of glyphs on a new atlas. Returns true if the glyphs fit, false otherwise. */
    bool layoutGlyphsOnAtlas();

    /** Called by file readers / writers to say that this document has been saved. */
    void setNeedsSavingToFalse();


private:

    /**
     * Do the work of laying out the glyphs. This is separate so we can share the routine between calcTextureSize and the layout function.
     * @return Did the glyphs fit onto atlas?
     */
    bool layoutGlyphs(SGFSpriteFont & spriteFont, const SGFGenerationSettings & settings, bool doPaint);


    // Signals
signals:
    void spriteFontUpdated(const SGFSpriteFont& spriteFont);


public slots:


    // Member Variables
private:
    SGFSpriteFont mSpriteFont;
    bool mIsDirty;
    bool mHasUnsavedData;
    QString mDocumentPath;

    SGFInputSettings mInputSettings;
    SGFGenerationSettings mGenerationSettings;
    SGFExportSettings mExportSettings;

    QVector<SGFEffect::Ptr> mEffects;
};

#endif // SGFDOCUMENT_H
