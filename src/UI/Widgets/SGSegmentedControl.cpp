#include "SGSegmentedControl.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QToolButton>

namespace {
QString baseStyleSheet()
{
    return QStringLiteral(
        "QToolButton {"
        "  border: 1px solid #1f1f24;"
        "  background: #2e2e34;"
        "  padding: 2px 6px;"
        "  color: #d6d6dc;"
        "  font-size: 9pt;"
        "  min-height: 18px;"
        "}"
        "QToolButton:checked {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4a4d56, stop:1 #353942);"
        "  color: #f2f2f6;"
        "}"
        "QToolButton:hover:!checked { background: #383840; }"
    );
}

QString cornerRules(int count)
{
    if ( count <= 0 ) {
        return QString();
    }
    if ( count == 1 ) {
        return QStringLiteral("QToolButton#sgSeg0 { border-radius: 6px; }");
    }
    if ( count == 2 ) {
        return QStringLiteral(
            "QToolButton#sgSeg0 { border-top-left-radius: 6px; border-bottom-left-radius: 6px; border-right: 0px; }"
            "QToolButton#sgSeg1 { border-top-right-radius: 6px; border-bottom-right-radius: 6px; }"
        );
    }
    QString s;
    s += QStringLiteral(
        "QToolButton#sgSeg0 { border-top-left-radius: 6px; border-bottom-left-radius: 6px; border-right: 0px; }"
    );
    for ( int i = 1; i < count - 1; ++i ) {
        s += QStringLiteral("QToolButton#sgSeg%1 { border-radius: 0px; border-left: 0px; border-right: 0px; }").arg(i);
    }
    s += QStringLiteral("QToolButton#sgSeg%1 { border-top-right-radius: 6px; border-bottom-right-radius: 6px; border-left: 0px; }")
             .arg(count - 1);
    return s;
}
} // namespace

SGSegmentedControl::SGSegmentedControl(QWidget *parent)
    : QWidget(parent)
{
    m_box = new QHBoxLayout(this);
    m_box->setContentsMargins(0, 0, 0, 0);
    m_box->setSpacing(0);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);

    connect(m_group, &QButtonGroup::buttonToggled, this,
        [this](QAbstractButton *button, bool checked) {
            if ( !checked || button == nullptr ) {
                return;
            }
            const int id = m_group->id(button);
            emit currentIndexChanged(id);
            emit currentTextChanged(currentText());
        });
}

void SGSegmentedControl::applyChromeStyleSheet()
{
    setStyleSheet(baseStyleSheet() + cornerRules(m_buttons.size()));
}

void SGSegmentedControl::setOptions(const QStringList &options)
{
    m_options = options;
    rebuildButtons();
}

void SGSegmentedControl::rebuildButtons()
{
    for ( QToolButton *b : m_buttons ) {
        m_group->removeButton(b);
        m_box->removeWidget(b);
        delete b;
    }
    m_buttons.clear();

    const int n = qMin(static_cast<int>(m_options.size()), 8);
    for ( int i = 0; i < n; ++i ) {
        auto *b = new QToolButton(this);
        b->setObjectName(QStringLiteral("sgSeg%1").arg(i));
        b->setCheckable(true);
        b->setAutoRaise(false);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        b->setText(m_options.at(i));
        m_group->addButton(b, i);
        m_box->addWidget(b);
        m_buttons.append(b);
    }

    applyChromeStyleSheet();

    if ( !m_buttons.isEmpty() ) {
        setCheckedIndexInternal(0, false);
    }
}

int SGSegmentedControl::currentIndex() const
{
    for ( int i = 0; i < m_buttons.size(); ++i ) {
        if ( m_buttons.at(i)->isChecked() ) {
            return i;
        }
    }
    return -1;
}

QString SGSegmentedControl::currentText() const
{
    const int idx = currentIndex();
    if ( idx < 0 || idx >= m_options.size() ) {
        return QString();
    }
    return m_options.at(idx);
}

void SGSegmentedControl::setCheckedIndexInternal(int index, bool emitSignals)
{
    if ( index < 0 || index >= m_buttons.size() ) {
        return;
    }
    for ( QToolButton *b : m_buttons ) {
        b->blockSignals(true);
    }
    m_buttons.at(index)->setChecked(true);
    for ( QToolButton *b : m_buttons ) {
        b->blockSignals(false);
    }

    if ( emitSignals ) {
        emit currentIndexChanged(currentIndex());
        emit currentTextChanged(currentText());
    }
}

void SGSegmentedControl::setCurrentIndex(int index)
{
    if ( index == currentIndex() ) {
        return;
    }
    if ( index < 0 || index >= m_buttons.size() ) {
        return;
    }
    setCheckedIndexInternal(index, true);
}

void SGSegmentedControl::setCurrentText(const QString &text)
{
    const int idx = m_options.indexOf(text);
    if ( idx < 0 ) {
        return;
    }
    setCurrentIndex(idx);
}
