#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCheckBox>
#include <cmath>

#include "Model/Exporters/SGFFontExporters.h"
#include "Model/SGFFile/SGFFileWriter.h"
#include "Model/SGFFile/SGFFileReader.h"
#include "Model/Effects/SGFUnderlayEffect.h"
#include "Model/Effects/SGFOuterGlowEffect.h"
#include "Model/Effects/SGFInnerHighlightEffect.h"
#include "Model/Effects/SGFSparkleOverlayEffect.h"

#include "UI/Widgets/EffectListRow.h"

static const char * APP_TITLE = "Sprite Glypher";
static const char * APP_VERSION = "1.0.3";

#if defined(Q_OS_MAC)
    static const char * APP_PLATFORM = "OSX";
#elif defined(Q_OS_WIN)
    static const char * APP_PLATFORM = "Windows";
#elif defined(Q_OS_LINUX)
    static const char * APP_PLATFORM = "Linux";
#else
    static const char * APP_PLATFORM = "Unknown";
#endif

MainWindow::MainWindow(QWidget *parent, const QString &initialFile) :
    QMainWindow(parent),
    mRenderGlyphRects(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeGui();

    if ( !initialFile.isEmpty() ) {
        if ( !openDocumentFromPath(initialFile) ) {
            loadNewDocument();
        }
    } else {
        loadNewDocument();
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if ( mDocument->needsSaving() )
    {
        const QMessageBox::StandardButton result = QMessageBox::question(
            this,
            tr("Save Changes?"),
            tr("Do you want to save the changes you made to this Sprite Font?"),
            QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,
            QMessageBox::Save);

        if ( result == QMessageBox::Discard )
        {
            event->accept();
        }
        else if ( result == QMessageBox::Cancel )
        {
            event->ignore();
        }
        else if ( result == QMessageBox::Save )
        {
            on_actionSave_triggered();
            event->accept();
        }
    }
    else
    {
        event->accept();
    }
}


bool MainWindow::openDocumentFromPath(const QString &path)
{
    if ( path.isEmpty() || !QFileInfo::exists(path) ) {
        return false;
    }

    SGFFileReader fileReader;
    SGFDocument::Ptr loadedDocument = fileReader.loadDocument(path);

    if ( loadedDocument != nullptr ) {
        setDocument(loadedDocument);
        return true;
    }

    QMessageBox::warning(this, tr("Open"), tr("Could not open file:\n%1\n\n%2").arg(path, fileReader.lastError()));
    return false;
}

void MainWindow::loadNewDocument()
{
    SGFDocument::Ptr document = SGFDocument::Ptr(new SGFDocument());
    document->initDefaultDocument();
    setDocument(document);
}


void MainWindow::setDocument(SGFDocument::Ptr document)
{
    mDocument = document;

    recreateGuiFromDocument();
    updateGuiFromDocument();
    updateAtlasImage();
}


void MainWindow::initializeGui()
{
    setWindowTitle(QString("%1 - %2 (%3)").arg(APP_TITLE, APP_VERSION, APP_PLATFORM));
    mSuppressAtlasUpdate = true;

    menuBar()->setVisible(false);

    ui->atlasGraphicsView->setBackgroundBrush(QBrush(QColor(100,100,100,255)));
    ui->atlasGraphicsView->setScene(&mGraphicsScene);

    mCurrentZoom = 1.0f;
    mRetinaEnabled = true;

    QObject::connect(ui->inputSettingsPanel, SIGNAL(valueChanged(SGFInputSettings)), this, SLOT(updateDocumentFromGui()));
    QObject::connect(ui->generationSettingsPanel, SIGNAL(valueChanged(SGFGenerationSettings)), this, SLOT(updateDocumentFromGui()));
    QObject::connect(ui->generationSettingsPanel, SIGNAL(showGlyphBoundsChanged(bool)), this, SLOT(on_generationGlyphBoundsChanged(bool)));
    QObject::connect(ui->exportSettingsPanel, SIGNAL(valueChanged(SGFExportSettings)), this, SLOT(updateDocumentFromGui()));
    QObject::connect(ui->listWidgetEffects, SIGNAL(itemSelectionChanged()), this, SLOT(selectedEffectChanged()));
    QObject::connect(ui->listWidgetEffects->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(effectListRowsMoved(QModelIndex,int,int,QModelIndex,int)));

    QMenu *effectsMenu = new QMenu(this);
    effectsMenu->addAction(tr("Add Fill"), this, &MainWindow::on_actionAdd_Fill_triggered);
    effectsMenu->addAction(tr("Add Shadow"), this, &MainWindow::on_actionAdd_Shadow_triggered);
    effectsMenu->addAction(tr("Add Underlay"), this, &MainWindow::on_actionAdd_Underlay_triggered);
    effectsMenu->addAction(tr("Add Outer Glow"), this, &MainWindow::on_actionAdd_Outer_Glow_triggered);
    effectsMenu->addAction(tr("Add Inner Highlight"), this, &MainWindow::on_actionAdd_Inner_Highlight_triggered);
    effectsMenu->addAction(tr("Add Sparkle Overlay"), this, &MainWindow::on_actionAdd_Sparkle_Overlay_triggered);
    effectsMenu->addAction(tr("Add Stroke"), this, &MainWindow::on_actionAdd_Stroke_triggered);
    effectsMenu->addAction(tr("Add Shaded Material"), this, &MainWindow::on_actionAdd_Shaded_Material_triggered);
    effectsMenu->addSeparator();
    effectsMenu->addAction(tr("Duplicate Effect"), this, &MainWindow::on_actionDuplicate_Selected_Effect_triggered);
    m_effectsHeaderDeleteAction = effectsMenu->addAction(tr("Delete Effect"), this, &MainWindow::on_actionDelete_Selected_Effect_triggered);
    ui->toolButtonEffectsMenu->setMenu(effectsMenu);
    ui->toolButtonEffectsMenu->setPopupMode(QToolButton::InstantPopup);
    updateEffectsHeaderMenuState();

    ui->generationSettingsPanel->setShowGlyphBounds(mRenderGlyphRects);

    mSuppressAtlasUpdate = false;
}


void MainWindow::recreateGuiFromDocument()
{
    mSuppressAtlasUpdate = true;

    // Clear Existing UI
    while( ui->stackedEffects->count() > 0 ) {
        ui->stackedEffects->removeWidget(ui->stackedEffects->widget(0));
    }

    mEffectPanels.clear();
    mEffectListRows.clear();
    ui->listWidgetEffects->clear();

    // Title rows: mEffectListRows[i] stays aligned with document index i; list order is reversed so the
    // top row is the topmost layer (last paint pass).
    const int effectCount = mDocument->getEffectCount();
    mEffectListRows.reserve(effectCount);
    for ( int i = 0; i < effectCount; ++i )
    {
        SGFEffect * effect = mDocument->getEffectAtIndex(i).get();
        EffectListRow * row = new EffectListRow(this, effect->getTitle());
        QObject::connect(row, SIGNAL(checkedStateChanged(bool)), this, SLOT(updateDocumentFromGui()));
        QObject::connect(row, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(effectListRowContextMenu(QPoint)));
        mEffectListRows.push_back(row);
    }
    for ( int i = effectCount - 1; i >= 0; --i )
    {
        EffectListRow * row = mEffectListRows[i];
        QListWidgetItem * item = new QListWidgetItem();
        ui->listWidgetEffects->addItem(item);
        item->setData(Qt::UserRole, i);
        item->setSizeHint(row->minimumSize());
        ui->listWidgetEffects->setItemWidget(item, row);
    }

    // Effect Panels
    for( int i = 0; i < mDocument->getEffectCount(); ++i )
    {
        SGFEffect * effect = mDocument->getEffectAtIndex(i).get();
        SGFEffectType effectType = effect->getEffectType();
        QWidget * panel = nullptr;

        if ( effectType == SGFEffectType::Effect_Fill )
        {
            FillEffectSettingsPanel * fillPanel = new FillEffectSettingsPanel(this);
            QObject::connect(fillPanel, SIGNAL(valueChanged(SGFFillEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = fillPanel;
        }
        else if ( effectType == SGFEffectType::Effect_Shadow )
        {
            ShadowEffectSettingsPanel * shadowPanel = new ShadowEffectSettingsPanel(this);
            QObject::connect(shadowPanel, SIGNAL(valueChanged(SGFShadowEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = shadowPanel;
        }
        else if ( effectType == SGFEffectType::Effect_Underlay )
        {
            UnderlayEffectSettingsPanel * underlayPanel = new UnderlayEffectSettingsPanel(this);
            QObject::connect(underlayPanel, SIGNAL(valueChanged(SGFUnderlayEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = underlayPanel;
        }
        else if ( effectType == SGFEffectType::Effect_OuterGlow )
        {
            OuterGlowEffectSettingsPanel * glowPanel = new OuterGlowEffectSettingsPanel(this);
            QObject::connect(glowPanel, SIGNAL(valueChanged(SGFOuterGlowEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = glowPanel;
        }
        else if ( effectType == SGFEffectType::Effect_InnerHighlight )
        {
            InnerHighlightEffectSettingsPanel * hiPanel = new InnerHighlightEffectSettingsPanel(this);
            QObject::connect(hiPanel, SIGNAL(valueChanged(SGFInnerHighlightEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = hiPanel;
        }
        else if ( effectType == SGFEffectType::Effect_SparkleOverlay )
        {
            SparkleOverlayEffectSettingsPanel * spPanel = new SparkleOverlayEffectSettingsPanel(this);
            QObject::connect(spPanel, SIGNAL(valueChanged(SGFSparkleOverlayEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = spPanel;
        }
        else if ( effectType == SGFEffectType::Effect_Stroke )
        {
            StrokeEffectSettingsPanel * strokePanel = new StrokeEffectSettingsPanel(this);
            QObject::connect(strokePanel, SIGNAL(valueChanged(SGFStrokeEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = strokePanel;
        }
        else if ( effectType == SGFEffectType::Effect_ShadedMaterial )
        {
            ShadedMaterialEffectSettingsPanel * shadedPanel = new ShadedMaterialEffectSettingsPanel(this);
            QObject::connect(shadedPanel, SIGNAL(valueChanged(SGFShadedMaterialEffectSettings)), this, SLOT(updateDocumentFromGui()));
            panel = shadedPanel;
        }

        if ( panel != nullptr )
        {
            ui->stackedEffects->addWidget(panel);
            mEffectPanels.push_back(panel);
        }
    }

    updateGuiFromDocument();
    updateAtlasImage();

    updateEffectsHeaderMenuState();

    mSuppressAtlasUpdate = false;
}


void MainWindow::updateAtlasImage()
{
    SGFSpriteFont spriteFont = mDocument->getSpriteFont();
    QPixmap pixmap = QPixmap::fromImage(spriteFont.textureAtlas);
    int pixelRatio = 1;

    if ( mRetinaEnabled )
    {
        pixelRatio = qApp->devicePixelRatio();
        pixmap.setDevicePixelRatio(pixelRatio);
    }

    mGraphicsScene.clear();
    const qreal pw = pixmap.width() / pixelRatio;
    const qreal ph = pixmap.height() / pixelRatio;
    mGraphicsScene.setSceneRect(-10, -10, pw + 20, ph + 20);
    const QRectF atlasRect(0, 0, pw, ph);
    mGraphicsScene.addRect(atlasRect, QPen(Qt::NoPen), atlasBackgroundBrush());
    const QColor previewBackdrop = mDocument->getGenerationSettings().color;
    if ( previewBackdrop.alpha() > 0 ) {
        mGraphicsScene.addRect(atlasRect, QPen(Qt::NoPen), QBrush(previewBackdrop));
    }
    mGraphicsScene.addRect(atlasRect, QPen(QColor(0, 0, 0, 255), 1.0, Qt::DotLine), QBrush(Qt::NoBrush));
    mGraphicsScene.addPixmap(pixmap);

    if ( mRenderGlyphRects )
    {
        for( SGFGlyph & glyph : spriteFont.glyphs )
        {
            QRect rect = glyph.atlasRect;
            rect = QRect((rect.x() / pixelRatio), (rect.y() / pixelRatio), (rect.width() / pixelRatio), (rect.height() / pixelRatio));
            mGraphicsScene.addRect(rect, QPen(QColor(255,0,0,180)), QBrush(QColor(0,0,0,0)));
        }
    }

    ui->atlasGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->atlasGraphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
    ui->atlasGraphicsView->setTransform(QTransform().scale(mCurrentZoom, mCurrentZoom));

    ui->labelAtlasSize->setText(QString("(%1,%2)").arg(QString::number(spriteFont.textureAtlas.width()), QString::number(spriteFont.textureAtlas.height())));
    ui->labelZoom->setText(QString("%1%").arg(QString::number((int)(mCurrentZoom * 100))));
    if ( ui->zoomSlider != nullptr ) {
        const int z = qBound(25, static_cast<int>(std::round(mCurrentZoom * 100.0f)), 400);
        const bool old = ui->zoomSlider->blockSignals(true);
        ui->zoomSlider->setValue(z);
        ui->zoomSlider->blockSignals(old);
        ui->zoomSlider->setToolTip(spriteFont.doGlyphsFit
            ? QString()
            : QStringLiteral("Glyphs do not fit on atlas!"));
    }

    updateEffectRowPreviews();

    return;
}


void MainWindow::updateEffectRowPreviews()
{
    for( int i = 0; i < mDocument->getEffectCount(); ++i )
    {
        SGFEffect * effect = mDocument->getEffectAtIndex(i).get();

        mEffectListRows[i]->setTitle(effect->getTitle());
        mEffectListRows[i]->setIcon(effect->getPreview(24, 24, 24));
    }
}


void MainWindow::updateGuiFromDocument()
{
    mSuppressAtlasUpdate = true;

    if ( mDocument->getEffectCount() != mEffectPanels.count() ) {
        return;
    }

    ui->inputSettingsPanel->setValue(mDocument->getInputSettings());
    ui->generationSettingsPanel->setValue(mDocument->getGenerationSettings());
    ui->exportSettingsPanel->setValue(mDocument->getExportSettings());

    for( int i = 0; i < mDocument->getEffectCount(); ++i )
    {
        SGFEffect * effect = mDocument->getEffectAtIndex(i).get();
        SGFEffectType effectType = effect->getEffectType();

        mEffectListRows[i]->setChecked(effect->isEnabled());
        mEffectListRows[i]->setTitle(effect->getTitle());
        mEffectListRows[i]->setIcon(effect->getPreview(24, 24, 18));

        if ( effectType == SGFEffectType::Effect_Fill )
        {
            SGFFillEffectSettings settings = dynamic_cast<SGFFillEffect *>(effect)->getSettings();
            FillEffectSettingsPanel * panel = dynamic_cast<FillEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Shadow )
        {
            SGFShadowEffectSettings settings = dynamic_cast<SGFShadowEffect *>(effect)->getSettings();
            ShadowEffectSettingsPanel * panel = dynamic_cast<ShadowEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Underlay )
        {
            SGFUnderlayEffectSettings settings = dynamic_cast<SGFUnderlayEffect *>(effect)->getSettings();
            UnderlayEffectSettingsPanel * panel = dynamic_cast<UnderlayEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_OuterGlow )
        {
            SGFOuterGlowEffectSettings settings = dynamic_cast<SGFOuterGlowEffect *>(effect)->getSettings();
            OuterGlowEffectSettingsPanel * panel = dynamic_cast<OuterGlowEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_InnerHighlight )
        {
            SGFInnerHighlightEffectSettings settings = dynamic_cast<SGFInnerHighlightEffect *>(effect)->getSettings();
            InnerHighlightEffectSettingsPanel * panel = dynamic_cast<InnerHighlightEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_SparkleOverlay )
        {
            SGFSparkleOverlayEffectSettings settings = dynamic_cast<SGFSparkleOverlayEffect *>(effect)->getSettings();
            SparkleOverlayEffectSettingsPanel * panel = dynamic_cast<SparkleOverlayEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Stroke )
        {
            SGFStrokeEffectSettings settings = dynamic_cast<SGFStrokeEffect *>(effect)->getSettings();
            StrokeEffectSettingsPanel * panel = dynamic_cast<StrokeEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
        else if ( effectType == SGFEffectType::Effect_ShadedMaterial )
        {
            SGFShadedMaterialEffectSettings settings = dynamic_cast<SGFShadedMaterialEffect *>(effect)->getSettings();
            ShadedMaterialEffectSettingsPanel * panel = dynamic_cast<ShadedMaterialEffectSettingsPanel *>(mEffectPanels[i]);
            panel->setValue(settings);
        }
    }

    mSuppressAtlasUpdate = false;
}


void MainWindow::updateDocumentFromGui()
{
    if ( mSuppressAtlasUpdate ) {
        return;
    }

    if ( mDocument->getEffectCount() != mEffectPanels.count() ) {
        return;
    }

    mDocument->setInputSettings(ui->inputSettingsPanel->getValue());
    mDocument->setGenerationSettings(ui->generationSettingsPanel->getValue());
    mDocument->setExportSettings(ui->exportSettingsPanel->getValue());

    for( int i = 0; i < mDocument->getEffectCount(); ++i )
    {
        SGFEffect * effect = mDocument->getEffectAtIndex(i).get();
        SGFEffectType effectType = effect->getEffectType();

        if ( effectType == SGFEffectType::Effect_Fill )
        {
            FillEffectSettingsPanel * panel = dynamic_cast<FillEffectSettingsPanel *>(mEffectPanels[i]);
            SGFFillEffectSettings settings = panel->getValue();

            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFFillEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Shadow )
        {
            ShadowEffectSettingsPanel * panel = dynamic_cast<ShadowEffectSettingsPanel *>(mEffectPanels[i]);
            SGFShadowEffectSettings settings = panel->getValue();

            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFShadowEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Underlay )
        {
            UnderlayEffectSettingsPanel * panel = dynamic_cast<UnderlayEffectSettingsPanel *>(mEffectPanels[i]);
            SGFUnderlayEffectSettings settings = panel->getValue();

            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFUnderlayEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_OuterGlow )
        {
            OuterGlowEffectSettingsPanel * panel = dynamic_cast<OuterGlowEffectSettingsPanel *>(mEffectPanels[i]);
            SGFOuterGlowEffectSettings settings = panel->getValue();
            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFOuterGlowEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_InnerHighlight )
        {
            InnerHighlightEffectSettingsPanel * panel = dynamic_cast<InnerHighlightEffectSettingsPanel *>(mEffectPanels[i]);
            SGFInnerHighlightEffectSettings settings = panel->getValue();
            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFInnerHighlightEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_SparkleOverlay )
        {
            SparkleOverlayEffectSettingsPanel * panel = dynamic_cast<SparkleOverlayEffectSettingsPanel *>(mEffectPanels[i]);
            SGFSparkleOverlayEffectSettings settings = panel->getValue();
            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFSparkleOverlayEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_Stroke )
        {
            StrokeEffectSettingsPanel * panel = dynamic_cast<StrokeEffectSettingsPanel *>(mEffectPanels[i]);
            SGFStrokeEffectSettings settings = panel->getValue();

            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFStrokeEffect *>(effect)->setSettings(settings);
        }
        else if ( effectType == SGFEffectType::Effect_ShadedMaterial )
        {
            ShadedMaterialEffectSettingsPanel * panel = dynamic_cast<ShadedMaterialEffectSettingsPanel *>(mEffectPanels[i]);
            SGFShadedMaterialEffectSettings settings = panel->getValue();

            settings.enabled = mEffectListRows[i]->isChecked();
            dynamic_cast<SGFShadedMaterialEffect *>(effect)->setSettings(settings);
        }
    }

    updateAtlasImage();
}


void MainWindow::selectedEffectChanged()
{
    const int index = selectedEffectIndex();
    if ( index < 0 || index >= mDocument->getEffectCount() ) {
        return;
    }
    ui->labelSelectedEffect->setText(mDocument->getEffectAtIndex(index)->getTitle());
    ui->stackedEffects->setCurrentIndex(index);
    updateEffectsHeaderMenuState();
}


void MainWindow::effectListRowsMoved(const QModelIndex & /*sourceParent*/, int /*sourceStart*/, int /*sourceEnd*/, const QModelIndex & /*destinationParent*/, int /*destinationRow*/)
{
    const int n = mDocument->getEffectCount();
    if ( n <= 0 ) {
        return;
    }

    QVector<SGFEffect::Ptr> newOrder;
    newOrder.reserve(n);
    for ( int visualRow = n - 1; visualRow >= 0; --visualRow )
    {
        QListWidgetItem * item = ui->listWidgetEffects->item(visualRow);
        if ( item == nullptr ) {
            return;
        }
        const int docIdx = item->data(Qt::UserRole).toInt();
        newOrder.append(mDocument->getEffectAtIndex(docIdx));
    }

    mDocument->reorderEffects(newOrder);
    recreateGuiFromDocument();
}


void MainWindow::effectListRowContextMenu(const QPoint & pos)
{
    QMenu menu;
    menu.addAction(tr("Delete Effect"), this, &MainWindow::on_actionDelete_Selected_Effect_triggered);
    menu.addAction(tr("Duplicate Effect"), this, &MainWindow::on_actionDuplicate_Selected_Effect_triggered);

    QListWidgetItem * cur = ui->listWidgetEffects->currentItem();
    QWidget * widget = cur != nullptr ? ui->listWidgetEffects->itemWidget(cur) : nullptr;
    if ( widget == nullptr ) {
        return;
    }
    menu.exec(widget->mapToGlobal(pos));
}


void MainWindow::updateEffectsHeaderMenuState()
{
    const bool hasSelection = ui->listWidgetEffects->count() > 0
        && ui->listWidgetEffects->currentRow() >= 0;
    if ( m_effectsHeaderDeleteAction != nullptr ) {
        m_effectsHeaderDeleteAction->setEnabled(hasSelection);
    }
}


int MainWindow::selectedEffectIndex()
{
    QListWidgetItem * item = ui->listWidgetEffects->currentItem();
    if ( item == nullptr ) {
        return -1;
    }
    return item->data(Qt::UserRole).toInt();
}


QBrush MainWindow::atlasBackgroundBrush()
{
    const int cell = 17;
    QPixmap pm(cell * 2, cell * 2);
    QPainter pmp(&pm);
    const QColor light(245, 245, 245, 255);
    const QColor dark(230, 230, 230, 255);
    pmp.fillRect(0, 0, cell, cell, light);
    pmp.fillRect(cell, cell, cell, cell, light);
    pmp.fillRect(0, cell, cell, cell, dark);
    pmp.fillRect(cell, 0, cell, cell, dark);
    pmp.end();

    return QBrush(pm);
}


void MainWindow::on_actionExport_PNG_triggered()
{
    SGFSpriteFont spriteFont = mDocument->getSpriteFont();

    if ( spriteFont.isNull || spriteFont.textureAtlas.isNull() ) {
        QMessageBox::information(this, "No Sprite Atlas", "No Sprite Atlas available for export", QMessageBox::Ok);
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Export PNG");

    if ( !path.isNull() )
    {
        SGFFontExporterPNG exporter;
        exporter.exportFont(mDocument.get());
    }
}


void MainWindow::on_actionToggle_Retina_Support_triggered()
{
    mRetinaEnabled = !mRetinaEnabled;
    updateAtlasImage();
}


void MainWindow::on_toolButtonZoomIn_clicked()
{
    mCurrentZoom += 0.25f;
    updateAtlasImage();
}


void MainWindow::on_toolButtonZoomOut_clicked()
{
    mCurrentZoom -= 0.25f;

    if ( mCurrentZoom < 0.25f ) {
        mCurrentZoom = 0.25f;
    }

    updateAtlasImage();
}


void MainWindow::on_toolButtonZoomReset_clicked()
{
    mCurrentZoom = 1.0f;
    updateAtlasImage();
}

void MainWindow::on_zoomSlider_valueChanged(int value)
{
    // Slider uses percent 25..400
    const float z = static_cast<float>(qBound(25, value, 400)) / 100.0f;
    if ( std::abs(mCurrentZoom - z) < 0.0001f ) {
        return;
    }
    mCurrentZoom = z;
    updateAtlasImage();
}


void MainWindow::on_actionSave_triggered()
{
    QString path = mDocument->documentPath();

    if ( path.isEmpty() || !QFileInfo(path).exists() )
    {
        on_actionSaveAs_triggered();
        return;
    }

    SGFFileWriter fileWriter(path);
    fileWriter.writeDocument(mDocument.get());
}

void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Sprite Glypher File (*.sgf)"));

    if ( !path.isEmpty() ) {
        openDocumentFromPath(path);
    }
}

void MainWindow::on_actionNew_triggered()
{
    if ( mDocument->needsSaving() )
    {
        const QMessageBox::StandardButton button = QMessageBox::question(
            this,
            tr("Are you sure?"),
            tr("Any changes made to the current document will be lost. Are you sure?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

        if ( button == QMessageBox::No ) {
            return;
        }
    }

    loadNewDocument();
}

void MainWindow::on_actionAdd_Fill_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFFillEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Shadow_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFShadowEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Underlay_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFUnderlayEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Outer_Glow_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFOuterGlowEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Inner_Highlight_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFInnerHighlightEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Sparkle_Overlay_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFSparkleOverlayEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Stroke_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFStrokeEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionAdd_Shaded_Material_triggered()
{
    mDocument->addEffect(SGFEffect::Ptr(new SGFShadedMaterialEffect()));
    recreateGuiFromDocument();
}

void MainWindow::on_actionDelete_Selected_Effect_triggered()
{
    int effectIndex = selectedEffectIndex();

    if ( effectIndex < 0 ) {
        QMessageBox::warning(this, "Please Select an Effect", "Please select a fill, shadow or stroke effect from the left hand panel by clicking on that effect's title bar.");
        return;
    }

    ui->listWidgetEffects->setCurrentIndex(ui->listWidgetEffects->model()->index(0,0));
    mDocument->removeEffect(effectIndex);
    recreateGuiFromDocument();
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), tr("Sprite Glypher File (*.sgf)"));

    if ( !path.isNull() )
    {
        SGFFileWriter fileWriter(path);
        fileWriter.writeDocument(mDocument.get());
    }
}

void MainWindow::on_actionDuplicate_Selected_Effect_triggered()
{
    int effectIndex = selectedEffectIndex();

    if ( effectIndex < 0 ) {
        QMessageBox::warning(this, "Please Select an Effect", "Please select a fill, shadow or stroke effect from the left hand panel by clicking on that effect's title bar.");
        return;
    }

    SGFEffect::Ptr effect = mDocument->getEffectAtIndex(effectIndex);
    SGFEffect::Ptr effectCopy = effect->clone();
    mDocument->insertEffect(effectIndex+1, effectCopy);
    recreateGuiFromDocument();
}

void MainWindow::on_actionExport_triggered()
{
    SGFExportSettings exportSettings = mDocument->getExportSettings();

    if ( exportSettings.path.isNull() ) {
        QMessageBox::information(this, tr("Set Export Path"), tr("You must set an export path in the 'Export Settings Panel' before attempting to export a file."), QMessageBox::Ok);
        return;
    }

    SGFSpriteFont spriteFont = mDocument->getSpriteFont();

    if ( spriteFont.isNull || spriteFont.textureAtlas.isNull() ) {
        QMessageBox::information(this, tr("No Sprite Atlas"), tr("No Sprite Atlas available for export"), QMessageBox::Ok);
        return;
    }

    SGFFontExporter::Ptr exporter = SGFFontExporter::FontExporterForSettings(exportSettings);

    if ( exporter == nullptr ) {
        QMessageBox::information(this, tr("No Valid Exporter"), tr("Could not find valid exporter for this file type. Sorry!"), QMessageBox::Ok);
        return;
    }

    exporter->exportFont(mDocument.get());
}

void MainWindow::on_actionToggle_Glyph_Outlines_triggered()
{
    mRenderGlyphRects = !mRenderGlyphRects;
    updateAtlasImage();
    ui->generationSettingsPanel->setShowGlyphBounds(mRenderGlyphRects);
}


void MainWindow::on_generationGlyphBoundsChanged(bool on)
{
    mRenderGlyphRects = on;
    updateAtlasImage();
}
