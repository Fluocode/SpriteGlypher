#include "SGFFontExporterBMFontBinary.h"

#include "SGFFontBMFontExportData.h"
#include "SGFFontExporter.h"
#include "../SGFDocument.h"
#include "../SGFExportSettings.h"

#include <QFile>
#include <QFileInfo>

namespace {

void appendU16(QByteArray &b, quint16 v)
{
    b.append(static_cast<char>(v & 0xff));
    b.append(static_cast<char>((v >> 8) & 0xff));
}

void appendU32(QByteArray &b, quint32 v)
{
    b.append(static_cast<char>(v & 0xff));
    b.append(static_cast<char>((v >> 8) & 0xff));
    b.append(static_cast<char>((v >> 16) & 0xff));
    b.append(static_cast<char>((v >> 24) & 0xff));
}

void appendS16(QByteArray &b, qint16 v)
{
    appendU16(b, static_cast<quint16>(v));
}

QByteArray packBlock(quint8 blockType, const QByteArray &payload)
{
    QByteArray block;
    block.append(static_cast<char>(blockType));
    quint32 sz = static_cast<quint32>(payload.size());
    appendU32(block, sz);
    block.append(payload);
    return block;
}

QByteArray buildInfoBlock(const SGFBMFontExportData &data)
{
    QByteArray p;
    appendU16(p, static_cast<quint16>(data.fontSize));

    quint8 bits = 0;
    if ( data.smooth ) {
        bits |= 1u << 0;
    }
    if ( data.unicode ) {
        bits |= 1u << 1;
    }
    if ( data.italic ) {
        bits |= 1u << 2;
    }
    if ( data.bold ) {
        bits |= 1u << 3;
    }
    p.append(static_cast<char>(bits));
    p.append(static_cast<char>(0));

    appendU16(p, static_cast<quint16>(data.stretchH));
    p.append(static_cast<char>(data.aa));
    p.append(static_cast<char>(data.paddingUp));
    p.append(static_cast<char>(data.paddingRight));
    p.append(static_cast<char>(data.paddingDown));
    p.append(static_cast<char>(data.paddingLeft));
    p.append(static_cast<char>(data.spacingHoriz));
    p.append(static_cast<char>(data.spacingVert));
    p.append(static_cast<char>(data.outline));

    QByteArray name = data.face.toUtf8();
    name.append(static_cast<char>(0));
    p.append(name);

    return packBlock(1, p);
}

QByteArray buildCommonBlock(const SGFBMFontExportData &data)
{
    QByteArray p;
    appendU16(p, static_cast<quint16>(data.lineHeight));
    appendU16(p, static_cast<quint16>(data.base));
    appendU16(p, static_cast<quint16>(data.scaleW));
    appendU16(p, static_cast<quint16>(data.scaleH));
    appendU16(p, static_cast<quint16>(data.pages));
    quint8 packedBit = data.packed ? static_cast<quint8>(0x80) : static_cast<quint8>(0);
    p.append(static_cast<char>(packedBit));
    p.append(static_cast<char>(data.alphaChnl));
    p.append(static_cast<char>(data.redChnl));
    p.append(static_cast<char>(data.greenChnl));
    p.append(static_cast<char>(data.blueChnl));

    return packBlock(2, p);
}

QByteArray buildPagesBlock(const SGFBMFontExportData &data)
{
    QByteArray p = data.page0FileName.toUtf8();
    p.append(static_cast<char>(0));
    return packBlock(3, p);
}

QByteArray buildCharsBlock(const SGFBMFontExportData &data)
{
    QByteArray p;
    for ( const SGFBMFontCharEntry &c : data.chars )
    {
        appendU32(p, c.id);
        appendU16(p, static_cast<quint16>(c.x));
        appendU16(p, static_cast<quint16>(c.y));
        appendU16(p, static_cast<quint16>(c.width));
        appendU16(p, static_cast<quint16>(c.height));
        appendS16(p, static_cast<qint16>(c.xoffset));
        appendS16(p, static_cast<qint16>(c.yoffset));
        appendS16(p, static_cast<qint16>(c.xadvance));
        p.append(static_cast<char>(c.page));
        p.append(static_cast<char>(c.chnl));
    }
    return packBlock(4, p);
}

QByteArray buildKerningsBlock(const SGFBMFontExportData &data)
{
    QByteArray p;
    for ( const SGFBMFontKerningEntry &k : data.kernings )
    {
        appendU32(p, k.first);
        appendU32(p, k.second);
        appendS16(p, k.amount);
    }
    return packBlock(5, p);
}

}

SGFFontExporterBMFontBinary::SGFFontExporterBMFontBinary()
{
}

bool SGFFontExporterBMFontBinary::exportFont(SGFDocument *doc)
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

bool SGFFontExporterBMFontBinary::writeDocumentToPath(SGFDocument *doc, const QString &path)
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

    QByteArray file;
    file.append("BMF");
    file.append(static_cast<char>(3));

    file.append(buildInfoBlock(data));
    file.append(buildCommonBlock(data));
    file.append(buildPagesBlock(data));
    file.append(buildCharsBlock(data));
    if ( !data.kernings.isEmpty() ) {
        file.append(buildKerningsBlock(data));
    }

    QFile out(fntPath);
    if ( !out.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
        mLastError = "Could not open descriptor file for writing.";
        return false;
    }
    if ( out.write(file) != file.size() ) {
        mLastError = "Write failed.";
        out.close();
        return false;
    }
    out.close();

    bool pngOk = doc->getSpriteFont().textureAtlas.save(pngPath, "PNG");
    if ( !pngOk ) {
        mLastError = "Save failed.";
    }

    return pngOk;
}
