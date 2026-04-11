#include "SGFFileWriter.h"

//
// Constants
//
const QString SGFFileWriter::kSGFRootKey = QString("SpriteFont");
const QString SGFFileWriter::kSGFEffectsKey = QString("Effects");


SGFFileWriter::SGFFileWriter(QString path) :
    mPath(path)
{

}


bool SGFFileWriter::writeDocument(SGFDocument *document)
{
    QFile outFile(mPath);

    if ( !outFile.open(QIODevice::WriteOnly) ) {
        mLastError = "Could not open file '" + mPath + "' for writing.";
        return false;
    }

    QXmlStreamWriter writer;
    writer.setDevice(&outFile);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(SGFFileWriter::kSGFRootKey);       // <SpriteFont>

    document->getInputSettings().writeToXmlStream(writer);
    document->getGenerationSettings().writeToXmlStream(writer);
    document->getExportSettings().writeToXmlStream(writer);

    writer.writeStartElement(kSGFEffectsKey);                   // <Effects>
    int effectCount = document->getEffectCount();

    for( int i = 0; i < effectCount; ++i )
    {
        SGFEffect * effect = document->getEffectAtIndex(i).get();
        effect->writeToXmlStream(writer);
    }

    writer.writeEndElement();          // </Effects>
    writer.writeEndElement();          // </SpriteFont>
    writer.writeEndDocument();

    document->setDocumentPath(mPath);
    document->setNeedsSavingToFalse();
    return true;
}


QString SGFFileWriter::lastError()
{
    return mLastError;
}
