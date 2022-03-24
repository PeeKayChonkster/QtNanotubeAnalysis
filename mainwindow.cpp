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
      ui(new Ui::MainWindow),
      analyzer(this)
{
    setWindowTitle("NanotubeAnalysis");
    ui->setupUi(this);
    ui->graphicsView->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    ui->progressBar->hide();
    connect(&futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::worker_finished);
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
    ui->progressBar->show();
    auto workerLambda = [this]() -> void {
        analyzer.startExtremumAnalysis();
    };
    futureWatcher.setFuture(QtConcurrent::run(workerLambda));
}


void MainWindow::on_actionOpen_image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose image file", ".", "Image file (*.png *.jpg)");
    currImg.load(fileName);
    currImg = currImg.convertToFormat(QImage::Format_Grayscale16);
    renderCurrImg();
    analyzer.setTargetImg(&currImg);
    resize(currImg.width(), currImg.height());
}

void MainWindow::renderCurrImg()
{
    if(!currImg.isNull())
    {
        scene.clear();
        scene.addPixmap(QPixmap::fromImage(currImg));
        ui->graphicsView->setScene(&scene);
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

void MainWindow::setProgress(int progress)
{
    ui->progressBar->setValue(progress);
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

void MainWindow::worker_finished()
{
    futureWatcher.waitForFinished();
    ui->progressBar->hide();
}

