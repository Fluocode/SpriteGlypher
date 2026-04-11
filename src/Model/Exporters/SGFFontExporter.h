#ifndef SGFFONTEXPORTER_H
#define SGFFONTEXPORTER_H

#include <QPair>
#include <string>
#include <memory>
#include "SGFFontExporterTypes.h"
#include "../SGFSpriteFont.h"


// Forward Declaration
class SGFDocument;
class SGFExportSettings;


/**
 * @brief An abstract base class for exporters that can save a sprite font.
 */
class SGFFontExporter
{
    // Typedefs
public:
    typedef std::shared_ptr<SGFFontExporter> Ptr;

    // Constants
public:
    static QString kExportFileTypeUnknown;
    static QString kExportFileTypeBMFontAscii;
    static QString kExportFileTypeBMFontXml;
    static QString kExportFileTypeBMFontBinary;
    static QString kExportFileTypeBMFontJson;

    // Static Methods
public:
    static QString ExportFileTypeToUITitle(SGFExportFileType value);
    static QString ExportFileTypeToString(SGFExportFileType value);
    static SGFExportFileType ExportFileTypeFromString(QString value);
    static QVector<SGFExportFileType> ExportFileTypeList();
    static SGFFontExporter::Ptr FontExporterForSettings(const SGFExportSettings &settings);
    /** Base path without trailing .fnt / .json — same rules as the reference bitmap-font tool. */
    static QString normalizedExportBasePath(SGFDocument *doc, const QString &settingsPath);

public:
    SGFFontExporter();

public:
    virtual bool exportFont(SGFDocument *doc) = 0;

    const std::string & getLastError() { return mLastError; }

    /** Given a path (which could be just a filename, a relative path or an absolute path) retrieve the desired save path. */
    QString fullPathFromExportPath(SGFDocument *doc, const QString & path);

protected:
    std::string mLastError;
};

#endif // SGFFONTEXPORTER_H
