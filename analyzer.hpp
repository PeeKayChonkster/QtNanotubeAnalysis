#ifndef Analyzer_HPP
#define Analyzer_HPP

#include <vector>
#include <QImage>
#include "nanotube.hpp"
#include <functional>

class MainWindow;

namespace nano
{

class Analyzer
{
private:
    const QColor maskColorPos = { 0, 230, 0, 180 };
    const QColor tubeMaskColorPos = { 230, 0, 0, 180 };
    const QColor maskColorNeg = { 0, 0, 0, 0 };
    const QImage::Format maskFormat = QImage::Format_RGBA8888;

    MainWindow* parent = nullptr;
    const QImage* targetImg = nullptr;
    QImage mask;
    QImage tubeMask;
    std::vector<Nanotube> nanotubes;
    int progressReport = 0;

    std::vector<Point> checkPixel(int x, int y, bool* checkArray);
    void setProgress(int prog);

    bool analysisCancelled = false;
public:
    Analyzer(MainWindow* parent);
    Analyzer(MainWindow* parent, const QImage* targetImg);
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
    int getProgress();
    bool areTubesCalculated() const;
    void cancelAnalysis();

    void resetAll();
};

}  // namespace nano

#endif // Analyzer_HPP
