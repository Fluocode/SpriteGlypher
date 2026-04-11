#include "SGFGlyph.h"

#include <QPainter>
#include "Effects/SGFEffect.h"


SGFGlyph::SGFGlyph()
    : mHasPreparedImages(false)
    , mImportedRaster(false)
{

}


SGFGlyph::SGFGlyph(QFont aFont, QChar aChar)
{
    setCharacter(aFont, aChar);
}


void SGFGlyph::setImportedFromPng(QChar aChar, const QImage &source)
{
    mImportedRaster = true;
    character = aChar;
    font = QFont();
    path = QPainterPath();

    QImage img = source;
    if ( img.format() != QImage::Format_ARGB32 ) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }
    image = img;
    backgroundImage = QImage();
    maskImage = QImage();
    tempImage = QImage();

    baseSize = img.size();
    effectPadding = SGFPadding(0.f, 0.f, 0.f, 0.f);
    minSize = QRect(0, 0, baseSize.width(), baseSize.height());
    mHasPreparedImages = true;
}


void SGFGlyph::setCharacter(QFont aFont, QChar aChar)
{
    mImportedRaster = false;
    font = aFont;
    character = aChar;
    path.addText(0, 0, font, character);

    QRectF bounds = path.boundingRect();
    baseSize = QSize((int)ceil(bounds.width()), (int)ceil(bounds.height()));
    effectPadding = SGFPadding(0,0,0,0);

    image = QImage();
    tempImage = QImage();
    backgroundImage = QImage();
    maskImage = QImage();

    mHasPreparedImages = false;
}


void SGFGlyph::expandSizeForEffect(SGFEffect * effect)
{
    if ( effect == nullptr ) {
        return;
    }

    SGFPadding padding = effect->paddingRequiredForGlyph(*this);
    effectPadding.top = std::max(padding.top, effectPadding.top);
    effectPadding.right = std::max(padding.right, effectPadding.right);
    effectPadding.bottom = std::max(padding.bottom, effectPadding.bottom);
    effectPadding.left = std::max(padding.left, effectPadding.left);

    minSize.setWidth(baseSize.width() + effectPadding.left + effectPadding.right);
    minSize.setHeight(baseSize.height() + effectPadding.top + effectPadding.bottom);

    mHasPreparedImages = false;
}


void SGFGlyph::prepareImages()
{
    if ( mImportedRaster || mHasPreparedImages ) {
        return;
    }

    // First move glyph to top left corner of canvas
    QRectF bounds = path.boundingRect();
    path.translate(-bounds.x(), -bounds.y());

    // Now move to adjust for padding
    path.translate(effectPadding.left, effectPadding.top);

    // Generate the layers for this glyph. It looks like a lot, but effects no longer generate temp images
    image = QImage((int)ceil(minSize.width()), (int)ceil(minSize.height()), QImage::Format_ARGB32);
    image.fill(QColor(0,0,0,0));

    tempImage = image.copy();
    backgroundImage = image.copy();
    maskImage = image.copy();

    // If there is nothing to draw (space | empty char) then there's nothing to do!
    if ( image.isNull() ) {
        return;
    }

    QPainter painter(&maskImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(path, Qt::black);
    painter.end();

    mHasPreparedImages = true;
}


void SGFGlyph::composeFinalImage()
{
    if ( mImportedRaster || !mHasPreparedImages ) {
        return;
    }

    if ( image.width() == 0 || image.height() == 0 ) {
        return;
    }

    tempImage.fill(QColor(0,0,0,0));

    QPainter painter(&tempImage);
    painter.drawImage(0, 0, backgroundImage);
    painter.drawImage(0, 0, image);
    painter.end();

    image = tempImage.copy();
}
