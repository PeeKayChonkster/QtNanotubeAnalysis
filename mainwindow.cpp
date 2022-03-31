#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tools.hpp"
#include "mygraphicsview.h"
#include "imageconfig.h"
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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      console(this),
      autoAnalysisConfig(this),
      manualAnalysisConfig(this),
      coordLabel(this),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("NanotubeAnalysis");
    connect(&futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::sl_worker_finished);
    connect(&analyzer, &nano::Analyzer::si_progress_changed, this, &MainWindow::sl_progress_changed);
    tools::init(this);
    progressDialog = new QProgressDialog("Calculating", "Cancel", 0, 100, this);
    progressDialog->setMinimumDuration(0);
    progressDialog->setMinimumWidth(400);
    progressDialog->setFixedSize(progressDialog->size());
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(false);
    progressDialog->setAutoReset(false);
    progressDialog->close();
    ui->graphicsView->verticalScrollBar()->installEventFilter(this);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setScene(&scene);
    connect(ui->graphicsView, &MyGraphicsView::si_mousePressLeft, this, &MainWindow::sl_graphicsScene_mousePressLeft);
    rulerLinePen.setColor(rulerLineColor);
    rulerLinePen.setWidth(rulerLIneWidth);
    ui->toolsGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    coordLabel.setFrameShape(QFrame::Panel);
    coordLabel.setFrameShadow(QFrame::Raised);
    statusBar()->addPermanentWidget(&coordLabel);
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


void MainWindow::on_actionOpen_image_triggered()
{
//    QString fileName = QFileDialog::getOpenFileName(this, "Choose image file", ".", "Image file (*.png *.jpg)");
//    currImg.load(fileName);
//    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
//    renderCurrImg();
//    analyzer.setTargetImg(&currImg);
//    resize(currImg.width(), currImg.height());

    // DEBUG //
    fastOpenImage();
    ui->graphicsView->show();
    if(ImageConfig(this).exec() == QDialog::Rejected)
    {
        removeCurrImage();
        ui->graphicsView->hide();
    }
}

void MainWindow::fastOpenImage()
{
    currImg.load("../QNanotubeAnalysis/res/img/SamplesJPEG/S1-ZnAg_02.jpg");
    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
    renderImages();
    analyzer.setTargetImg(&currImg);
    resize(currImg.width(), currImg.height());
}

void MainWindow::removeCurrImage()
{
    if(currImgPixmapItem)
    {
        scene.clear();
        currImgPixmapItem = nullptr;
        currImg = QImage();
    }
}

void MainWindow::startProgressDialog()
{
    QSize screenSize = QApplication::primaryScreen()->availableSize();
    progressDialog->move((screenSize.width() - progressDialog->width()) / 2, (screenSize.height() - progressDialog->height()) / 2);
    progressDialog->show();
}

void MainWindow::setMask()
{
    mask = analyzer.getMask();
}

void MainWindow::setTubeMask()
{
    tubeMask = analyzer.getTubeMask();
}

void MainWindow::setRulerPoint(QPoint point)
{
    if(currImg.isNull()) return;

    if(firstRulerLinePoint)
    {
        QLineF line(firstRulerLinePoint.value(), ui->graphicsView->mapToScene(point));
        rulerLineItems.push_back(scene.addLine(line, rulerLinePen));

        // Create label with line length;

        firstRulerLinePoint = std::nullopt;
    }
    else
    {
        firstRulerLinePoint = ui->graphicsView->mapToScene(point);
    }
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

void MainWindow::on_actionShow_console_triggered()
{
    console.show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    console.close();
    autoAnalysisConfig.close();
}

void MainWindow::mouseMoveGraphicsViewEvent(QMouseEvent *event)
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
        case Tool::MaskBrush:
            break;
        case Tool::MaskEraser:
            break;
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

void MainWindow::sl_graphicsScene_mousePressLeft(QPoint pos)
{
    switch(activeTool)
    {
        case Tool::None:
            break;
        case Tool::Ruler:
            setRulerPoint(pos);
            break;
        case Tool::TubeAdder:
            break;
        case Tool::MaskBrush:
            break;
        case Tool::MaskEraser:
            break;
    }
}

void MainWindow::sl_progress_changed(int progress)
{
    progressDialog->setValue(progress);
}

void MainWindow::sl_worker_finished()
{
    futureWatcher.waitForFinished();
    progressDialog->close();
    setMask();
    setTubeMask();
    maskVisible = tubeMaskVisible = currImgVisible = true;
    renderImages();
}


void MainWindow::on_actionRuler_toggled(bool arg1)
{
    if(arg1) activeTool = Tool::Ruler;
    else activeTool = Tool::None;
}


void MainWindow::on_actionClear_all_ruler_lines_triggered()
{
    for(QGraphicsLineItem* lineItem : rulerLineItems)
    {
        scene.removeItem(lineItem);
        delete lineItem;
    }
    rulerLineItems.clear();
}


