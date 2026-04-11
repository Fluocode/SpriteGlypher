#include "SGFExportSettings.h"

#include "Exporters/SGFFontExporter.h"

const QString SGFExportSettings::kRootKey = QString("ExportSettings");
const QString SGFExportSettings::kFormatKey = QString("format");
const QString SGFExportSettings::kPathKey = QString("path");
const QString SGFExportSettings::kRetinaCopyKey = QString("retinaCopy");


SGFExportSettings::SGFExportSettings() :
    format(ExportType_BMFont_Xml),
    path(),
    retinaCopy(false)
{

}


SGFExportSettings::SGFExportSettings(const QDomElement &element) :
    format(ExportType_BMFont_Xml),
    path(),
    retinaCopy(false)
{
    readFromXmlNode(element);
}


bool SGFExportSettings::writeToXmlStream(QXmlStreamWriter &writer)
{
    if ( writer.hasError() ) {
        return false;
    }

    writer.writeStartElement(kRootKey);
    writer.writeAttribute(kFormatKey, SGFFontExporter::ExportFileTypeToString(format));
    writer.writeAttribute(kPathKey, path);
    writer.writeAttribute(kRetinaCopyKey, QString::number((retinaCopy == true) ? 1 : 0));
    writer.writeEndElement();

    return true;
}


void SGFExportSettings::readFromXmlNode(const QDomElement &element)
{
    QString xmlFormat = element.attribute(kFormatKey);
    QString xmlPath = element.attribute(kPathKey);
    QString xmlRetinaCopy = element.attribute(kRetinaCopyKey);

    if ( !xmlFormat.isEmpty() ) { format = SGFFontExporter::ExportFileTypeFromString(xmlFormat); }
    if ( !xmlPath.isEmpty() ) { path = xmlPath; }
    if ( !xmlRetinaCopy.isEmpty() ) { retinaCopy = (xmlRetinaCopy.toInt() == 1); }
}

