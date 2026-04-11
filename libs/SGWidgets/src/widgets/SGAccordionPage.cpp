#include "widgets/SGAccordionPage.h"
#include "widgets/SGAccordion.h"

SGAccordionPage::SGAccordionPage(SGAccordion *parent, QWidget *widget) :
    mParent(parent),
    mHeader(NULL),
    mWidget(widget),
    mIsOpen(true)
{
    setObjectName("sg_accordion_page");

    mLayout = new QVBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    setLayout(mLayout);

    mHeader = new SGAccordionHeader(this);
    mHeader->setObjectName(QLatin1String("sg_accordion_header"));
    mHeader->show();
    mLayout->addWidget(mHeader);

    mWidget->setParent(this);
    mWidget->hide();
    mLayout->addWidget(mWidget);

    if ( mWidget != NULL ) {
        setText(mWidget->windowTitle());
    }
}


SGAccordion * SGAccordionPage::parent()
{
    return mParent;
}


void SGAccordionPage::setSelected(bool value)
{
    mHeader->setSelected(value);

    if ( value == true ) {
        open();
    }
}


QString SGAccordionPage::text() const
{
    return (mHeader && mHeader->button()) ? mHeader->button()->text() : QString();
}


void SGAccordionPage::setText(const QString &text)
{
    if (mHeader && mHeader->button()) {
        mHeader->button()->setText(text);
    }

    if (mWidget) {
        mWidget->setWindowTitle(text);
    }
}


QIcon SGAccordionPage::icon() const
{
    return (mHeader && mHeader->button()) ? mHeader->button()->icon() : QIcon();
}


void SGAccordionPage::setIcon(const QIcon &icon)
{
    if (mHeader && mHeader->button()) {
        mHeader->button()->setIcon(icon);
    }
}


QString SGAccordionPage::toolTip() const
{
    return (mHeader) ? mHeader->toolTip() : QString();
}


void SGAccordionPage::setToolTip(const QString &tip)
{
    if (mHeader) {
        mHeader->setToolTip(tip);
    }
}


void SGAccordionPage::setAccessory(QWidget * widget)
{
    mHeader->setAccessory(widget);
}


QWidget * SGAccordionPage::accessory()
{
    return mHeader->accessory();
}


QWidget * SGAccordionPage::accessory() const
{
    return mHeader->accessory();
}


SGAccordionHeader * SGAccordionPage::header()
{
    return mHeader;
}


SGAccordionHeader * SGAccordionPage::header() const
{
    return mHeader;
}



QWidget * SGAccordionPage::widget()
{
    return mWidget;
}


QWidget * SGAccordionPage::widget() const
{
    return mWidget;
}


void SGAccordionPage::open()
{
    if ( mIsOpen ) {
        return;
    }

    mWidget->show();
    mHeader->setOpen(true);
    mIsOpen = true;
}


void SGAccordionPage::close()
{
    if ( !mIsOpen ) {
        return;
    }

    mWidget->hide();
    mHeader->setOpen(false);
    mIsOpen = false;
}


void SGAccordionPage::toggle()
{
    if ( mIsOpen ) {
        close();
    } else {
        open();
    }
}


void SGAccordionPage::makeCurrent()
{
    mParent->setCurrentIndex(mParent->indexOf(mWidget));
}
