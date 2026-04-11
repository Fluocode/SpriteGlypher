#include "designer/SGAccordionContainerExtension.h"

#include "widgets/SGAccordion.h"


SGAccordionContainerExtension::SGAccordionContainerExtension(SGAccordion *widget, QObject *parent) :
    QObject(parent)
{
    mWidget = widget;
}


void SGAccordionContainerExtension::addWidget(QWidget *widget)
{
    mWidget->addPage(widget);
}


void SGAccordionContainerExtension::insertWidget(int index, QWidget *widget)
{
    mWidget->insertPage(index, widget);
}


QWidget* SGAccordionContainerExtension::widget(int index) const
{
    return mWidget->widget(index);
}


void SGAccordionContainerExtension::remove(int index)
{
    mWidget->removePage(index);
}


int SGAccordionContainerExtension::count() const
{
    return mWidget->pageCount();
}


void SGAccordionContainerExtension::setCurrentIndex(int index)
{
    mWidget->setCurrentIndex(index);
}


int SGAccordionContainerExtension::currentIndex() const
{
    return mWidget->currentIndex();
}

