#ifndef EFFECTLISTROW_H
#define EFFECTLISTROW_H

#include <QCheckBox>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

namespace Ui {
class EffectListRow;
}

class EffectListRow : public QWidget
{
    Q_OBJECT

    // Object Lifecycle
public:
    explicit EffectListRow(QWidget *parent = 0, const QString &title = QString(), bool checked = false);
    ~EffectListRow();


public:
    const QString title() const;
    void setTitle(const QString &value);

    QPixmap icon() const;
    void setIcon(const QPixmap &value);

    bool isChecked() const;
    void setChecked(bool value);


signals:
    void checkedStateChanged(bool checked);

private slots:
    void checkBoxStateChanged(int state);

private:
    Ui::EffectListRow *ui;
};

#endif // EFFECTLISTROW_H
