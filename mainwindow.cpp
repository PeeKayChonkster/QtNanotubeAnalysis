#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tools.hpp"
#include "mygraphicsview.h"
#include "configwindow.h"
#include <nameof.hpp>
#include <QFileDialog>
#include <QPixmap>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <functional>
#include <QtConcurrent/QtConcurrent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QGraphicsProxyWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      console(this),
      autoAnalysisConfig(this),
      manualAnalysisConfig(this),
      coordLabel(this),
      toolLabel(this),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("NanotubeAnalysis");

    connect(&futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::sl_worker_finished);
    connect(&analyzer, &nano::Analyzer::si_progress_changed, this, &MainWindow::sl_progress_changed);

    tools::init(this);



    ui->graphicsView->verticalScrollBar()->installEventFilter(this);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setScene(&scene);

    rulerLinePen.setColor(rulerLineColor);
    rulerLinePen.setWidth(rulerLIneWidth);
    rulerLinePen.setCapStyle(Qt::RoundCap);

    ui->toolsGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    coordLabel.setAlignment(Qt::AlignRight);
    toolLabel.setText("Active tool: None");
    statusBar()->addPermanentWidget(&toolLabel);
    statusBar()->addPermanentWidget(&coordLabel, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete progressDialog;
}

void MainWindow::startAutoAnalysis()
{
    console.show();
    console.print();    scene.update();
    console.print("<<<<< Starting auto analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this]() -> void {
        analyzer.startExtremumAnalysis();
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::startManualAnalysis(float threshold)
{
    console.show();
    console.print();
    console.print("<<<<< Starting manual analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this, threshold]() -> void {
        analyzer.startManualAnalysis(threshold);
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::setBrushRadius(float radius)
{
    if(radius > 1.0f)
    {
        brushRadius = radius;
        if(brushEllipseItem)
        {
            scene.removeItem(brushEllipseItem);
            delete brushEllipseItem;
            float brushDiameter = brushRadius * 2.0f;
            brushEllipseItem = scene.addEllipse(0.0f, 0.0f, brushDiameter, brushDiameter, QPen(brushCursorColor, brushDiameter * brushCursorCircleWidthFactor));
            brushEllipseItem->setZValue(1.0f);
        }
    }
}

float MainWindow::getBrushRadius() const
{
    return brushRadius;
}

void MainWindow::setPixelSize(float size)
{
    if(size > 0.0f) analyzer.pixelSize_nm = size;
}

float MainWindow::getPixelSize() const
{
    return analyzer.pixelSize_nm;
}

void MainWindow::setExtremumDeltaStep(float value)
{
    analyzer.extremumDeltaStep = value;
}

float MainWindow::getExtremumDeltaStep() const
{
    return analyzer.extremumDeltaStep;
}

void MainWindow::setExtremumOverflowTolerance(uint16_t value)
{
    analyzer.extremumOverflowTolerance = value;
}

uint16_t MainWindow::getExtremumOverflowTolerance() const
{
    return analyzer.extremumOverflowTolerance;
}

void MainWindow::setProcessFullRange(bool value)
{
    analyzer.processFullRange = value;
}

bool MainWindow::getProcessFullRange() const
{
    return analyzer.processFullRange;
}

void MainWindow::setMinPixelInTube(uint16_t value)
{
    analyzer.minPixelsInTube = value;
}

uint16_t MainWindow::getMinPixelInTube() const
{
    return analyzer.minPixelsInTube;
}

void MainWindow::on_actionOpen_image_triggered()
{
//    QString fileName = QFileDialog::getOpenFileName(this, "Choose image file", ".", "Image file (*.png *.jpg)");
//    currImg.load(fileName);
//    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
//    renderImages();
//    analyzer.setTargetImg(&currImg);
//    setMask();
//    setTubeMask();
//    resize(currImg.width(), currImg.height());
//    scene.setSceneRect(0.0f, 0.0f, currImg.width(), currImg.height());
//    tools::print("Loaded image file: " + fileName);

    // DEBUG //
    fastOpenImage();
    ui->graphicsView->show();
}

void MainWindow::fastOpenImage()
{
    currImg.load("../QNanotubeAnalysis/res/img/SamplesJPEG/S1-ZnAg_02.jpg");
    tools::print(std::string("Loaded image file: ") + "../QNanotubeAnalysis/res/img/SamplesJPEG/S1-ZnAg_02.jpg");
    tools::print(std::string("Image size: " + std::to_string(currImg.width()) + "x" + std::to_string(currImg.height())));
    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
    analyzer.setTargetImg(&currImg);
    mask = analyzer.getMask();
    tubeMask = analyzer.getTubeMask();
    renderImages();
    resize(currImg.width(), currImg.height());
    scene.setSceneRect(0.0f, 0.0f, currImg.width(), currImg.height());
}

void MainWindow::clearGraphicsView()
{
    if(!currImg.isNull())
    {
        scene.clear();
        currImgPixmapItem = nullptr;
        currImg = QImage();
        mask = nullptr;
        tubeMask = nullptr;
        analyzer.resetAll();
        clearAllRulerLines();
    }

}

void MainWindow::startProgressDialog()
{
    if(!progressDialog)
    {
        progressDialog = new QProgressDialog("Calculating", "Cancel", 0, 100, this);
        progressDialog->setMinimumDuration(0);
        progressDialog->setMinimumWidth(400);
        progressDialog->setFixedSize(progressDialog->size());
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setAutoClose(false);
        progressDialog->setAutoReset(false);
        connect(progressDialog, &QProgressDialog::canceled, this, &MainWindow::sl_analysis_canceled);
    }
    QSize screenSize = QApplication::primaryScreen()->availableSize();
    progressDialog->move((screenSize.width() - progressDialog->width()) / 2, (screenSize.height() - progressDialog->height()) / 2);
    progressDialog->show();
}

void MainWindow::addRulerPoint(QPoint point)
{
    if(currImg.isNull()) return;

    if(firstRulerLinePoint)
    {
        QLineF line(firstRulerLinePoint.value(), point);
        float lineRealLength = line.length() * analyzer.pixelSize_nm;
        QString unitOfMeasure = " nm";
        if(lineRealLength > 1000.0f)
        {
            unitOfMeasure = " µm";
            lineRealLength *= 0.001;
        }
        QLabel* label = new QLabel(QString::number(lineRealLength) + unitOfMeasure);
        label->setStyleSheet("background-color: " + rulerLabelBgColor);
        label->move(line.center().toPoint());
        QGraphicsLineItem* sceneLine = scene.addLine(line, rulerLinePen);
        QGraphicsProxyWidget* sceneWidget = scene.addWidget(label);
        sceneLine->setZValue(1.0f);
        sceneWidget->setZValue(1.0f);
        rulerLineItems.push_back(RulerPair(sceneLine, sceneWidget));
        firstRulerLinePoint = std::nullopt;
    }
    else
    {
        firstRulerLinePoint = point;
    }
}

void MainWindow::addTubeAtPos(QPoint pos)
{
    if(mask && tubeMask)
    {
        analyzer.addTubeAtPos(pos);
        renderImages();
    }
}

void MainWindow::removeTubeAtPos(QPoint pos)
{
    if(mask && tubeMask)
    {
        analyzer.removeTubeAtPos(pos);
        renderImages();
    }
}

void MainWindow::paintMaskAtPos(QPoint pos)
{
    if(!currImg.isNull())
    {
        analyzer.paintMaskAtPos(pos, brushRadius);
        renderImages();
    }
}

void MainWindow::eraseMaskAtPos(QPoint pos)
{
    if(!currImg.isNull())
    {
        analyzer.eraseMaskAtPos(pos, brushRadius);
        renderImages();
    }
}

void MainWindow::clearAllRulerLines()
{
    for(RulerPair& pair : rulerLineItems)
    {
        scene.removeItem(pair.first);
        delete pair.first;
        scene.removeItem(pair.second);
        delete pair.second;

    }
    rulerLineItems.clear();
}

void MainWindow::setActiveTool(Tool tool)
{
    activeTool = tool;
    std::string toolString = "Active tool: " + std::string(NAMEOF_ENUM(activeTool));
    toolLabel.setText(toolString.c_str());
}

void MainWindow::renderImages()
{
    if(!currImg.isNull() && currImgVisible)
    {
        if(currImgPixmapItem)
        {
            scene.removeItem(currImgPixmapItem);
            delete currImgPixmapItem;
        }
        currImgPixmapItem = scene.addPixmap(QPixmap::fromImage(currImg));
    }
    if(mask && maskVisible)
    {
        if(maskPixmapItem)
        {
            scene.removeItem(maskPixmapItem);
            delete maskPixmapItem;
        }
        maskPixmapItem = scene.addPixmap(QPixmap::fromImage(*mask));
    }
    if(tubeMask && tubeMaskVisible)
    {
        if(tubeMaskPixmapItem)
        {
            scene.removeItem(tubeMaskPixmapItem);
            delete tubeMaskPixmapItem;
        }
        tubeMaskPixmapItem = scene.addPixmap(QPixmap::fromImage(*tubeMask));
    }
}

void MainWindow::calculateMask(float threshold)
{
    analyzer.calculateMask(threshold);
    renderImages();
}

void MainWindow::on_actionShow_console_triggered()
{
    console.show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    console.close();
    autoAnalysisConfig.close();
    manualAnalysisConfig.close();
}

void MainWindow::mouseMoveEventGV(QMouseEvent *event)
{
    QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
    coordLabel.setText(QString("x:%1|y:%2").arg(scenePos.x(), 5).arg(scenePos.y(), 5));

    switch(activeTool)
    {
        case Tool::None:
            break;
        case Tool::Ruler:
        {
            static QGraphicsLineItem* tempRulerLineItem = nullptr;
            if(firstRulerLinePoint)
            {

                if(tempRulerLineItem)
                {
                    scene.removeItem(tempRulerLineItem);
                    delete tempRulerLineItem;
                }
                QLineF line(firstRulerLinePoint.value(), scenePos);
                tempRulerLineItem = scene.addLine(line, rulerLinePen);
            }
            else if(tempRulerLineItem)
            {
                scene.removeItem(tempRulerLineItem);
                delete tempRulerLineItem;
                tempRulerLineItem = nullptr;
            }
            break;
        }
        case Tool::TubeAdder:
            break;
        case Tool::TubeRemover:
            break;
        case Tool::MaskBrush:
        {
            QPointF brushPos(scenePos.x() - brushRadius, scenePos.y() - brushRadius);
            brushEllipseItem->setPos(brushPos);
            if(pressingActionButtonGV)
            {
                paintMaskAtPos(scenePos.toPoint());
            }
            break;
        }

        case Tool::MaskEraser:
        {
            QPointF brushPos(scenePos.x() - brushRadius, scenePos.y() - brushRadius);
            brushEllipseItem->setPos(brushPos);
            if(pressingActionButtonGV)
            {
                eraseMaskAtPos(scenePos.toPoint());
            }
            break;
        }
    }
}

void MainWindow::mousePressEventGV(QMouseEvent *event)
{
    if(event->button() == actionButton)
    {
        pressingActionButtonGV = true;
        QPoint scenePos = ui->graphicsView->mapToScene(event->pos()).toPoint();
        switch(activeTool)
        {
            case Tool::None:
                break;
            case Tool::Ruler:
                addRulerPoint(scenePos);
                break;
            case Tool::TubeAdder:
                addTubeAtPos(scenePos);
                break;
            case Tool::TubeRemover:
                removeTubeAtPos(scenePos);
                break;
            case Tool::MaskBrush:
                paintMaskAtPos(scenePos);
                break;
            case Tool::MaskEraser:
                eraseMaskAtPos(scenePos);
                break;
        }
    }
}

void MainWindow::mouseReleaseEventGV(QMouseEvent *event)
{
    if(event->button() == actionButton)
    {
        pressingActionButtonGV = false;
    }
}

void MainWindow::on_actionStart_extremum_analysis_triggered()
{
    if(!currImg.isNull())
    {
        autoAnalysisConfig.exec();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}

void MainWindow::on_actionStart_manual_analysis_triggered()
{
    if(!currImg.isNull())
    {
        manualAnalysisConfig.show();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}

void MainWindow::sl_progress_changed(int progress)
{
    progressDialog->setValue(progress);
}

void MainWindow::sl_analysis_canceled()
{
    qDebug("Here");
    analyzer.cancelAnalysis();
}

void MainWindow::sl_worker_finished()
{
    futureWatcher.waitForFinished();
    progressDialog->hide();
    renderImages();
}


void MainWindow::on_actionRuler_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::Ruler : Tool::None);
}


void MainWindow::on_actionClear_all_ruler_lines_triggered()
{
    clearAllRulerLines();
}

void MainWindow::on_actionTubeAdder_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::TubeAdder : Tool::None);
}


void MainWindow::on_actionMaskBrush_toggled(bool arg1)
{
    if(arg1 && !currImg.isNull())
    {
        activeTool = Tool::MaskBrush;
        if(!brushEllipseItem)
        {
            float brushDiameter = brushRadius * 2.0f;
            brushEllipseItem = scene.addEllipse(0.0f, 0.0f, brushDiameter, brushDiameter, QPen(brushCursorColor, brushDiameter * brushCursorCircleWidthFactor));
            brushEllipseItem->setZValue(1.0f);
            brushEllipseItem->setTransformOriginPoint(brushRadius, brushRadius);
        }
    }
    else
    {
        if(brushEllipseItem)
        {
            scene.removeItem(brushEllipseItem);
            delete brushEllipseItem;
            brushEllipseItem = nullptr;
        }
    }
}

void MainWindow::on_actionMaskEraser_toggled(bool arg1)
{
    if(arg1 && !currImg.isNull())
    {
        activeTool = Tool::MaskEraser;
        if(!brushEllipseItem)
        {
            float brushDiameter = brushRadius * 2.0f;
            brushEllipseItem = scene.addEllipse(0.0f, 0.0f, brushDiameter, brushDiameter, QPen(brushCursorColor, brushDiameter * brushCursorCircleWidthFactor));
            brushEllipseItem->setZValue(1.0f);
            brushEllipseItem->setTransformOriginPoint(brushRadius, brushRadius);
        }
    }
    else
    {
        if(brushEllipseItem)
        {
            scene.removeItem(brushEllipseItem);
            delete brushEllipseItem;
            brushEllipseItem = nullptr;
        }
    }
}

void MainWindow::on_actionTubeRemover_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::TubeRemover : Tool::None);
}

void MainWindow::on_actionShow_Hide_mask_toggled(bool arg1)
{
    maskVisible = arg1;
    renderImages();
}

void MainWindow::on_actionShow_Hide_tube_mask_toggled(bool arg1)
{
    tubeMaskVisible = arg1;
    renderImages();
}

void MainWindow::on_actionImage_config_triggered()
{
    if(!currImg.isNull())
    {
        ConfigWindow(this).exec();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}

