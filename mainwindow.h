#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "analyzer.hpp"
#include "autoanalysisconfig.h"
#include "manualanalysisconfig.h"
#include "currentmaskanalysisconfig.h"
#include "console.h"
#include <QGraphicsScene>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <vector>
#include <optional>
#include <QLabel>

#define NAMEOF(name) #name

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startAutoAnalysis();
    void startManualAnalysis(float threshold);
    void startCurrentMaskAnalysis();
    void updateTextures();
    void calculateMask(float threshold);
    void clearMask();


    // These work with MyGraphicsView mouse events
    void mouseMoveEventGV(QMouseEvent* event);
    void mousePressEventGV(QMouseEvent* event);
    void mouseReleaseEventGV(QMouseEvent* event);

    void setBrushRadius(float radius);
    float getBrushRadius() const;
    void setPixelSize(float size);
    float getPixelSize() const;
    void setExtremumDeltaStep(float value);
    float getExtremumDeltaStep() const;
    void setExtremumOverflowTolerance(uint16_t value);
    uint16_t getExtremumOverflowTolerance() const;
    void setProcessFullRange(bool value);
    bool getProcessFullRange() const;
    void setMinPixelInTube(uint16_t value);
    uint16_t getMinPixelInTube() const;

    using RulerPair = QPair<QGraphicsLineItem*, QGraphicsProxyWidget*>;

    friend class Tools;

private slots:
    void on_actionOpen_image_triggered();

    void on_actionShow_console_triggered();

    void on_actionStart_extremum_analysis_triggered();

    void sl_worker_finished();

    void sl_progress_changed(int progress);

    void sl_analysis_canceled();

    void on_actionStart_manual_analysis_triggered();

    void on_actionClear_all_ruler_lines_triggered();

    void on_actionRuler_toggled(bool arg1);

    void on_actionTubeAdder_toggled(bool arg1);

    void on_actionMaskBrush_toggled(bool arg1);

    void on_actionMaskEraser_toggled(bool arg1);

    void on_actionTubeRemover_toggled(bool arg1);

    void on_actionShow_Hide_mask_toggled(bool arg1);

    void on_actionShow_Hide_tube_mask_toggled(bool arg1);

    void on_actionImage_config_triggered();

    void on_actionAnalyze_current_mask_triggered();

    void on_actionExit_triggered();

    void on_actionClear_mask_triggered();

protected:
    void closeEvent(QCloseEvent* event) override;

    enum class Tool { None, Ruler, MaskBrush, MaskEraser, TubeAdder, TubeRemover };

private:
    Ui::MainWindow *ui;
    nano::Analyzer analyzer;
    Console console;
    QImage currImg;
    const QImage* mask = nullptr;
    const QImage* tubeMask = nullptr;
    QGraphicsPixmapItem* currImgPixmapItem = nullptr;
    QGraphicsPixmapItem* maskPixmapItem = nullptr;
    QGraphicsPixmapItem* tubeMaskPixmapItem = nullptr;
    QGraphicsEllipseItem* brushEllipseItem = nullptr;
    AutoAnalysisConfig autoAnalysisConfig;
    ManualAnalysisConfig manualAnalysisConfig;
    CurrentMaskAnalysisConfig currentMaskAnalysisConfig;
    QGraphicsScene scene;
    QFutureWatcher<void> futureWatcher;
    QProgressDialog* progressDialog = nullptr;
    QLabel coordLabel;
    QLabel toolLabel;
    Tool activeTool = Tool::None;
    std::vector<RulerPair> rulerLineItems;
    std::optional<QPointF> firstRulerLinePoint = std::nullopt;
    QPen rulerLinePen;
    float brushRadius = 7.0f;

    // constants
    const QColor rulerLineColor = QColorConstants::Blue;
    const QColor brushCursorColor = QColor(255, 255, 0, 200);
    const float brushCursorCircleWidthFactor = 0.02;
    const uint rulerLIneWidth = 2u;
    const QString rulerLabelBgColor = "rgba(0,0,0,0.7);";
    const Qt::MouseButton actionButton = Qt::LeftButton;

    // flags
    bool currImgVisible = true;
    bool maskVisible = true;
    bool tubeMaskVisible = true;
    bool pressingActionButtonGV = false;


    void openImage();
    void fastOpenImage(); // DEBUG
    void clearGraphicsView();
    void startProgressDialog();
    void addRulerPoint(QPoint point);
    void clearAllRulerLines();
    void setActiveTool(Tool tool);
    void addTubeAtPos(QPoint pos);
    void removeTubeAtPos(QPoint pos);
    void paintMaskAtPos(QPoint pos);
    void eraseMaskAtPos(QPoint pos);
};
#endif // MAINWINDOW_H
