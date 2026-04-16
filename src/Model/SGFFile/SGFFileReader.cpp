#include "SGFFileReader.h"

#include <QDomDocument>
#include "SGFFileWriter.h"

SGFFileReader::SGFFileReader()
{

}

QString SGFFileReader::lastError()
{
    return mLastError;
}

SGFDocument::Ptr SGFFileReader::loadDocument(const QString &path)
{
    mDocument = SGFDocument::Ptr(new SGFDocument());

    QFile file(path);
    QDomDocument document("sfbDocument");

    if ( !document.setContent(&file) ) {
        mLastError = "Failed to load XML from file at path: " + path;
        return SGFDocument::Ptr(nullptr);
    }

    QDomElement docNode = document.documentElement();

    if ( docNode.isNull() || docNode.tagName().compare(SGFFileWriter::kSGFRootKey) != 0 ) {
        mLastError = "Failed to find node: " + SGFFileWriter::kSGFRootKey;
        return SGFDocument::Ptr(nullptr);
    }

    QDomNodeList nodes = docNode.childNodes();

    for( int i = 0; i < nodes.count(); ++i )
    {
        QDomElement element = nodes.at(i).toElement();
        QString tagName = element.tagName();
        //qDebug() << tagName;

        if ( tagName.compare(SGFInputSettings::kRootKey, Qt::CaseInsensitive) == 0 )
        {
            mDocument->setInputSettings(SGFInputSettings(element));
        }
        else if ( tagName.compare(SGFGenerationSettings::kRootKey, Qt::CaseInsensitive) == 0 )
        {
            mDocument->setGenerationSettings(SGFGenerationSettings(element));
        }
        else if ( tagName.compare(SGFExportSettings::kRootKey, Qt::CaseInsensitive) == 0 )
        {
            mDocument->setExportSettings(SGFExportSettings(element));
        }
        else if ( tagName.compare(SGFFileWriter::kSGFEffectsKey, Qt::CaseInsensitive) == 0 )
        {
            QDomNodeList effectNodes = element.childNodes();

            for( int j = 0; j < effectNodes.count(); ++j )
            {
                QDomElement effectElement = effectNodes.at(j).toElement();

                if ( effectElement.isNull() || effectElement.tagName().compare(SGFEffect::kRootKey) != 0 ) {
                    continue;
                }

                SGFEffect::Ptr effect = SGFEffect::CreateEffectFromXmlNode(effectElement);

                if ( effect != nullptr )
                {
                    mDocument->addEffect(effect);
                }
            }
        }
    }

    mDocument->setDocumentPath(path);
    mDocument->setNeedsSavingToFalse();
    return mDocument;
}



