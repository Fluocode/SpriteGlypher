#ifndef SGFFONTEXPORTERTYPES_H
#define SGFFONTEXPORTERTYPES_H

#include <QImage>
#include <QMetaType>
#include <QVector>

enum SGFExportFileType
{
    ExportType_Unknown,
    ExportType_BMFont_Ascii,
    ExportType_BMFont_Xml,
    ExportType_BMFont_Binary,
    ExportType_BMFont_Json
};

Q_DECLARE_METATYPE(SGFExportFileType)

#endif // SGFFONTEXPORTERTYPES_H
