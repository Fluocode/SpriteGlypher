#ifndef GENERATIONSETTINGSPANEL_H
#define GENERATIONSETTINGSPANEL_H

#include <QWidget>

#include "../../Model/SGFTypes.h"


namespace Ui {
class GenerationSettingsPanel;
}

class GenerationSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GenerationSettingsPanel(QWidget *parent = 0);
    ~GenerationSettingsPanel();

    void setValue(const SGFGenerationSettings & value);
    SGFGenerationSettings getValue() const;

    /** Sync with MainWindow glyph-outline / bounds overlay (not part of SGFGenerationSettings). */
    void setShowGlyphBounds(bool on);

signals:
    void valueChanged(SGFGenerationSettings value);

    /** Same behavior as View → Toggle Glyph Outlines. */
    void showGlyphBoundsChanged(bool on);

private slots:
    void subWidgetValueChanged();

private:
    Ui::GenerationSettingsPanel *ui;

    bool mIsUpdatingGui;
    SGFGenerationSettings mValue;
};

#endif // GENERATIONSETTINGSPANEL_H
