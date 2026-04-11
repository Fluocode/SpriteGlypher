#include "SGFFontExporterBMFont.h"

#include "SGFFontBMFontExportData.h"
#include "SGFFontExporter.h"
#include "../SGFDocument.h"
#include "../SGFExportSettings.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

SGFFontExporterBMFont::SGFFontExporterBMFont()
{

}


bool SGFFontExporterBMFont::exportFont(SGFDocument *doc)
{
    if ( doc == nullptr || doc->getSpriteFont().isNull ) {
        mLastError = "Null document or document with null sprite font provided for exporting.";
        return false;
    }

    QString path = SGFFontExporter::normalizedExportBasePath(doc, doc->getExportSettings().path);
    if ( path.isNull() ) {
        mLastError = "Invalid path provided.";
        return false;
    }

    bool result = writeDocumentToPath(doc, path);

    if ( !result ) {
        return result;
    }

    if ( doc->getExportSettings().retinaCopy )
    {
        SGFDocument::Ptr retinaDoc = doc->clone();
        retinaDoc->scale(2.0f);
        retinaDoc->generateSpriteFont();

        result = writeDocumentToPath(retinaDoc.get(), path + QStringLiteral("@2x"));
    }

    return result;
}


bool SGFFontExporterBMFont::writeDocumentToPath(SGFDocument *doc, const QString &path)
{
    if ( path.isNull() ) {
        mLastError = "Invalid path provided.";
        return false;
    }

    QString fntPath = path;
    if ( fntPath.right(4).compare(QStringLiteral(".fnt"), Qt::CaseInsensitive) != 0 ) {
        fntPath.append(QStringLiteral(".fnt"));
    }

    QString pngPath = fntPath;
    pngPath.replace(QStringLiteral(".fnt"), QStringLiteral(".png"), Qt::CaseInsensitive);

    SGFBMFontExportData data;
    if ( !SGFFontBMFontExport::buildExportData(doc, data) ) {
        mLastError = "Failed to build export data.";
        return false;
    }

    data.page0FileName = QFileInfo(pngPath).fileName();

    QString padding = QStringLiteral("%1,%2,%3,%4")
            .arg(data.paddingUp)
            .arg(data.paddingRight)
            .arg(data.paddingDown)
            .arg(data.paddingLeft);
    QString spacing = QStringLiteral("%1,%2").arg(data.spacingHoriz).arg(data.spacingVert);

    QStringList lines;

    QStringList info;
    info << QStringLiteral("info");
    info << QStringLiteral("face=\"%1\"").arg(data.face);
    info << QStringLiteral("size=%1").arg(data.fontSize);
    info << QStringLiteral("bold=%1").arg(data.bold ? 1 : 0);
    info << QStringLiteral("italic=%1").arg(data.italic ? 1 : 0);
    info << QStringLiteral("charset=\"%1\"").arg(data.charset);
    info << QStringLiteral("unicode=%1").arg(data.unicode ? 1 : 0);
    info << QStringLiteral("stretchH=%1").arg(data.stretchH);
    info << QStringLiteral("smooth=%1").arg(data.smooth);
    info << QStringLiteral("aa=%1").arg(data.aa);
    info << QStringLiteral("padding=%1").arg(padding);
    info << QStringLiteral("spacing=%1").arg(spacing);
    info << QStringLiteral("outline=%1").arg(data.outline);
    lines << info.join(QLatin1Char(' '));

    QStringList common;
    common << QStringLiteral("common");
    common << QStringLiteral("lineHeight=%1").arg(data.lineHeight);
    common << QStringLiteral("base=%1").arg(data.base);
    common << QStringLiteral("scaleW=%1").arg(data.scaleW);
    common << QStringLiteral("scaleH=%1").arg(data.scaleH);
    common << QStringLiteral("pages=%1").arg(data.pages);
    common << QStringLiteral("packed=%1").arg(data.packed);
    common << QStringLiteral("alphaChnl=%1").arg(data.alphaChnl);
    common << QStringLiteral("redChnl=%1").arg(data.redChnl);
    common << QStringLiteral("greenChnl=%1").arg(data.greenChnl);
    common << QStringLiteral("blueChnl=%1").arg(data.blueChnl);
    lines << common.join(QLatin1Char(' '));

    QStringList page;
    page << QStringLiteral("page");
    page << QStringLiteral("id=%1").arg(0);
    page << QStringLiteral("file=\"%1\"").arg(data.page0FileName);
    lines << page.join(QLatin1Char(' '));

    QStringList charsHeader;
    charsHeader << QStringLiteral("chars");
    charsHeader << QStringLiteral("count=%1").arg(data.chars.size());
    lines << charsHeader.join(QLatin1Char(' '));

    for ( const SGFBMFontCharEntry &glyph : data.chars )
    {
        QStringList c;
        c << QStringLiteral("char");
        c << QStringLiteral("id=%1").arg(glyph.id);
        c << QStringLiteral("x=%1").arg(glyph.x);
        c << QStringLiteral("y=%1").arg(glyph.y);
        c << QStringLiteral("width=%1").arg(glyph.width);
        c << QStringLiteral("height=%1").arg(glyph.height);
        c << QStringLiteral("xoffset=%1").arg(glyph.xoffset);
        c << QStringLiteral("yoffset=%1").arg(glyph.yoffset);
        c << QStringLiteral("xadvance=%1").arg(glyph.xadvance);
        c << QStringLiteral("page=%1").arg(glyph.page);
        c << QStringLiteral("chnl=%1").arg(glyph.chnl);
        lines << c.join(QLatin1Char(' '));
    }

    QStringList kerningLines;
    for ( const SGFBMFontKerningEntry &k : data.kernings )
    {
        QStringList ke;
        ke << QStringLiteral("kerning");
        ke << QStringLiteral("first=%1").arg(k.first);
        ke << QStringLiteral("second=%1").arg(k.second);
        ke << QStringLiteral("amount=%1").arg(k.amount);
        kerningLines << ke.join(QLatin1Char(' '));
    }

    lines << QStringLiteral("kernings count=%1").arg(kerningLines.count());
    lines.append(kerningLines);

    QFile file(fntPath);
    if ( !file.open(QIODevice::WriteOnly) ) {
        mLastError = "Could not open descriptor file for writing.";
        return false;
    }

    QTextStream textStream(&file);
    textStream << lines.join(QStringLiteral("\n"));
    file.close();

    bool pngOk = doc->getSpriteFont().textureAtlas.save(pngPath, "PNG");
    if ( !pngOk ) {
        mLastError = "Save failed.";
    }

    return pngOk;
}
