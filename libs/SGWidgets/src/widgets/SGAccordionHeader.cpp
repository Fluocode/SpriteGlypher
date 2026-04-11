#include "widgets/SGAccordionHeader.h"
#include "widgets/SGAccordion.h"
#include "widgets/SGAccordionPage.h"


SGAccordionHeader::SGAccordionHeader(SGAccordionPage * page) :
    QWidget(page->parent()),
    mPage(page),
    mAccessory(NULL),
    mSelected(false),
    mIndexInPage(-1)
{
    setBackgroundRole(QPalette::Window);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
    setAutoFillBackground(false);

    mOpenIcon = mPage->parent()->pageOpenIcon();
    mClosedIcon = mPage->parent()->pageClosedIcon();

    mLayout = new QHBoxLayout(this);
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0,0,0,0);

    mArrowButton = new QPushButton(this);
    mArrowButton->setFlat(true);
    mArrowButton->setAutoDefault(false);
    mArrowButton->setDefault(false);
    mArrowButton->setMaximumSize(24, 200);
    mArrowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mArrowButton->setIcon(mClosedIcon);
    // Flat buttons still paint the native (often light) fill on top of the header — keep fully transparent.
    mArrowButton->setStyleSheet(QStringLiteral(
        "QPushButton { background: transparent; border: none; padding: 0px; }"
        "QPushButton:flat { background: transparent; }"));
    mLayout->addWidget(mArrowButton);

    mTitleButton = new QPushButton(this);
    mTitleButton->setFlat(true);
    mTitleButton->setAutoDefault(false);
    mTitleButton->setDefault(false);
    mTitleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mTitleButton->setStyleSheet(QStringLiteral(
        "QPushButton { background: transparent; border: none; text-align: left; color: #c8c8c8; padding: 4px 6px; }"
        "QPushButton:flat { background: transparent; }"));
    mTitleButton->setFont(mPage->parent()->font());
    mLayout->addWidget(mTitleButton);

    connect(mTitleButton, SIGNAL(clicked()), this, SLOT(onTitleButtonClicked()));
    connect(mArrowButton, SIGNAL(clicked()), this, SLOT(onArrowButtonClicked()));

    setLayout(mLayout);
}



void SGAccordionHeader::setOpen(bool b)
{
    mOpen = b;
    mArrowButton->setIcon(b ? mOpenIcon : mClosedIcon);
    update();
}


void SGAccordionHeader::setSelected(bool b)
{
    mSelected = b;

    const QString titleStyle = mSelected
        ? QStringLiteral(
              "QPushButton { background: transparent; border: none; text-align: left; color: #f0f0f0; padding: 4px 6px; }"
              "QPushButton:flat { background: transparent; }"
              "QPushButton:hover { background: rgba(255,255,255,18); }")
        : QStringLiteral(
              "QPushButton { background: transparent; border: none; text-align: left; color: #c8c8c8; padding: 4px 6px; }"
              "QPushButton:flat { background: transparent; }"
              "QPushButton:hover { background: rgba(255,255,255,12); }");
    mTitleButton->setStyleSheet(titleStyle);

    update();
}


QSize SGAccordionHeader::sizeHint() const
{
    return QSize(80, 24);
}


QSize SGAccordionHeader::minimumSizeHint() const
{
    return QSize(80, 24);
}


//
// Accessory Widget
//


void SGAccordionHeader::setAccessory(QWidget * widget)
{
    if ( mAccessory != NULL )
    {
        mLayout->removeWidget(mAccessory);
        delete mAccessory;
        mAccessory = NULL;
    }

    if ( widget != NULL )
    {
        mAccessory = widget;
        mAccessory->setParent(this);
        mLayout->addWidget(mAccessory);
    }
}


QWidget * SGAccordionHeader::accessory()
{
    return mAccessory;
}


QWidget * SGAccordionHeader::accessory() const
{
    return mAccessory;
}



void SGAccordionHeader::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);

    // Dark panel styling: inactive headers match surrounding chrome; active uses a subdued blue accent.
    QColor c1 = mSelected ? QColor(52, 78, 118) : QColor(58, 58, 62);
    QColor c2 = mSelected ? QColor(38, 56, 88) : QColor(45, 45, 50);
    QColor lineColor = mSelected ? QColor(65, 95, 140) : QColor(78, 78, 84);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, c1);
    gradient.setColorAt(1, c2);
    painter.fillRect(rect(), gradient);

    painter.setPen(lineColor);
    painter.drawLine(rect().topLeft(), rect().topRight());
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());
}


//
// Signals
//

void SGAccordionHeader::onTitleButtonClicked()
{
    mPage->makeCurrent();
}


void SGAccordionHeader::onArrowButtonClicked()
{
    mPage->toggle();
}


