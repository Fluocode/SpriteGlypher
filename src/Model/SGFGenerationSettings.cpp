#include "SGFGenerationSettings.h"


const QString SGFGenerationSettings::kRootKey = QString("GenerationSettings");
const QString SGFGenerationSettings::kWidthKey = QString("width");
const QString SGFGenerationSettings::kHeightKey = QString("height");
const QString SGFGenerationSettings::kColorKey = QString("color");
const QString SGFGenerationSettings::kPaddingKey = QString("padding");
const QString SGFGenerationSettings::kSpacingKey = QString("spacing");
const QString SGFGenerationSettings::kPaginateKey = QString("paginate");


SGFGenerationSettings::SGFGenerationSettings() :
    width(0),
    height(0),
    // Preview backdrop only (see MainWindow atlas view). Not written into the exported texture.
    color(QColor(0, 0, 0, 128)),
    padding(2),
    spacing(2),
    paginate(false)
{

}


SGFGenerationSettings::SGFGenerationSettings(const QDomElement & element) :
    width(0),
    height(0),
    // Preview backdrop only (see MainWindow atlas view). Not written into the exported texture.
    color(QColor(0, 0, 0, 128)),
    padding(2),
    spacing(2),
    paginate(false)
{
    readFromXmlNode(element);
}


bool SGFGenerationSettings::writeToXmlStream(QXmlStreamWriter &writer)
{
    if ( writer.hasError() ) {
        return false;
    }

    writer.writeStartElement(kRootKey);
    writer.writeAttribute(kWidthKey, QString::number(width));
    writer.writeAttribute(kHeightKey, QString::number(height));
    writer.writeAttribute(kColorKey, color.name(QColor::HexArgb));
    writer.writeAttribute(kPaddingKey, QString::number(padding));
    writer.writeAttribute(kSpacingKey, QString::number(spacing));
    writer.writeAttribute(kPaginateKey, QString::number(paginate ? 1 : 0));
    writer.writeEndElement();

    return true;
}


void SGFGenerationSettings::readFromXmlNode(const QDomElement &element)
{
    QString xmlWidth = element.attribute(kWidthKey);
    QString xmlHeight = element.attribute(kHeightKey);
    QString xmlColor = element.attribute(kColorKey);
    QString xmlPadding = element.attribute(kPaddingKey);
    QString xmlSpacing = element.attribute(kSpacingKey);
    QString xmlPaginate = element.attribute(kPaginateKey);

    if ( !xmlWidth.isEmpty() ) { width = xmlWidth.toInt(); }
    if ( !xmlHeight.isEmpty() ) { height = xmlHeight.toInt(); }
    if ( !xmlColor.isEmpty() ) { color = QColor(xmlColor); }
    if ( !xmlPadding.isEmpty() ) { padding = xmlPadding.toInt(); }
    if ( !xmlSpacing.isEmpty() ) { spacing = xmlSpacing.toInt(); }
    if ( !xmlPaginate.isEmpty() ) { paginate = (xmlPaginate.toInt() == 1); }
}
