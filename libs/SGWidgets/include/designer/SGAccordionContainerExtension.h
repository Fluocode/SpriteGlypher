#ifndef SGACCORDIONCONTAINEREXTENSION_H
#define SGACCORDIONCONTAINEREXTENSION_H

#include <QObject>
#include <QDesignerContainerExtension>

class SGAccordion;


class SGAccordionContainerExtension : public QObject, public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)

public:
    SGAccordionContainerExtension(SGAccordion *widget, QObject *parent = 0);

    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);
    QWidget *widget(int index) const;
    void remove(int index);
    int count() const;

    void setCurrentIndex(int index);
    int currentIndex() const;

private:
    SGAccordion *mWidget;
};

#endif // SGACCORDIONCONTAINEREXTENSION_H
