#include <stack>
#include <iostream>
#include <cassert>
#include "analyzer.hpp"
#include "prim_exception.hpp"
#include "tools.hpp"


nano::Analyzer::Analyzer() { mask = mask.convertToFormat(maskFormat); tubeMask = tubeMask.convertToFormat(maskFormat); }

nano::Analyzer::Analyzer(const QImage* targetImg):
        targetImg(targetImg), mask(targetImg->width(), targetImg->height(), maskFormat), tubeMask(targetImg->width(), targetImg->height(), maskFormat), nanotubes()
{
    setTargetImg(targetImg);
}

void nano::Analyzer::setTargetImg(const QImage* targetImg)
{
    this->targetImg = targetImg;
}

void nano::Analyzer::calculateMask(float threshold)
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to calculate mask without target image.");
    if(mask.isNull()) mask = QImage(targetImg->width(), targetImg->height(), maskFormat);

    uint32_t numberOfPixels = targetImg->width() * targetImg->height();

    for(int y = 0; y < targetImg->height(); ++y)
    {
        for(int x = 0; x < targetImg->width(); ++x)
        {
            const uint32_t idx = x + y * targetImg->width();
            float value = targetImg->pixelColor(x, y).redF();
            if(value >= threshold)
            {
                mask.setPixelColor(x, y, maskColorPos);
            }
            else
            {
                mask.setPixelColor(x, y, maskColorNeg);
            }
        }
    }
}

void nano::Analyzer::scanMaskForTubes()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to scan mask without target image.");
    if(mask.isNull()) throw PRIM_EXCEPTION("Trying to scan mask for tubes before creating mask.");
    if(tubeMask.isNull()) tubeMask = QImage(targetImg->width(), targetImg->height(), maskFormat);
    tubeMask.fill(maskColorNeg);
    setProgress(0.0f);
    nanotubes.clear();
    uint32_t numberOfPixels = targetImg->width() * targetImg->height();
    bool* checkArray = new bool[numberOfPixels] { false };

    for(int y = 0; y < mask.height(); ++y)
    {
        for(int x = 0; x < mask.width(); ++x)
        {
            int idx = x + y * mask.width();
            setProgress(idx / (float)numberOfPixels);
            if(!checkArray[idx])
            {
                std::vector<Point> points = checkPixel(x, y, checkArray);
                if(points.size() >= minPixelsInTube)
                {
                    for(const auto& point : points) { tubeMask.setPixelColor(x, y, maskColorPos); }
                    nanotubes.push_back(std::move(points));
                }
            }
        }
    }
    setProgress(1.0f);
    delete[] checkArray;
}

std::vector<nano::Point> nano::Analyzer::checkPixel(int x, int y, bool* checkArray)
{
    std::stack<Point> pointsStack;
    std::vector<Point> resultPoints;

    Point start(x, y);
    pointsStack.push(start);
    
    while(!pointsStack.empty())
    {
        Point currPoint = pointsStack.top(); pointsStack.pop();
        int idx = currPoint.x + currPoint.y * mask.width();

        if(currPoint.x < mask.width() && currPoint.x >= 0 && currPoint.y < mask.height() && currPoint.y >= 0 && !checkArray[idx] && mask.pixelColor(currPoint.x, currPoint.y).alphaF())
        {
            checkArray[idx] = true;
            resultPoints.push_back(currPoint);

            Point adjPoint;

            adjPoint = currPoint;
            adjPoint.x += 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x += 1u;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x += 1u;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width() && adjPoint.x >= 0 && adjPoint.y < mask.height() && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width()]) pointsStack.push(adjPoint);
        }
    }

    return std::move(resultPoints);
}

void nano::Analyzer::startExtremumAnalysis()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to find nanotube extremum without target image.");
    setProgress(0.0f);
    float threshold = 1.0f;
    float extremumThreshold = 1.0f;
    uint32_t extremumNumberOfTubes = 0u;
    calculateMask(threshold);
    scanMaskForTubes();
    uint8_t stopFlag = 0u;
    uint32_t currNumberOfTubes = nanotubes.size();
    uint32_t prevNumberOfTubes = currNumberOfTubes;
    printLine("Threshold = " + floatToString(threshold, 3u) + "; Nanotubes = " + std::to_string(currNumberOfTubes));
    while(true)
    {
        if(analysisCancelled)
        {
            analysisCancelled = false;
            progressReport = 0.0f;
            mask = QImage();
            nanotubes.clear();
            printLine("<<<<< Analysis cancelled >>>>>");
            return;
        }

        threshold -= extremumDeltaStep;
        calculateMask(threshold);
        scanMaskForTubes();
        currNumberOfTubes = nanotubes.size();
        printLine("Threshold = " + floatToString(threshold, 3u) + "; Nanotubes = " + std::to_string(currNumberOfTubes));

        if(currNumberOfTubes <= prevNumberOfTubes && currNumberOfTubes > 0u)    // derivative is negative
        {
            if(stopFlag == 0u && extremumNumberOfTubes < currNumberOfTubes)     // found local extremum
            {
                extremumThreshold = threshold + extremumDeltaStep;
                extremumNumberOfTubes = prevNumberOfTubes;
            }
            ++stopFlag;
        }
        else
        {
            stopFlag = 0u;
        }
        if((stopFlag >= extremumOverflowTolerance && !processFullRange) || threshold - extremumDeltaStep < 0.0f)                 // end analysis
        {
            calculateMask(extremumThreshold);
            scanMaskForTubes();

            printLine("<<<<< Results >>>>>");
            printLine("Extremum threshold = " + floatToString(extremumThreshold, 3u));
            printLine("Extremum number of tubes = " + std::to_string(extremumNumberOfTubes));
            printLine("Min pixels in nanotube = " + std::to_string(minPixelsInTube));
            printLine("Pixel size = " + floatToString(pixelSize_nm, 3u) + " (nm)");
            printLine("Image area = " + floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
            printLine("Nanotube density = " + floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
            return;
        }
        else                                                                    // resume analysis
        {
            prevNumberOfTubes = currNumberOfTubes;
        }
    }
    
}

void nano::Analyzer::startManualAnalysis(float threshold)
{
    setProgress(0.0f);
    nanotubes.clear();
    calculateMask(threshold);
    scanMaskForTubes();
    printLine("<<<<< Results >>>>>");
    printLine("Number of tubes = " + std::to_string(nanotubes.size()));
    printLine("Threshold = " + floatToString(threshold, 3u));
    printLine("Min pixels in nanotube = " + std::to_string(minPixelsInTube));
    printLine("Pixel size = " + floatToString(pixelSize_nm, 3u) + " (nm)");
    printLine("Image area = " + floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
    printLine("Nanotube density = " + floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
}

const QImage* nano::Analyzer::getMask() const
{
    return &mask;
}

const QImage* nano::Analyzer::getTubeMask() const
{
    return &tubeMask;
}

const std::vector<nano::Nanotube>* nano::Analyzer::getTubes() const
{ 
    return &nanotubes;
}

void nano::Analyzer::setProgress(float prog)
{
    progressReport = prog;
}

float nano::Analyzer::getProgress() const
{
    return progressReport;
}

float nano::Analyzer::getImageArea()
{
    assert(targetImg);
    return static_cast<float>(targetImg->width()) * pixelSize_nm * static_cast<float>(targetImg->height()) * pixelSize_nm;
}

float nano::Analyzer::getDensity()
{
    return nanotubes.size() / getImageArea();
}

void nano::Analyzer::resetAll()
{
    targetImg = nullptr;
    mask = QImage();
    tubeMask = QImage();
    progressReport = 0.0f;
    pixelSize_nm = 0.0f;
    nanotubes.clear();
}

bool nano::Analyzer::areTubesCalculated() const
{
    return !nanotubes.empty();
}

void nano::Analyzer::cancelAnalysis()
{
    analysisCancelled = true;
}
