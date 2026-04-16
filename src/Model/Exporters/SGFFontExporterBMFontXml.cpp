#include "SGFFontExporterBMFontXml.h"

#include "SGFFontBMFontExportData.h"
#include "SGFFontExporter.h"
#include "../SGFDocument.h"
#include "../SGFExportSettings.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

SGFFontExporterBMFontXml::SGFFontExporterBMFontXml()
{
}

bool SGFFontExporterBMFontXml::exportFont(SGFDocument *doc)
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

    if ( result && doc->getExportSettings().retinaCopy )
    {
        SGFDocument::Ptr retinaDoc = doc->clone();
        retinaDoc->scale(2.0f);
        retinaDoc->generateSpriteFont();
        result = writeDocumentToPath(retinaDoc.get(), path + QStringLiteral("@2x"));
    }

    return result;
}

bool SGFFontExporterBMFontXml::writeDocumentToPath(SGFDocument *doc, const QString &path)
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

    QFile file(fntPath);
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
        mLastError = "Could not open descriptor file for writing.";
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("font"));
    // If this comment is missing after export, the .fnt was not written by this build (wrong path or old exe).
    writer.writeComment(QStringLiteral(" SpriteGlypher 1.0.3 bmfont-xml "));

    writer.writeStartElement(QStringLiteral("info"));
    writer.writeAttribute(QStringLiteral("face"), data.face);
    writer.writeAttribute(QStringLiteral("size"), QString::number(data.fontSize));
    writer.writeAttribute(QStringLiteral("bold"), QString::number(data.bold ? 1 : 0));
    writer.writeAttribute(QStringLiteral("italic"), QString::number(data.italic ? 1 : 0));
    writer.writeAttribute(QStringLiteral("charset"), data.charset);
    writer.writeAttribute(QStringLiteral("unicode"), QString::number(data.unicode ? 1 : 0));
    writer.writeAttribute(QStringLiteral("stretchH"), QString::number(data.stretchH));
    writer.writeAttribute(QStringLiteral("smooth"), QString::number(data.smooth));
    writer.writeAttribute(QStringLiteral("aa"), QString::number(data.aa));
    writer.writeAttribute(QStringLiteral("padding"), padding);
    writer.writeAttribute(QStringLiteral("spacing"), spacing);
    writer.writeAttribute(QStringLiteral("outline"), QString::number(data.outline));
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("common"));
    writer.writeAttribute(QStringLiteral("lineHeight"), QString::number(data.lineHeight));
    writer.writeAttribute(QStringLiteral("base"), QString::number(data.base));
    writer.writeAttribute(QStringLiteral("scaleW"), QString::number(data.scaleW));
    writer.writeAttribute(QStringLiteral("scaleH"), QString::number(data.scaleH));
    writer.writeAttribute(QStringLiteral("pages"), QString::number(data.pages));
    writer.writeAttribute(QStringLiteral("packed"), QString::number(data.packed));
    writer.writeAttribute(QStringLiteral("alphaChnl"), QString::number(data.alphaChnl));
    writer.writeAttribute(QStringLiteral("redChnl"), QString::number(data.redChnl));
    writer.writeAttribute(QStringLiteral("greenChnl"), QString::number(data.greenChnl));
    writer.writeAttribute(QStringLiteral("blueChnl"), QString::number(data.blueChnl));
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("pages"));
    writer.writeStartElement(QStringLiteral("page"));
    writer.writeAttribute(QStringLiteral("id"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("file"), data.page0FileName);
    writer.writeEndElement();
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("chars"));
    writer.writeAttribute(QStringLiteral("count"), QString::number(data.chars.size()));
    for ( const SGFBMFontCharEntry &ch : data.chars )
    {
        writer.writeEmptyElement(QStringLiteral("char"));
        writer.writeAttribute(QStringLiteral("id"), QString::number(ch.id));
        writer.writeAttribute(QStringLiteral("x"), QString::number(ch.x));
        writer.writeAttribute(QStringLiteral("y"), QString::number(ch.y));
        writer.writeAttribute(QStringLiteral("width"), QString::number(ch.width));
        writer.writeAttribute(QStringLiteral("height"), QString::number(ch.height));
        writer.writeAttribute(QStringLiteral("xoffset"), QString::number(ch.xoffset));
        writer.writeAttribute(QStringLiteral("yoffset"), QString::number(ch.yoffset));
        writer.writeAttribute(QStringLiteral("xadvance"), QString::number(ch.xadvance));
        writer.writeAttribute(QStringLiteral("page"), QString::number(ch.page));
        writer.writeAttribute(QStringLiteral("chnl"), QString::number(ch.chnl));
    }
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("kernings"));
    writer.writeAttribute(QStringLiteral("count"), QString::number(data.kernings.size()));
    for ( const SGFBMFontKerningEntry &k : data.kernings )
    {
        writer.writeEmptyElement(QStringLiteral("kerning"));
        writer.writeAttribute(QStringLiteral("first"), QString::number(k.first));
        writer.writeAttribute(QStringLiteral("second"), QString::number(k.second));
        writer.writeAttribute(QStringLiteral("amount"), QString::number(k.amount));
    }
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();

    bool pngOk = doc->getSpriteFont().textureAtlas.save(pngPath, "PNG");
    if ( !pngOk ) {
        mLastError = "Save failed.";
    }

    return pngOk;
}
