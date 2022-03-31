#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "analyzer.hpp"
#include "autoanalysisconfig.h"
#include "manualanalysisconfig.h"
#include "console.h"
#include <QGraphicsScene>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <vector>
#include <optional>
#include <QLabel>

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
    void startManualAnalysis(float threshold);

    friend class ManualAnalysisConfig;
    friend class MyGraphicsView;

private slots:
    void on_actionOpen_image_triggered();

    void on_actionShow_console_triggered();

    void on_actionStart_extremum_analysis_triggered();

    void sl_worker_finished();

    void sl_progress_changed(int progress);

    void on_actionStart_manual_analysis_triggered();

    void sl_graphicsScene_mousePressLeft(QPoint pos);

    void on_actionClear_all_ruler_lines_triggered();

    void on_actionRuler_toggled(bool arg1);

protected:
    void closeEvent(QCloseEvent* event) override;
    void mouseMoveGraphicsViewEvent(QMouseEvent* event);

    enum class Tool { None, Ruler, MaskBrush, MaskEraser, TubeAdder };


private:
    Ui::MainWindow *ui;
    QImage currImg;
    const QImage* mask = nullptr;
    const QImage* tubeMask = nullptr;
    QGraphicsPixmapItem* currImgPixmapItem = nullptr;
    QGraphicsPixmapItem* maskPixmapItem = nullptr;
    QGraphicsPixmapItem* tubeMaskPixmapItem = nullptr;
    AutoAnalysisConfig autoAnalysisConfig;
    ManualAnalysisConfig manualAnalysisConfig;
    QGraphicsScene scene;
    QFutureWatcher<void> futureWatcher;
    QProgressDialog* progressDialog = nullptr;
    QLabel coordLabel;
    Tool activeTool = Tool::None;
    std::vector<QGraphicsLineItem*> rulerLineItems;
    std::optional<QPointF> firstRulerLinePoint = std::nullopt;
    QPen rulerLinePen;
    const QColor rulerLineColor = QColorConstants::Blue;
    const uint rulerLIneWidth = 5u;

    // flags
    bool currImgVisible = true;
    bool maskVisible = true;
    bool tubeMaskVisible = true;

    void renderImages();
    void fastOpenImage(); // DEBUG
    void removeCurrImage();
    void startProgressDialog();
    void setMask();
    void setTubeMask();
    void setRulerPoint(QPoint point);
};
#endif // MAINWINDOW_H
