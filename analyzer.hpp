#ifndef Analyzer_HPP
#define Analyzer_HPP

#include <vector>
#include <QImage>
#include "nanotube.hpp"
#include <functional>
#include <QObject>

class MainWindow;

namespace nano
{

class Analyzer: public QObject
{
     Q_OBJECT

private:
    const QColor maskColorPos = { 0, 230, 0, 150 };
    const QColor tubeMaskColorPos = { 230, 0, 0, 150 };
    const QColor maskColorNeg = { 0, 0, 0, 0 };
    const QImage::Format maskFormat = QImage::Format_RGBA8888;

    const QImage* targetImg = nullptr;
    QImage mask;
    QImage tubeMask;
    std::vector<Nanotube> nanotubes;

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
    uint16_t minPixelsInTube = 50;
    bool processFullRange = false;


    void setTargetImg(const QImage* targetImg);
    void calculateMask(float threshold);
    void scanMaskForTubes();
    void startExtremumAnalysis();
    void startManualAnalysis(float threshold);
    void startCurrentMaskAnalysis();
    void addTubeAtPos(QPoint pos);
    void removeTubeAtPos(QPoint pos);
    void paintMaskAtPos(QPoint pos, float radius = 1.0f);
    void eraseMaskAtPos(QPoint pos, float radius = 1.0f);
    const QImage* getMask() const;
    const QImage* getTubeMask() const;
    const std::vector<Nanotube>* getTubes() const;
    float getImageArea();
    float getDensity();
    bool areTubesCalculated() const;
    void cancelAnalysis();
    std::vector<ImgPoint> getPointsInRadius(Point<float> center, float radius) const;


    void resetAll();

signals:
    void si_progress_changed(int progress);
};

}  // namespace nano

#endif // Analyzer_HPP
