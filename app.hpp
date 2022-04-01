#ifndef APP_HPP
#define APP_HPP

#include "mainwindow.h"

class App final
{
private:
    App() = delete;
    App(const App& other) = delete;

    inline static class MainWindow* mw = nullptr;
    inline static bool initialized = false;
    inline static nano::Analyzer analyzer;

    enum class Tool { None, Ruler, MaskBrush, MaskEraser, TubeAdder, TubeRemover };

    using RulerPair = QPair<QGraphicsLineItem*, QGraphicsProxyWidget*>;


public:

    inline static Tool activeTool = Tool::None;
    inline static std::vector<RulerPair> rulerLineItems;
    inline static std::optional<QPointF> firstRulerLinePoint = std::nullopt;
    inline static QPen rulerLinePen;
    inline static float brushRadius = 7.0f;

    static void init(MainWindow* mainWindow);
    static std::string floatToString(float f, uint8_t precision);
    static void print(std::string line, QColor colorj = QColorConstants::White);
    static MainWindow* getMainWindow();

    static void mouseMoveEventGV(QMouseEvent* event);
    static void mousePressEventGV(QMouseEvent* event);
    static void mouseReleaseEventGV(QMouseEvent* event);

    static void setBrushRadius(float radius);
    static float getBrushRadius();
    static void setPixelSize(float size);
    static float getPixelSize();
    static void setExtremumDeltaStep(float value);
    static float getExtremumDeltaStep();
    static void setExtremumOverflowTolerance(uint16_t value);
    static uint16_t getExtremumOverflowTolerance();
    static void setProcessFullRange(bool value);
    static bool getProcessFullRange();
    static  void setMinPixelInTube(uint16_t value);
    static uint16_t getMinPixelInTube();

    // constants
    inline static const QColor rulerLineColor = QColorConstants::Blue;
    inline static const QColor brushCursorColor = QColor(255, 255, 0, 200);
    inline static const float brushCursorCircleWidthFactor = 0.02;
    inline static const uint rulerLIneWidth = 2u;
    inline static const QString rulerLabelBgColor = "rgba(0,0,0,0.7);";
    inline static const Qt::MouseButton actionButton = Qt::LeftButton;

    // flags
    inline static bool currImgVisible = true;
    inline static bool maskVisible = true;
    inline static bool tubeMaskVisible = true;
    inline static bool pressingActionButtonGV = false;
};

#endif // APP_HPP
