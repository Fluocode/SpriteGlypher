#ifndef INPUTSETTINGSPANEL_H
#define INPUTSETTINGSPANEL_H

#include <QWidget>
#include <QFontDatabase>

#include "../../Model/SGFTypes.h"

class QToolButton;
class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QFrame;

namespace Ui {
class InputSettingsPanel;
}

class InputSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InputSettingsPanel(QWidget *parent = nullptr);
    ~InputSettingsPanel();

    void setValue(const SGFInputSettings & value);
    SGFInputSettings getValue() const;

signals:
    void valueChanged(SGFInputSettings value);

private slots:
    void fontFamilyChanged();
    void fontStyleChanged();
    void fontSizeChanged();
    void inputCharactersChanged();

    void on_buttonAscii_clicked();

    void on_buttonNehe_clicked();

    void onPngModeToggled(bool pngSelected);
    void onPngFaceEdited(const QString &text);
    void onPngAddRow();
    void onPngRemoveRow();
    void onPngMoveUp();
    void onPngMoveDown();
    void onPngBrowseClicked();
    void onFontFileModeToggled(bool fileSelected);
    void onFontFileDropped(const QString &path);

private:
    void updateSourceModeUi();
    void rebuildPngTable();
    void readPngTableIntoSettings();
    void emitValueChanged();

    Ui::InputSettingsPanel *ui;

    bool mEmitSignals = true;
    SGFInputSettings mValue;

    QWidget *m_sourceGroup = nullptr;
    QToolButton *m_btnSystemFont = nullptr;
    QToolButton *m_btnPng = nullptr;
    QWidget *m_pngWidget = nullptr;
    QLineEdit *m_pngFaceEdit = nullptr;
    QTableWidget *m_pngTable = nullptr;
    QPushButton *m_pngAdd = nullptr;
    QPushButton *m_pngRemove = nullptr;
    QPushButton *m_pngUp = nullptr;
    QPushButton *m_pngDown = nullptr;

    QToolButton *m_btnFontFile = nullptr;
    QWidget *m_fontFileWidget = nullptr;
    QLabel *m_fontFileNameLabel = nullptr;
    QFrame *m_fontFileDropArea = nullptr;
};

#endif // INPUTSETTINGSPANEL_H
