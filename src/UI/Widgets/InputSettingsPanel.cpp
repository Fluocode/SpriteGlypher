#include "InputSettingsPanel.h"
#include "ui_InputSettingsPanel.h"

#include <QAbstractItemView>
#include <QFileDialog>
#include <QFont>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QFrame>
#include <functional>

namespace {

class FontDropArea : public QFrame
{
public:
    std::function<void(const QString &path)> onFileDropped;

    explicit FontDropArea(QWidget *parent = nullptr) : QFrame(parent)
    {
        setAcceptDrops(true);
        setFixedHeight(90);
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Sunken);
        setStyleSheet(QStringLiteral(
            "QFrame {"
            "  border: 1px dashed rgba(220,220,230,110);"
            "  border-radius: 10px;"
            "  background: rgba(255,255,255,6);"
            "}"
            "QFrame[dragOver=\"true\"] {"
            "  border: 1px dashed rgba(255,255,255,210);"
            "  background: rgba(90,120,200,40);"
            "}"));

        auto *l = new QVBoxLayout(this);
        l->setContentsMargins(10, 10, 10, 10);
        l->setSpacing(4);
        auto *t = new QLabel(tr("Drag & drop a .ttf / .otf font file here"), this);
        t->setWordWrap(true);
        t->setAlignment(Qt::AlignCenter);
        t->setStyleSheet(QStringLiteral("QLabel { color: rgba(220,220,230,180); }"));
        l->addWidget(t);
    }

protected:
    void dragEnterEvent(QDragEnterEvent *e) override
    {
        if ( e->mimeData() && e->mimeData()->hasUrls() ) {
            const QList<QUrl> urls = e->mimeData()->urls();
            if ( !urls.isEmpty() ) {
                const QString p = urls.first().toLocalFile();
                const QString ext = QFileInfo(p).suffix().toLower();
                if ( ext == QStringLiteral("ttf") || ext == QStringLiteral("otf") || ext == QStringLiteral("ttc") ) {
                    setProperty("dragOver", true);
                    style()->unpolish(this);
                    style()->polish(this);
                    e->acceptProposedAction();
                    return;
                }
            }
        }
        e->ignore();
    }

    void dragLeaveEvent(QDragLeaveEvent *e) override
    {
        Q_UNUSED(e);
        setProperty("dragOver", false);
        style()->unpolish(this);
        style()->polish(this);
    }

    void dropEvent(QDropEvent *e) override
    {
        setProperty("dragOver", false);
        style()->unpolish(this);
        style()->polish(this);

        if ( e->mimeData() && e->mimeData()->hasUrls() ) {
            const QList<QUrl> urls = e->mimeData()->urls();
            if ( !urls.isEmpty() ) {
                const QString p = urls.first().toLocalFile();
                if ( !p.isEmpty() ) {
                    if ( onFileDropped ) {
                        onFileDropped(p);
                    }
                    e->acceptProposedAction();
                    return;
                }
            }
        }
        e->ignore();
    }
};

}

InputSettingsPanel::InputSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputSettingsPanel)
{
    ui->setupUi(this);
    ui->rowFontSize->configureInt(6, 256, 1);
    ui->inputTextEditCharacters->setWordWrapMode(QTextOption::WrapAnywhere);

    QFont compactFont = font();
    compactFont.setPointSize(9);

    m_sourceGroup = new QWidget(this);
    m_sourceGroup->setFont(compactFont);
    auto *gv = new QVBoxLayout(m_sourceGroup);
    gv->setContentsMargins(0, 0, 0, 0);
    gv->setSpacing(0);

    auto *segRow = new QWidget(m_sourceGroup);
    auto *seg = new QHBoxLayout(segRow);
    seg->setContentsMargins(0, 0, 0, 0);
    seg->setSpacing(0);

    m_btnSystemFont = new QToolButton(segRow);
    m_btnFontFile = new QToolButton(segRow);
    m_btnPng = new QToolButton(segRow);
    for ( QToolButton *b : {m_btnSystemFont, m_btnFontFile, m_btnPng} ) {
        b->setFont(compactFont);
        b->setCheckable(true);
        b->setAutoRaise(false);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        b->setMinimumHeight(28);
    }
    m_btnSystemFont->setText(tr("System font"));
    m_btnFontFile->setText(tr("Font file"));
    m_btnPng->setText(tr("PNG images"));

    // Exclusive selection (segmented control)
    auto *bg = new QButtonGroup(segRow);
    bg->setExclusive(true);
    bg->addButton(m_btnSystemFont);
    bg->addButton(m_btnFontFile);
    bg->addButton(m_btnPng);
    m_btnSystemFont->setChecked(true);

    // Segmented control — dark theme (matches MainWindow panel chrome)
    segRow->setStyleSheet(QStringLiteral(
        "QToolButton {"
        "  border: 1px solid #1f1f24;"
        "  background: #2e2e34;"
        "  padding: 6px 10px;"
        "  color: #d6d6dc;"
        "}"
        "QToolButton:checked {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4a4d56, stop:1 #353942);"
        "  color: #f2f2f6;"
        "}"
        "QToolButton:hover:!checked { background: #383840; }"
        "QToolButton#sgSegLeft { border-top-left-radius: 8px; border-bottom-left-radius: 8px; border-right: 0px; }"
        "QToolButton#sgSegMid { border-right: 0px; }"
        "QToolButton#sgSegRight { border-top-right-radius: 8px; border-bottom-right-radius: 8px; }"
    ));
    m_btnSystemFont->setObjectName(QStringLiteral("sgSegLeft"));
    m_btnFontFile->setObjectName(QStringLiteral("sgSegMid"));
    m_btnPng->setObjectName(QStringLiteral("sgSegRight"));

    seg->addWidget(m_btnSystemFont);
    seg->addWidget(m_btnFontFile);
    seg->addWidget(m_btnPng);
    gv->addWidget(segRow);

    m_fontFileWidget = new QWidget(this);
    m_fontFileWidget->setFont(compactFont);
    auto *fv = new QVBoxLayout(m_fontFileWidget);
    fv->setContentsMargins(0, 0, 0, 0);
    fv->setSpacing(6);
    m_fontFileNameLabel = new QLabel(tr("Font: (drop a file below)"), m_fontFileWidget);
    m_fontFileNameLabel->setWordWrap(true);
    fv->addWidget(m_fontFileNameLabel);
    m_fontFileDropArea = new FontDropArea(m_fontFileWidget);
    static_cast<FontDropArea *>(m_fontFileDropArea)->onFileDropped = [this](const QString &p) { onFontFileDropped(p); };
    fv->addWidget(m_fontFileDropArea);

    m_pngWidget = new QWidget(this);
    m_pngWidget->setFont(compactFont);
    auto *pv = new QVBoxLayout(m_pngWidget);
    pv->setContentsMargins(0, 0, 0, 0);
    auto *faceLabel = new QLabel(tr("Face name (metadata / export):"));
    faceLabel->setWordWrap(true);
    pv->addWidget(faceLabel);
    m_pngFaceEdit = new QLineEdit(QStringLiteral("PNG Font"));
    pv->addWidget(m_pngFaceEdit);

    m_pngTable = new QTableWidget(0, 3);
    m_pngTable->setFont(compactFont);
    m_pngTable->setHorizontalHeaderLabels({tr("Char"), tr("PNG file"), QString()});
    m_pngTable->horizontalHeader()->setFont(compactFont);
    m_pngTable->verticalHeader()->setVisible(false);
    m_pngTable->horizontalHeader()->setMinimumSectionSize(48);
    m_pngTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_pngTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_pngTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_pngTable->setColumnWidth(0, 56);
    m_pngTable->setColumnWidth(2, 34);
    m_pngTable->setMinimumHeight(120);
    m_pngTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pngTable->setSelectionMode(QAbstractItemView::SingleSelection);
    pv->addWidget(m_pngTable);

    auto *pg = new QGridLayout;
    pg->setContentsMargins(0, 0, 0, 0);
    pg->setHorizontalSpacing(4);
    pg->setVerticalSpacing(4);

    m_pngAdd = new QPushButton(tr("Add"));
    m_pngRemove = new QPushButton(tr("Remove"));
    m_pngUp = new QPushButton(tr("Up"));
    m_pngDown = new QPushButton(tr("Down"));
    for ( QPushButton *b : {m_pngAdd, m_pngRemove, m_pngUp, m_pngDown} ) {
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    pg->addWidget(m_pngAdd, 0, 0);
    pg->addWidget(m_pngRemove, 0, 1);
    pg->addWidget(m_pngUp, 1, 0);
    pg->addWidget(m_pngDown, 1, 1);
    pv->addLayout(pg);

    ui->verticalLayout->insertWidget(0, m_sourceGroup);
    ui->verticalLayout->insertWidget(1, m_fontFileWidget);
    ui->verticalLayout->insertWidget(2, m_pngWidget);

    QObject::connect(m_btnPng, &QToolButton::toggled, this, &InputSettingsPanel::onPngModeToggled);
    QObject::connect(m_btnFontFile, &QToolButton::toggled, this, &InputSettingsPanel::onFontFileModeToggled);
    QObject::connect(m_pngFaceEdit, &QLineEdit::textEdited, this, &InputSettingsPanel::onPngFaceEdited);
    QObject::connect(m_pngAdd, &QPushButton::clicked, this, &InputSettingsPanel::onPngAddRow);
    QObject::connect(m_pngRemove, &QPushButton::clicked, this, &InputSettingsPanel::onPngRemoveRow);
    QObject::connect(m_pngUp, &QPushButton::clicked, this, &InputSettingsPanel::onPngMoveUp);
    QObject::connect(m_pngDown, &QPushButton::clicked, this, &InputSettingsPanel::onPngMoveDown);

    QObject::connect(ui->inputFontComboBoxFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(fontFamilyChanged()));
    QObject::connect(ui->inputFontComboBoxStyle, SIGNAL(currentTextChanged(QString)), this, SLOT(fontStyleChanged()));
    QObject::connect(ui->rowFontSize, SIGNAL(valueChanged()), this, SLOT(fontSizeChanged()));
    QObject::connect(ui->inputTextEditCharacters, SIGNAL(textChanged()), this, SLOT(inputCharactersChanged()));
    // Drop area calls onFontFileDropped via callback (no MOC needed).

    updateSourceModeUi();
}

InputSettingsPanel::~InputSettingsPanel()
{
    delete ui;
}

void InputSettingsPanel::onFontFileModeToggled(bool fileSelected)
{
    Q_UNUSED(fileSelected);
    if ( m_btnFontFile != nullptr && m_btnFontFile->isChecked() ) {
        mValue.inputSource = SGFInputSource::FontFile;
    }
    updateSourceModeUi();
    emitValueChanged();
}

void InputSettingsPanel::onFontFileDropped(const QString &path)
{
    const QString ext = QFileInfo(path).suffix().toLower();
    if ( !(ext == QStringLiteral("ttf") || ext == QStringLiteral("otf") || ext == QStringLiteral("ttc")) ) {
        return;
    }

    const int fontId = QFontDatabase::addApplicationFont(path);
    const QStringList fams = QFontDatabase::applicationFontFamilies(fontId);
    if ( fams.isEmpty() ) {
        m_fontFileNameLabel->setText(tr("Font: (invalid font file)"));
        return;
    }

    mValue.inputSource = SGFInputSource::FontFile;
    mValue.fontFilePath = path;
    mValue.fontFamily = fams.first();

    // Sync UI selectors to the loaded family.
    ui->inputFontComboBoxFamily->setCurrentText(mValue.fontFamily);
    fontFamilyChanged();
    m_fontFileNameLabel->setText(tr("Font: %1").arg(mValue.fontFamily));

    if ( m_btnFontFile != nullptr ) {
        m_btnFontFile->setChecked(true);
    }

    updateSourceModeUi();
    emitValueChanged();
}

void InputSettingsPanel::updateSourceModeUi()
{
    const bool png = m_btnPng != nullptr && m_btnPng->isChecked();
    m_pngWidget->setVisible(png);

    // System-font controls should disappear entirely in PNG mode.
    // (They are irrelevant and visually confusing when configuring PNG glyph inputs.)
    const bool file = m_btnFontFile != nullptr && m_btnFontFile->isChecked();
    const bool sys = !png;
    m_fontFileWidget->setVisible(file);
    ui->inputFontComboBoxFamily->setVisible(sys);
    ui->inputFontComboBoxStyle->setVisible(sys);
    ui->rowFontSize->setVisible(sys);
    ui->inputTextEditCharacters->setVisible(sys);
    ui->buttonAscii->setVisible(sys);
    ui->buttonNehe->setVisible(sys);
    ui->label->setVisible(sys);      // "Font:"
    ui->label_34->setVisible(sys);   // "Style:"
    ui->label_32->setVisible(sys);   // "Size:"
    ui->label_2->setVisible(sys);    // "Sprite Font Characters"

    // In font-file mode we keep the font family box visible but locked to the loaded family.
    ui->inputFontComboBoxFamily->setVisible(!png);
    ui->inputFontComboBoxFamily->setEnabled(!png && !file);
    ui->inputFontComboBoxStyle->setEnabled(!png);
    ui->rowFontSize->setEnabled(!png);
    ui->inputTextEditCharacters->setEnabled(!png);
    ui->buttonAscii->setEnabled(!png);
    ui->buttonNehe->setEnabled(!png);
}

void InputSettingsPanel::rebuildPngTable()
{
    m_pngTable->clearContents();
    m_pngTable->setRowCount(0);

    for ( const SGFPngGlyphSlot &slot : mValue.pngGlyphs ) {
        const int row = m_pngTable->rowCount();
        m_pngTable->insertRow(row);

        const QFont cellFont = m_pngTable->font();

        auto *ch = new QLineEdit;
        ch->setFont(cellFont);
        ch->setMaxLength(8);
        if ( !slot.character.isNull() ) {
            ch->setText(QString(slot.character));
        }
        QObject::connect(ch, &QLineEdit::textChanged, this, [this]() { emitValueChanged(); });

        auto *pe = new QLineEdit(slot.imagePath);
        pe->setFont(cellFont);
        QObject::connect(pe, &QLineEdit::textChanged, this, [this]() { emitValueChanged(); });

        auto *browse = new QPushButton(tr("…"));
        browse->setFont(cellFont);
        browse->setFixedWidth(28);
        QObject::connect(browse, &QPushButton::clicked, this, &InputSettingsPanel::onPngBrowseClicked);

        m_pngTable->setCellWidget(row, 0, ch);
        m_pngTable->setCellWidget(row, 1, pe);
        m_pngTable->setCellWidget(row, 2, browse);
    }
}

void InputSettingsPanel::readPngTableIntoSettings()
{
    mValue.pngGlyphs.clear();
    for ( int r = 0; r < m_pngTable->rowCount(); ++r ) {
        auto *ch = qobject_cast<QLineEdit *>(m_pngTable->cellWidget(r, 0));
        auto *pe = qobject_cast<QLineEdit *>(m_pngTable->cellWidget(r, 1));
        if ( ch == nullptr || pe == nullptr ) {
            continue;
        }
        const QString ct = ch->text();
        if ( ct.isEmpty() || pe->text().isEmpty() ) {
            continue;
        }
        SGFPngGlyphSlot s;
        s.character = ct.at(0);
        s.imagePath = pe->text();
        mValue.pngGlyphs.append(s);
    }
}

void InputSettingsPanel::emitValueChanged()
{
    if ( mEmitSignals ) {
        readPngTableIntoSettings();
        if ( m_btnPng != nullptr && m_btnPng->isChecked() ) {
            mValue.inputSource = SGFInputSource::PngSprites;
        } else if ( m_btnFontFile != nullptr && m_btnFontFile->isChecked() ) {
            mValue.inputSource = SGFInputSource::FontFile;
        } else {
            mValue.inputSource = SGFInputSource::SystemFont;
        }
        mValue.pngFontFaceName = m_pngFaceEdit->text();
        emit valueChanged(mValue);
    }
}

void InputSettingsPanel::onPngModeToggled(bool)
{
    updateSourceModeUi();
    if ( !mEmitSignals ) {
        return;
    }
    readPngTableIntoSettings();
    if ( m_btnPng != nullptr && m_btnPng->isChecked() ) {
        mValue.inputSource = SGFInputSource::PngSprites;
    } else if ( m_btnFontFile != nullptr && m_btnFontFile->isChecked() ) {
        mValue.inputSource = SGFInputSource::FontFile;
    } else {
        mValue.inputSource = SGFInputSource::SystemFont;
    }
    mValue.pngFontFaceName = m_pngFaceEdit->text();
    emit valueChanged(mValue);
}

void InputSettingsPanel::onPngFaceEdited(const QString &)
{
    if ( mEmitSignals ) {
        readPngTableIntoSettings();
        mValue.pngFontFaceName = m_pngFaceEdit->text();
        mValue.inputSource = SGFInputSource::PngSprites;
        emit valueChanged(mValue);
    }
}

void InputSettingsPanel::onPngAddRow()
{
    readPngTableIntoSettings();
    SGFPngGlyphSlot s;
    mValue.pngGlyphs.append(s);
    rebuildPngTable();
    m_pngTable->setCurrentCell(m_pngTable->rowCount() - 1, 0);
    emitValueChanged();
}

void InputSettingsPanel::onPngRemoveRow()
{
    readPngTableIntoSettings();
    const int r = m_pngTable->currentRow();
    if ( r < 0 || r >= mValue.pngGlyphs.size() ) {
        return;
    }
    mValue.pngGlyphs.removeAt(r);
    rebuildPngTable();
    emitValueChanged();
}

void InputSettingsPanel::onPngMoveUp()
{
    readPngTableIntoSettings();
    const int r = m_pngTable->currentRow();
    if ( r <= 0 ) {
        return;
    }
    mValue.pngGlyphs.swapItemsAt(r, r - 1);
    bool p = mEmitSignals;
    mEmitSignals = false;
    rebuildPngTable();
    m_pngTable->setCurrentCell(r - 1, 0);
    mEmitSignals = p;
    emitValueChanged();
}

void InputSettingsPanel::onPngMoveDown()
{
    readPngTableIntoSettings();
    const int r = m_pngTable->currentRow();
    if ( r < 0 || r >= mValue.pngGlyphs.size() - 1 ) {
        return;
    }
    mValue.pngGlyphs.swapItemsAt(r, r + 1);
    bool p = mEmitSignals;
    mEmitSignals = false;
    rebuildPngTable();
    m_pngTable->setCurrentCell(r + 1, 0);
    mEmitSignals = p;
    emitValueChanged();
}

void InputSettingsPanel::onPngBrowseClicked()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if ( btn == nullptr ) {
        return;
    }
    int row = -1;
    for ( int r = 0; r < m_pngTable->rowCount(); ++r ) {
        if ( m_pngTable->cellWidget(r, 2) == btn ) {
            row = r;
            break;
        }
    }
    if ( row < 0 ) {
        return;
    }
    const QString path = QFileDialog::getOpenFileName(this, tr("PNG image"), QString(),
        tr("Images (*.png *.webp *.bmp);;All files (*)"));
    if ( path.isEmpty() ) {
        return;
    }
    auto *pe = qobject_cast<QLineEdit *>(m_pngTable->cellWidget(row, 1));
    if ( pe != nullptr ) {
        pe->setText(path);
    }
}

void InputSettingsPanel::setValue(const SGFInputSettings & value)
{
    mValue = value;

    bool prevEmitSignals = mEmitSignals;
    mEmitSignals = false;

    if ( mValue.inputSource == SGFInputSource::PngSprites ) {
        if ( m_btnPng ) {
            m_btnPng->setChecked(true);
        }
    } else if ( mValue.inputSource == SGFInputSource::FontFile ) {
        if ( m_btnFontFile ) {
            m_btnFontFile->setChecked(true);
        }
    } else {
        if ( m_btnSystemFont ) {
            m_btnSystemFont->setChecked(true);
        }
    }

    m_pngFaceEdit->setText(mValue.pngFontFaceName.isEmpty() ? QStringLiteral("PNG Font") : mValue.pngFontFaceName);

    ui->inputFontComboBoxFamily->setCurrentText(mValue.fontFamily);
    fontFamilyChanged();

    for ( int i = 0; i < ui->inputFontComboBoxStyle->count(); ++i ) {
        if ( ui->inputFontComboBoxStyle->itemText(i).compare(mValue.fontStyle) == 0 ) {
            ui->inputFontComboBoxStyle->setCurrentIndex(i);
        }
    }
    mValue.fontStyle = ui->inputFontComboBoxStyle->currentText();

    ui->rowFontSize->setIntValue(static_cast<int>(mValue.fontSize));
    ui->inputTextEditCharacters->setText(mValue.characters);

    if ( m_fontFileNameLabel != nullptr ) {
        if ( mValue.inputSource == SGFInputSource::FontFile && !mValue.fontFamily.isEmpty() ) {
            m_fontFileNameLabel->setText(tr("Font: %1").arg(mValue.fontFamily));
        } else {
            m_fontFileNameLabel->setText(tr("Font: (drop a file below)"));
        }
    }

    rebuildPngTable();
    updateSourceModeUi();

    mEmitSignals = prevEmitSignals;
}


SGFInputSettings InputSettingsPanel::getValue() const
{
    InputSettingsPanel *self = const_cast<InputSettingsPanel *>(this);
    self->readPngTableIntoSettings();
    self->mValue.fontFamily = ui->inputFontComboBoxFamily->currentText();
    self->mValue.fontStyle = ui->inputFontComboBoxStyle->currentText();
    self->mValue.fontSize = static_cast<float>(ui->rowFontSize->intValue());
    self->mValue.characters = ui->inputTextEditCharacters->toPlainText();
    self->mValue.inputSource = (m_btnPng != nullptr && m_btnPng->isChecked()) ? SGFInputSource::PngSprites : SGFInputSource::SystemFont;
    self->mValue.pngFontFaceName = m_pngFaceEdit->text();
    return self->mValue;
}


void InputSettingsPanel::fontFamilyChanged()
{
    mValue.fontFamily = ui->inputFontComboBoxFamily->currentText();
    QStringList fontStyles = mFontDatabase.styles(mValue.fontFamily);

    bool prevEmitSignals = mEmitSignals;
    mEmitSignals = false;

    ui->inputFontComboBoxStyle->clear();
    ui->inputFontComboBoxStyle->addItems(fontStyles);
    ui->inputFontComboBoxStyle->setCurrentIndex(0);

    int initialIndex = fontStyles.indexOf(
        QRegularExpression(QStringLiteral("Regular"), QRegularExpression::CaseInsensitiveOption));

    if ( initialIndex > -1 ) {
        ui->inputFontComboBoxStyle->setCurrentIndex(initialIndex);
    }

    mEmitSignals = prevEmitSignals;

    if ( mEmitSignals ) {
        emit valueChanged(mValue);
    }
}


void InputSettingsPanel::fontStyleChanged()
{
    mValue.fontStyle = ui->inputFontComboBoxStyle->currentText();

    if ( mEmitSignals ) {
        emit valueChanged(mValue);
    }
}


void InputSettingsPanel::fontSizeChanged()
{
    mValue.fontSize = static_cast<float>(ui->rowFontSize->intValue());

    if ( mEmitSignals ) {
        emit valueChanged(mValue);
    }
}


void InputSettingsPanel::inputCharactersChanged()
{
    const QString raw = ui->inputTextEditCharacters->toPlainText();

    // Ensure characters are unique in the editor (e.g. typing "AAAA" keeps a single "A").
    // Keep first occurrence order so pasted character sets remain stable.
    QString uniq;
    uniq.reserve(raw.size());
    QSet<QChar> seen;
    seen.reserve(raw.size());

    // Preserve caret position as best-effort.
    const int oldPos = ui->inputTextEditCharacters->textCursor().position();
    int newPos = oldPos;

    for ( int i = 0; i < raw.size(); ++i )
    {
        const QChar c = raw[i];
        const bool isNew = !seen.contains(c);
        if ( isNew ) {
            seen.insert(c);
            uniq.append(c);
        } else {
            if ( i < oldPos ) {
                newPos = std::max(0, newPos - 1);
            }
        }
    }

    if ( uniq != raw ) {
        const bool prev = ui->inputTextEditCharacters->blockSignals(true);
        ui->inputTextEditCharacters->setPlainText(uniq);
        QTextCursor tc = ui->inputTextEditCharacters->textCursor();
        tc.setPosition(std::min(newPos, static_cast<int>(uniq.size())));
        ui->inputTextEditCharacters->setTextCursor(tc);
        ui->inputTextEditCharacters->blockSignals(prev);
    }

    mValue.characters = uniq;

    if ( mEmitSignals ) {
        emit valueChanged(mValue);
    }
}


void InputSettingsPanel::on_buttonAscii_clicked()
{
    ui->inputTextEditCharacters->setText(SGFInputSettings::kCharacterSetAscii);
}

void InputSettingsPanel::on_buttonNehe_clicked()
{
    ui->inputTextEditCharacters->setText(SGFInputSettings::kCharacterSetNehe);
}
