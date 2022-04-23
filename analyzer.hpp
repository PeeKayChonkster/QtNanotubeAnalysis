#ifndef Analyzer_HPP
#define Analyzer_HPP

#include <vector>
#include <QImage>
#include "element.hpp"
#include <functional>
#include <QObject>
#include <tuple>

class MainWindow;

namespace nano
{

class Analyzer: public QObject
{
     Q_OBJECT

private:
    const QColor maskColorPos = { 0, 230, 0, 150 };
    const QColor elementMaskColorPos = { 230, 0, 0, 150 };
    const QColor maskColorNeg = { 0, 0, 0, 0 };
    const QImage::Format maskFormat = QImage::Format_RGBA8888;

    const QImage* targetImg = nullptr;
    QImage mask;
    QImage elementMask;
    std::vector<Element> elements;

    std::vector<ImgPoint> checkPixel(int x, int y, bool* checkArray);
    void setProgress(int prog);

    bool analysisCancelled = false;
public:
    Analyzer();
    Analyzer(const QImage* targetImg);
    Analyzer(const Analyzer& other) = delete;

    // analysis config
    float pixelSize_nm = 0.0f;
    float extremumDeltaStep = 0.02f;
    uint16_t extremumOverflowTolerance = 5;
    uint16_t minPixelsInElement = 50;
    bool processFullRange = false;


    void setTargetImg(const QImage* targetImg);
    void calculateMask(float threshold, QRect area = QRect());
    void clearMasks();
    void scanMaskForElements();
    void startExtremumAnalysis();
    /// returns { threshold, numberOfElements, elementsDensity }
    std::vector<std::tuple<float, uint, float>> startFullRangeAnalysis(float deltaStep, QRect area = QRect());
    void startManualAnalysis(float threshold);
    void startCurrentMaskAnalysis();
    float startThresholdAnalysis(float deltaStep);
    void addElementAtPos(QPoint pos);
    void removeElementAtPos(QPoint pos);
    void paintMaskAtPos(QPoint pos, float radius = 1.0f);
    void eraseMaskAtPos(QPoint pos, float radius = 1.0f);
    const QImage* getMask() const;
    const QImage* getElementMask() const;
    const std::vector<Element>* getElements() const;
    float getImageArea();
    float getDensity();
    bool areElementsCalculated() const;
    void cancelAnalysis();
    std::vector<ImgPoint> getPointsInRadius(Point<float> center, float radius) const;


    void resetAll();

signals:
    void si_progress_changed(int progress);
};

}  // namespace nano

#endif // Analyzer_HPP
