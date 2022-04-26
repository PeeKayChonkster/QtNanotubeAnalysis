#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tools.hpp"
#include "mygraphicsview.h"
#include "configwindow.h"
#include "prim_exception.hpp"
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
#include <currentmaskanalysisconfig.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      console(new Console(this)),
      autoAnalysisConfig(new AutoAnalysisConfig(this)),
      manualAnalysisConfig(new ManualAnalysisConfig(this)),
      currentMaskAnalysisConfig(new CurrentMaskAnalysisConfig(this)),
      fullRangeAnalysisConfig(new FullRangeAnalysisConfig(this)),
      thresholdAnalysisConfig(new ThresholdAnalysisConfig(this)),
      coordLabel(new QLabel(this)),
      toolLabel(new QLabel(this)),
      chartWindow(new ChartWindow(this)),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("NanotubeAnalysis");

    connect(&futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::sl_worker_finished);
    connect(&analyzer, &nano::Analyzer::si_progress_changed, this, &MainWindow::sl_progress_changed);
    connect(&analyzer, &nano::Analyzer::si_chart_series_output, this, &MainWindow::sl_add_chart_series);

    Tools::init(this);

    ui->graphicsView->verticalScrollBar()->installEventFilter(this);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setScene(&scene);

    rulerLinePen.setColor(rulerLineColor);
    rulerLinePen.setWidth(rulerLineWidth);
    rulerLinePen.setCapStyle(Qt::RoundCap);

    ui->toolsGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    coordLabel->setAlignment(Qt::AlignRight);
    toolLabel->setText("Active tool: None");
    statusBar()->addPermanentWidget(toolLabel);
    statusBar()->addPermanentWidget(coordLabel, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startAutoAnalysis()
{
    console->show();
    console->print();    scene.update();
    console->print("<<<<< Starting auto analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this]() -> void {
        analyzer.startExtremumAnalysis();
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::startManualAnalysis(float threshold)
{
    console->show();
    console->print();
    console->print("<<<<< Starting manual analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this, threshold]() -> void {
        analyzer.startManualAnalysis(threshold);
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::startCurrentMaskAnalysis()
{
    console->print();
    console->print("<<<<< Starting current mask analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this]() -> void {
        analyzer.startCurrentMaskAnalysis();
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::startThresholdAnalysis(float deltaStep, uint divisionCount, bool horizontal)
{
    console->print();
    console->print("<<<<< Starting threshold analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [=]() -> void {
        analyzer.startThresholdAnalysis(deltaStep, divisionCount, horizontal);
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}

void MainWindow::startFullRangeAnalysis(float deltaStep, bool writeTable)
{
    console->print();
    console->print("<<<<< Starting full range analysis >>>>>", QColorConstants::Green);
    startProgressDialog();
    auto workerLambda = [this, deltaStep, writeTable]() -> void {
        std::vector<std::tuple<float, uint, float>> results = analyzer.startFullRangeAnalysis(deltaStep);
        if(writeTable) writeCSVFile(results);
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
    if(size > 0.0f)
    {
        analyzer.pixelSize_nm = size;

        // adjust ruler line labels
        for(RulerPair& pair : rulerLineItems)
        {
            float newLength = pair.first->line().length() * size;
            QLabel* label = static_cast<QLabel*>(pair.second->widget());
            DistanceUnit oldUnit = Tools::stringToDistanceUnit(Tools::splitString(label->text().toStdString(), ' ')[1]);
            Distance distance = Tools::getRelevantDistance(newLength, oldUnit);
            label->setText(QString::number(distance.first) + " " + std::string(NAMEOF_ENUM(distance.second)).c_str());
            label->adjustSize();
        }
    }
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

void MainWindow::setMinPixelInElement(uint16_t value)
{
    analyzer.minPixelsInElement = value;
}

uint16_t MainWindow::getMinPixelInElement() const
{
    return analyzer.minPixelsInElement;
}

void MainWindow::showChart()
{
    chartWindow->show();
}

void MainWindow::hideChart()
{
    chartWindow->hide();
}

void MainWindow::clearChart()
{
    chartWindow->clear();
}

void MainWindow::on_actionOpen_image_triggered()
{
    openImage();
}

void MainWindow::openImage()
{
//    QString fileName = QFileDialog::getOpenFileName(this, "Choose image file", ".", "Image file (*.png *.jpg)");
//    currImg.load(fileName);
//    //currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
//    analyzer.setTargetImg(&currImg);
//    mask = analyzer.getMask();
//    elementMask = analyzer.getElementMask();
//    updateTextures();
//    resize(currImg.width(), currImg.height());
//    scene.setSceneRect(0.0f, 0.0f, currImg.width(), currImg.height());
//    Tools::print("Loaded image file: " + fileName.toStdString(), Colors::green);

    // DEBUG //
    fastOpenImage();
}

void MainWindow::fastOpenImage()
{
    currImg.load("../QNanotubeAnalysis/res/img/SamplesJPEG/S1-ZnAg_02_PS=5,58.jpg");
    Tools::print(std::string("Loaded image file: ") + "../QNanotubeAnalysis/res/img/SamplesJPEG/S1-ZnAg_02_PS=5,58.jpg", Colors::lime);
    Tools::print(std::string("Image size: " + std::to_string(currImg.width()) + "x" + std::to_string(currImg.height())));
    //currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
    analyzer.setTargetImg(&currImg);
    mask = analyzer.getMask();
    elementMask = analyzer.getElementMask();
    updateTextures();
    resize(currImg.width(), currImg.height());
    scene.setSceneRect(0.0f, 0.0f, currImg.width(), currImg.height());
}

void MainWindow::clearGraphicsView()
{
    if(!currImg.isNull())
    {
        scene.clear();
        currImgPixmapItem = nullptr;
        maskPixmapItem = nullptr;
        elementMaskPixmapItem = nullptr;
        ui->graphicsView->setScene(&scene);
        currImg = QImage();
        mask = nullptr;
        elementMask = nullptr;
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
        Distance distance = Tools::getRelevantDistance(line.length() * analyzer.pixelSize_nm, DistanceUnit::nm);
        float lineRealLength = distance.first;
        QString unitOfMeasure(std::string(NAMEOF_ENUM(distance.second)).c_str());
        QLabel* label = new QLabel(QString::number(lineRealLength) + " " + unitOfMeasure);
        label->setStyleSheet("background-color: " + rulerLabelBgColor + " color: white;");
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

void MainWindow::addElementAtPos(QPoint pos)
{
    if(mask && elementMask)
    {
        analyzer.addElementAtPos(pos);
        updateTextures();
    }
}

void MainWindow::removeElementAtPos(QPoint pos)
{
    if(mask && elementMask)
    {
        analyzer.removeElementAtPos(pos);
        updateTextures();
    }
}

void MainWindow::paintMaskAtPos(QPoint pos)
{
    if(!currImg.isNull())
    {
        analyzer.paintMaskAtPos(pos, brushRadius);
        updateTextures();
    }
}

void MainWindow::eraseMaskAtPos(QPoint pos)
{
    if(!currImg.isNull())
    {
        analyzer.eraseMaskAtPos(pos, brushRadius);
        updateTextures();
    }
}

void MainWindow::writeCSVFile(const std::vector<std::tuple<float, uint, float>>& data)
{
    static const QString filePrefix = "table";
    static const QString fileSuffix = ".csv";
    uint fileNumber = 0;
    QString fileName = filePrefix + fileSuffix;
    QFile file(fileName);
    while(file.exists())
    {
        fileName = filePrefix + QString::number(++fileNumber) + fileSuffix;
        file.setFileName(fileName);
    }
    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
    {

        QTextStream stream(&file);
        stream.setRealNumberPrecision(3);
        stream << "Threshold,Number of elements,Density(1/mm2)\n";
        for(const auto& chunk : data)
        {
            stream << std::get<0>(chunk) << ',' << std::get<1>(chunk) << ',' << std::get<2>(chunk) << '\n';
        }
        file.close();
        Tools::print("Saved results table to " + fileName.toStdString() + " file.", Colors::lime);
    }
    else
    {
        throw PRIM_EXCEPTION("Could't open file with name:" + file.fileName().toStdString());
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
    toolLabel->setText(toolString.c_str());
}

void MainWindow::updateTextures()
{
    if(!currImg.isNull() && currImgVisible)
    {
        if(currImgPixmapItem)
        {
            delete currImgPixmapItem;
        }
        currImgPixmapItem = scene.addPixmap(QPixmap::fromImage(currImg));
    }
    if(mask && maskVisible)
    {
        if(maskPixmapItem)
        {
            delete maskPixmapItem;
        }
        maskPixmapItem = scene.addPixmap(QPixmap::fromImage(*mask));
    }
    if(elementMask && elementMaskVisible)
    {
        if(elementMaskPixmapItem)
        {
            delete elementMaskPixmapItem;
        }
        elementMaskPixmapItem = scene.addPixmap(QPixmap::fromImage(*elementMask));
    }
}

void MainWindow::calculateMask(float threshold)
{
    analyzer.calculateMask(threshold);
    updateTextures();
}

void MainWindow::clearMasks()
{
    analyzer.clearMasks();
    updateTextures();
}

QColor MainWindow::getSystemBackgroundColor() const
{
    return palette().base().color();
}


void MainWindow::on_actionShow_console_triggered()
{
    console->show();
}

void MainWindow::mouseMoveEventGV(QMouseEvent *event)
{
    QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
    coordLabel->setText(QString("x:%1|y:%2").arg(scenePos.x(), 5).arg(scenePos.y(), 5));

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
                addElementAtPos(scenePos);
                break;
            case Tool::TubeRemover:
                removeElementAtPos(scenePos);
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
        autoAnalysisConfig->exec();
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
        manualAnalysisConfig->show();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}

void MainWindow::on_actionAnalyze_current_mask_triggered()
{
    if(!currImg.isNull())
    {
        currentMaskAnalysisConfig->show();
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

void MainWindow::sl_add_chart_series(const std::vector<std::pair<float, float>>& series, QString legendName, QString xName, QString yName, QColor color)
{
    chartWindow->addSeries(series, legendName, xName, yName, color);
    showChart();
}

void MainWindow::sl_worker_finished()
{
    futureWatcher.waitForFinished();
    progressDialog->hide();
    updateTextures();
}


void MainWindow::on_actionRuler_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::Ruler : Tool::None);
}


void MainWindow::on_actionClear_all_ruler_lines_triggered()
{
    clearAllRulerLines();
}

void MainWindow::on_actionElementAdder_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::TubeAdder : Tool::None);
}


void MainWindow::on_actionMaskBrush_toggled(bool arg1)
{
    if(arg1 && !currImg.isNull())
    {
        setActiveTool(arg1? Tool::MaskBrush : Tool::None);
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
        setActiveTool(arg1? Tool::MaskEraser : Tool::None);
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

void MainWindow::on_actionElementRemover_toggled(bool arg1)
{
    setActiveTool(arg1? Tool::TubeRemover : Tool::None);
}

void MainWindow::on_actionShow_Hide_mask_toggled(bool arg1)
{
    maskVisible = arg1;
    updateTextures();
}

void MainWindow::on_actionShow_Hide_tube_mask_toggled(bool arg1)
{
    elementMaskVisible = arg1;
    updateTextures();
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

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionClear_masks_triggered()
{
    clearMasks();
}


void MainWindow::on_actionStart_threshold_analysis_triggered()
{
    if(!currImg.isNull())
    {
        thresholdAnalysisConfig->exec();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}


void MainWindow::on_actionStart_full_range_analysis_triggered()
{
    if(!currImg.isNull())
    {
        fullRangeAnalysisConfig->exec();
    }
    else
    {
        QMessageBox::warning(this, "No image!", "There is no image to analyze!");
    }
}

