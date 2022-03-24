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
    const QColor maskColorPos = { 0, 230, 0, 180 };
    const QColor tubeMaskColorPos = { 230, 0, 0, 180 };
    const QColor maskColorNeg = { 0, 0, 0, 0 };
    const QImage::Format maskFormat = QImage::Format_RGBA8888;

    const QImage* targetImg = nullptr;
    QImage mask;
    QImage tubeMask;
    std::vector<Nanotube> nanotubes;

    std::vector<Point> checkPixel(int x, int y, bool* checkArray);
    void setProgress(int prog);

    bool analysisCancelled = false;
public:
    Analyzer();
    Analyzer(const QImage* targetImg);
    Analyzer(const Analyzer& other) = delete;

    // analysis config
    float pixelSize_nm = 0.0f;
    float extremumDeltaStep = 0.02f;
    int extremumOverflowTolerance = 5;
    int minPixelsInTube = 50;
    bool processFullRange = false;


    void setTargetImg(const QImage* targetImg);
    void calculateMask(float threshold);
    void scanMaskForTubes();
    void startExtremumAnalysis();
    void startManualAnalysis(float threshold);
    const QImage* getMask() const;
    const QImage* getTubeMask() const;
    const std::vector<Nanotube>* getTubes() const;
    float getImageArea();
    float getDensity();
    bool areTubesCalculated() const;
    void cancelAnalysis();

    void resetAll();

signals:
    void si_progress_changed(int progress);
};

}  // namespace nano

#endif // Analyzer_HPP
