#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tools.hpp"
#include <QFileDialog>
#include <QPixmap>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <functional>
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      console(this),
      autoAnalysisConfig(this),
      manualAnalysisConfig(this),
      ui(new Ui::MainWindow)
{
    setWindowTitle("NanotubeAnalysis");
    ui->setupUi(this);
    ui->graphicsView->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    connect(&futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::sl_worker_finished);
    connect(&analyzer, &nano::Analyzer::si_progress_changed, this, &MainWindow::sl_progress_changed);
    tools::init(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startAutoAnalysis()
{
    console.show();
    console.print();
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
}

void MainWindow::fastOpenImage()
{
    currImg.load("../QNanotubeAnalysis/img/SamplesJPEG/S1-ZnAg_02.jpg");
    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
    renderImages();
    analyzer.setTargetImg(&currImg);
    resize(currImg.width(), currImg.height());
}

void MainWindow::startProgressDialog()
{
    progressDialog = new QProgressDialog("Calculating", "Cancel", 0, 100, this);
    progressDialog->setMinimumDuration(0);
    progressDialog->setMinimumWidth(400);
    progressDialog->setFixedSize(progressDialog->size());
    QSize screenSize = QApplication::primaryScreen()->availableSize();
    progressDialog->move((screenSize.width() - progressDialog->width()) / 2, (screenSize.height() - progressDialog->height()) / 2);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(false);
    progressDialog->setAutoReset(false);
}

void MainWindow::setMask()
{
    mask = analyzer.getMask();
}

void MainWindow::setTubeMask()
{
    tubeMask = analyzer.getTubeMask();
}

void MainWindow::renderImages()
{
    scene.clear();
    if(!currImg.isNull() && currImgVisible)
    {
        scene.addPixmap(QPixmap::fromImage(currImg));
        ui->graphicsView->setScene(&scene);
    }
    if(mask && maskVisible)
    {
        scene.addPixmap(QPixmap::fromImage(*mask));
    }
    if(tubeMask && tubeMaskVisible)
    {
        scene.addPixmap(QPixmap::fromImage(*tubeMask));
    }
}

void MainWindow::on_actionShow_console_triggered()
{
    console.show();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(!currImg.isNull())
    {
        float delta = imgScaleDelta;
        if(event->angleDelta().y() < 0.0f) delta *= -1.0f;
        if(QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier)) delta *= 5.0f;
        ui->graphicsView->scale(1.0f + delta, 1.0f + delta);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        holdingRightButton = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        holdingRightButton = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
//    static QPoint oldMousePos(0, 0);
//    if(holdingRightButton)
//    {
//        QPoint delta = event->pos() - oldMousePos;
//        ui->graphicsView->move(ui->graphicsView->pos() + delta);
//    }
    //    oldMousePos = event->pos();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    console.close();
    autoAnalysisConfig.close();
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

void MainWindow::sl_worker_finished()
{
    futureWatcher.waitForFinished();
    progressDialog->close();
    delete progressDialog;
    progressDialog = nullptr;
    setMask();
    setTubeMask();
    maskVisible = tubeMaskVisible = currImgVisible = true;
    renderImages();
}

