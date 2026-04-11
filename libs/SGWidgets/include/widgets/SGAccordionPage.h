#ifndef SGACCORDIONPAGE_H
#define SGACCORDIONPAGE_H

#include <memory>

#include "SGAccordionHeader.h"


// Forward Declaration
class SGAccordion;


/**
 * @brief Represents a single page on an accordion widget and provides functionality for manipulating the page.
 */
class SGAccordionPage : public QWidget
{
    Q_OBJECT

public:
    SGAccordionPage(SGAccordion * parent, QWidget * widget);

public:
    SGAccordion * parent();

    void setSelected(bool value);

    QString text() const;
    void setText(const QString &text);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString toolTip() const;
    void setToolTip(const QString &tip);

    void setAccessory(QWidget * widget);
    QWidget * accessory();
    QWidget * accessory() const;

    SGAccordionHeader * header();          //< Todo: Remove this and encapsulate
    SGAccordionHeader * header() const;    //< Todo: Remove this and encapsulate
    QWidget * widget();
    QWidget * widget() const;


    bool operator==(const SGAccordionPage& other) const
    {
        return mWidget == other.mWidget;
    }

    void open();
    void close();
    void toggle();
    void makeCurrent();

private:
    SGAccordion * mParent;

    QVBoxLayout * mLayout;
    SGAccordionHeader * mHeader;
    QWidget * mWidget;

    bool mIsOpen;
};

#endif // SGACCORDIONPAGE_H
