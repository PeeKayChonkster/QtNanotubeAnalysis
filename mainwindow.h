#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "analyzer.hpp"
#include "autoanalysisconfig.h"
#include "console.h"
#include <QGraphicsScene>
#include <QFutureWatcher>
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    nano::Analyzer analyzer;
    Console console;

    void startAutoAnalysis();

private slots:
    void on_actionOpen_image_triggered();

    void on_actionShow_console_triggered();

    void on_actionStart_extremum_analysis_triggered();

    void sl_worker_finished();

    void sl_progress_changed(int progress);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;
    QImage currImg;
    AutoAnalysisConfig autoAnalysisConfig;
    QGraphicsScene scene;
    float imgScaleDelta = 0.01f;
    bool holdingRightButton = false;
    QFutureWatcher<void> futureWatcher;
    QProgressDialog* progressDialog = nullptr;
    QScreen* screen = nullptr;

    void renderCurrImg();
    void fastOpenImage(); // DEBUG
    void startProgressDialog();
};
#endif // MAINWINDOW_H
