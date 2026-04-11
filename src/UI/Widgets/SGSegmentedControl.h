#ifndef SGSEGMENTEDCONTROL_H
#define SGSEGMENTEDCONTROL_H

#include <QVector>
#include <QWidget>

class QToolButton;
class QButtonGroup;
class QHBoxLayout;

/**
 * Segmented control (pill toggle) for small option lists (typically 2–3 items).
 * Replaces QComboBox when the domain is tiny.
 */
class SGSegmentedControl : public QWidget
{
    Q_OBJECT

public:
    explicit SGSegmentedControl(QWidget *parent = nullptr);

    void setOptions(const QStringList &options);
    QStringList options() const { return m_options; }

    int currentIndex() const;
    QString currentText() const;

public slots:
    void setCurrentIndex(int index);
    void setCurrentText(const QString &text);

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);

private:
    void rebuildButtons();
    void applyChromeStyleSheet();
    void setCheckedIndexInternal(int index, bool emitSignals);

    QStringList m_options;
    QVector<QToolButton *> m_buttons;
    QHBoxLayout *m_box = nullptr;
    QButtonGroup *m_group = nullptr;
};

#endif // SGSEGMENTEDCONTROL_H
