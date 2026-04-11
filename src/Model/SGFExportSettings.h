#ifndef SGFEXPORTSETTINGS_H
#define SGFEXPORTSETTINGS_H

#include <QtCore>

#include "SGFFile/SGFDocumentElement.h"
#include "Exporters/SGFFontExporterTypes.h"

class SGFExportSettings : public SGFDocumentElement
{
public:
    static const QString kRootKey;

private:
    static const QString kFormatKey;
    static const QString kPathKey;
    static const QString kRetinaCopyKey;

public:
    SGFExportSettings();
    SGFExportSettings(const QDomElement & element);

public:
    virtual bool writeToXmlStream(QXmlStreamWriter &writer);
    virtual void readFromXmlNode(const QDomElement & element);

public:
    SGFExportFileType format;     //< File Format to Export
    QString path;       //< Path
    bool retinaCopy;    //< Should we also save a retina @2x version?
};

Q_DECLARE_METATYPE(SGFExportSettings)

#endif // SGFEXPORTSETTINGS_H
