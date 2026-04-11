#ifndef SGFDOCUMENTELEMENT_H
#define SGFDOCUMENTELEMENT_H

#include <QtXml>


/**
 * @brief A simple interface that allows a class that the file reader/writer can use to serialize a document element.
 */
class SGFDocumentElement
{
public:
    SGFDocumentElement();

    virtual ~SGFDocumentElement();

    /** Write this document element to the given Xml stream. */
    virtual bool writeToXmlStream(QXmlStreamWriter &writer) = 0;

    /** Read properties for this element from the given xml node. */
    virtual void readFromXmlNode(const QDomElement & element) = 0;

};

#endif // SGFDOCUMENTELEMENT_H
