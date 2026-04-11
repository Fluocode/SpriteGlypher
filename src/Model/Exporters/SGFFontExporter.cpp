#include "SGFFontExporter.h"

#include <QDir>
#include <QFileInfo>

#include "SGFFontExporters.h"
#include "SGFFontExporterTypes.h"
#include "../SGFDocument.h"
#include "../SGFExportSettings.h"

//
// Constants
//

QString SGFFontExporter::kExportFileTypeUnknown = QString("Unknown");
QString SGFFontExporter::kExportFileTypeBMFontAscii = QString("BMFont_Ascii");
QString SGFFontExporter::kExportFileTypeBMFontXml = QString("BMFont_Xml");
QString SGFFontExporter::kExportFileTypeBMFontBinary = QString("BMFont_Binary");
QString SGFFontExporter::kExportFileTypeBMFontJson = QString("BMFont_Json");


//
// Static Methods
//


QString SGFFontExporter::ExportFileTypeToUITitle(SGFExportFileType value)
{
    switch( value )
    {
        case ExportType_BMFont_Ascii: return QString("BMFont (Ascii)");
        case ExportType_BMFont_Xml: return QString("BMFont (XML)");
        case ExportType_BMFont_Binary: return QString("BMFont (Binary)");
        case ExportType_BMFont_Json: return QString("BMFont (JSON)");
        default: break;
    }

    return kExportFileTypeUnknown;
}


QString SGFFontExporter::ExportFileTypeToString(SGFExportFileType value)
{
    switch( value )
    {
        case ExportType_BMFont_Ascii: return kExportFileTypeBMFontAscii;
        case ExportType_BMFont_Xml: return kExportFileTypeBMFontXml;
        case ExportType_BMFont_Binary: return kExportFileTypeBMFontBinary;
        case ExportType_BMFont_Json: return kExportFileTypeBMFontJson;
        default: break;
    }

    return "Unknown";
}


SGFExportFileType SGFFontExporter::ExportFileTypeFromString(QString value)
{
    if ( value.compare(kExportFileTypeBMFontAscii, Qt::CaseInsensitive) == 0 ) {
        return ExportType_BMFont_Ascii;
    }
    if ( value.compare(kExportFileTypeBMFontXml, Qt::CaseInsensitive) == 0 ) {
        return ExportType_BMFont_Xml;
    }
    if ( value.compare(kExportFileTypeBMFontBinary, Qt::CaseInsensitive) == 0 ) {
        return ExportType_BMFont_Binary;
    }
    if ( value.compare(kExportFileTypeBMFontJson, Qt::CaseInsensitive) == 0 ) {
        return ExportType_BMFont_Json;
    }

    return ExportType_Unknown;
}


QVector<SGFExportFileType> SGFFontExporter::ExportFileTypeList()
{
    QVector<SGFExportFileType> list;
    list.append(ExportType_BMFont_Xml);
    list.append(ExportType_BMFont_Ascii);
    list.append(ExportType_BMFont_Binary);
    list.append(ExportType_BMFont_Json);
    return list;
}


SGFFontExporter::Ptr SGFFontExporter::FontExporterForSettings(const SGFExportSettings &settings)
{
    if ( settings.format == ExportType_BMFont_Ascii ) {
        return SGFFontExporter::Ptr(new SGFFontExporterBMFont());
    }
    if ( settings.format == ExportType_BMFont_Xml ) {
        return SGFFontExporter::Ptr(new SGFFontExporterBMFontXml());
    }
    if ( settings.format == ExportType_BMFont_Binary ) {
        return SGFFontExporter::Ptr(new SGFFontExporterBMFontBinary());
    }
    if ( settings.format == ExportType_BMFont_Json ) {
        return SGFFontExporter::Ptr(new SGFFontExporterBMFontJson());
    }

    return SGFFontExporter::Ptr(nullptr);
}


QString SGFFontExporter::normalizedExportBasePath(SGFDocument *doc, const QString &settingsPath)
{
    if ( settingsPath.isNull() ) {
        return QString();
    }

    QFileInfo pathInfo = QFileInfo(settingsPath);
    QString path;

    if ( pathInfo.isAbsolute() ) {
        path = settingsPath;
    } else {
        QString rootPath = doc->documentPath();
        if ( rootPath.isNull() ) {
            rootPath = QDir::homePath();
        }
        QFileInfo rootInfo = QFileInfo(rootPath);
        QDir rootDir = (rootInfo.isDir()) ? QDir(rootPath) : QFileInfo(rootPath).dir();
        path = rootDir.absolutePath() + "/" + settingsPath;
    }

    if ( path.endsWith(QStringLiteral(".fnt"), Qt::CaseInsensitive) ) {
        path.chop(4);
    } else if ( path.endsWith(QStringLiteral(".json"), Qt::CaseInsensitive) ) {
        path.chop(5);
    }

    return path;
}


//
// Object Lifecycle
//


SGFFontExporter::SGFFontExporter()
{

}


QString SGFFontExporter::fullPathFromExportPath(SGFDocument * doc, const QString &path)
{
    if ( path.isNull() ) {
        return QString();
    }

    QFileInfo pathInfo = QFileInfo(path);

    if ( pathInfo.isAbsolute() ) {
        return path;
    }

    // Relative path, first attempt to make it relative to the document path folder
    QString rootPath = doc->documentPath();

    if ( rootPath.isNull() ) {
        rootPath = QDir::homePath();
    }

    QFileInfo rootInfo = QFileInfo(rootPath);
    QDir rootDir = (rootInfo.isDir()) ? QDir(rootPath) : QFileInfo(rootPath).dir();
    QString finalPath = rootDir.absolutePath() + "/" + path;
    return finalPath;
}
