#include <stack>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <limits>
#include <mainwindow.h>
#include "analyzer.hpp"
#include "prim_exception.hpp"
#include "tools.hpp"


nano::Analyzer::Analyzer()
{
    mask = mask.convertToFormat(maskFormat);
    elementMask = elementMask.convertToFormat(maskFormat);
}

nano::Analyzer::Analyzer(const QImage* targetImg):
        targetImg(targetImg),
        mask(targetImg->width(), targetImg->height(), maskFormat),
        elementMask(targetImg->width(), targetImg->height(), maskFormat),
        elements()
{
    setTargetImg(targetImg);
}

void nano::Analyzer::setTargetImg(const QImage* targetImg)
{
    this->targetImg = targetImg;
    elementMask = QImage(targetImg->width(), targetImg->height(), maskFormat);
    mask = QImage(targetImg->width(), targetImg->height(), maskFormat);
}

void nano::Analyzer::calculateMask(float threshold, QRect area)
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to calculate mask without target image.");

    if(area.isEmpty()) area = targetImg->rect();
    mask = QImage(area.width(), area.height(), maskFormat);
    elementMask = QImage(area.width(), area.height(), maskFormat);
    clearMasks();

    for(int y = 0; y < mask.height(); ++y)
    {
        for(int x = 0; x < mask.width(); ++x)
        {
            float value = targetImg->pixelColor(x + area.x(), y + area.y()).lightnessF();
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

void nano::Analyzer::clearMasks()
{
    mask.fill(maskColorNeg);
    elementMask.fill(maskColorNeg);
    elements.clear();
}

void nano::Analyzer::scanMaskForElements()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to scan mask without target image.");
    if(mask.isNull()) throw PRIM_EXCEPTION("Trying to scan mask for tubes before creating mask.");

    setProgress(0);
    elements.clear();
    uint32_t numberOfPixels = targetImg->width() * targetImg->height();
    bool* checkArray = new bool[numberOfPixels] { false };

    for(int y = 0; y < mask.height(); ++y)
    {
        for(int x = 0; x < mask.width(); ++x)
        {
            int idx = x + y * mask.width();
            setProgress((int)(idx / (float)numberOfPixels * 100.0f));
            if(!checkArray[idx])
            {
                std::vector<ImgPoint> points = checkPixel(x, y, checkArray);
                if(points.size() >= minPixelsInElement)
                {
                    for(const auto& point : points)
                    {
                        elementMask.setPixelColor(point.x, point.y, elementMaskColorPos);
                    }
                    elements.push_back(std::move(points));
                }
            }
        }
    }
    setProgress(100);
    delete[] checkArray;
}

std::vector<nano::ImgPoint> nano::Analyzer::checkPixel(int x, int y, bool* checkArray)
{
    std::stack<ImgPoint> pointsStack;
    std::vector<ImgPoint> resultPoints;

    ImgPoint start(x, y);
    pointsStack.push(start);
    
    while(!pointsStack.empty())
    {
        ImgPoint currPoint = pointsStack.top(); pointsStack.pop();
        int idx = currPoint.x + currPoint.y * mask.width();

        if(currPoint.x < mask.width() && currPoint.x >= 0 && currPoint.y < mask.height() && currPoint.y >= 0 && !checkArray[idx] && mask.pixelColor(currPoint.x, currPoint.y).alphaF())
        {
            checkArray[idx] = true;
            resultPoints.push_back(currPoint);

            ImgPoint adjPoint;

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
    setProgress(0);
    float threshold = 1.0f;
    float extremumThreshold = 1.0f;
    uint32_t extremumNumberOfTubes = 0u;
    calculateMask(threshold);
    scanMaskForElements();
    uint8_t stopFlag = 0u;
    uint32_t currNumberOfElements = elements.size();
    uint32_t prevNumberOfElements = currNumberOfElements;
    Tools::print("Threshold = " + Tools::floatToString(threshold, 3u) + "; Elements = " + std::to_string(currNumberOfElements));
    while(true)
    {
        if(analysisCancelled)
        {
            analysisCancelled = false;
            setProgress(0);
            clearMasks();
            elements.clear();
            Tools::print("<<<<< Analysis cancelled >>>>>", QColorConstants::Red);
            return;
        }

        threshold -= extremumDeltaStep;
        calculateMask(threshold);
        scanMaskForElements();
        currNumberOfElements = elements.size();
        Tools::print("Threshold = " + Tools::floatToString(threshold, 3u) + "; Elements = " + std::to_string(currNumberOfElements));

        if(currNumberOfElements <= prevNumberOfElements && currNumberOfElements > 0u)    // derivative is negative
        {
            if(stopFlag == 0u && extremumNumberOfTubes < currNumberOfElements)     // found local extremum
            {
                extremumThreshold = threshold + extremumDeltaStep;
                extremumNumberOfTubes = prevNumberOfElements;
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
            scanMaskForElements();

            Tools::print("<<<<< Results >>>>>", QColorConstants::Green);
            Tools::print("Extremum threshold = " + Tools::floatToString(extremumThreshold, 3u));
            Tools::print("Extremum number of elements = " + std::to_string(extremumNumberOfTubes));
            Tools::print("Min pixels in element = " + std::to_string(minPixelsInElement));
            Tools::print("Pixel size = " + Tools::floatToString(pixelSize_nm, 3u) + " (nm)");
            Tools::print("Image area = " + Tools::floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
            Tools::print("Element density = " + Tools::floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
            return;
        }
        else                                                                    // resume analysis
        {
            prevNumberOfElements = currNumberOfElements;
        }
    }
    
}

std::vector<std::tuple<float, uint, float>> nano::Analyzer::startFullRangeAnalysis(float deltaStep, QRect rect)
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to find nanotube extremum without target image.");
    Tools::print("<<< Starting full range analysis >>>", Colors::lime);
    Tools::print("Area: x=" + std::to_string(rect.x()) +
                 "; y=" + std::to_string(rect.y()) +
                 "; width=" + std::to_string(rect.width()) +
                 "; height=" + std::to_string(rect.height()), Colors::gray);
    if(rect.isEmpty()) rect = targetImg->rect();
    std::vector<std::tuple<float, uint, float>> results;
    setProgress(0);
    float threshold = 1.0f;
    float extremumThreshold = 1.0f;
    float area_mm2 = rect.width() * rect.height() * pixelSize_nm * pixelSize_nm * 0.000001;
    calculateMask(threshold, rect);
    scanMaskForElements();
    uint8_t stopFlag = 0u;
    uint32_t currNumberOfElements = elements.size();
    Tools::print("Threshold = " + Tools::floatToString(threshold, 3u) + "; Elements = " + std::to_string(currNumberOfElements));
    while(threshold > 0.0f)
    {
        if(analysisCancelled)
        {
            analysisCancelled = false;
            setProgress(0);
            mask.fill(maskColorNeg);
            elementMask.fill(maskColorNeg);
            elements.clear();
            Tools::print("<<<<< Analysis cancelled >>>>>", QColorConstants::Red);
            return std::vector<std::tuple<float, uint, float>>();
        }

        threshold -= deltaStep;
        calculateMask(threshold, rect);
        scanMaskForElements();
        currNumberOfElements = elements.size();
        results.push_back({ threshold, currNumberOfElements, currNumberOfElements / area_mm2 });
        Tools::print("Threshold = " + Tools::floatToString(threshold, 3u) + "; Elements = " + std::to_string(currNumberOfElements));
    }

    clearMasks();

    Tools::print("<<<<< Full range analysis completed >>>>>", QColorConstants::Green);

    return std::move(results);

}

void nano::Analyzer::startManualAnalysis(float threshold)
{
    setProgress(0);
    elements.clear();
    calculateMask(threshold);
    scanMaskForElements();
    Tools::print("<<<<< Results >>>>>", QColorConstants::Green);
    Tools::print("Number of elements = " + std::to_string(elements.size()));
    Tools::print("Threshold = " + Tools::floatToString(threshold, 3u));
    Tools::print("Min pixels in element = " + std::to_string(minPixelsInElement));
    Tools::print("Pixel size = " + Tools::floatToString(pixelSize_nm, 3u) + " (nm)");
    Tools::print("Image area = " + Tools::floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
    Tools::print("Element density = " + Tools::floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
}

void nano::Analyzer::startCurrentMaskAnalysis()
{
    setProgress(0);
    elements.clear();
    scanMaskForElements();
    Tools::print("<<<<< Results >>>>>", QColorConstants::Green);
    Tools::print("Number of elements = " + std::to_string(elements.size()));
    Tools::print("Min pixels in element = " + std::to_string(minPixelsInElement));
    Tools::print("Pixel size = " + Tools::floatToString(pixelSize_nm, 3u) + " (nm)");
    Tools::print("Image area = " + Tools::floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
    Tools::print("Element density = " + Tools::floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
}

float nano::Analyzer::startThresholdAnalysis(float deltaStep)
{
    // TEMP
    const uint divisionCount = 2u;
    bool horizontal = true;

    const uint stepWidth = targetImg->width() / divisionCount;
    const uint remainder = targetImg->width() % divisionCount;

    std::vector<std::vector<std::tuple<float, uint, float>>> ranges;

    for(int i = 0; i < divisionCount; ++i)
    {
        QRect area;
        if(horizontal)
        {
            area.setX(i * stepWidth);
            area.setY(0);
            area.setWidth(i == divisionCount - 1? stepWidth + remainder : stepWidth);
            area.setHeight(targetImg->height());
        }
        else
        {
            area.setX(0);
            area.setY(i * stepWidth);
            area.setWidth(targetImg->width());
            area.setHeight(i == divisionCount - 1? stepWidth + remainder : stepWidth);
        }

        std::vector<std::tuple<float, uint, float>> range(startFullRangeAnalysis(deltaStep, area));
        ranges.push_back(std::move(range));
    }

    std::vector<std::pair<float, float>> averageRange;
    averageRange.reserve(ranges[0].size());
    std::vector<std::pair<float, float>> averageDifferenceRange;
    averageDifferenceRange.reserve(ranges[0].size());

    // Calculate average range and average difference range
    for(int i = 0; i < ranges[0].size(); ++i)
    {
        std::vector<float> batch;
        batch.reserve(ranges.size());
        for(int j = 0; j < ranges.size(); ++j)
        {
            batch.push_back(std::get<2>(ranges[j][i]));
        }
        averageRange.push_back({ std::get<0>(ranges[0][i]), Tools::getAverage(batch) });
        averageDifferenceRange.push_back({ std::get<0>(ranges[0][i]), Tools::getAverageDifference(batch) });
    }

    // get max difference point between average and averageDifference
    float maxDifference = 0.0f;
    float optimalThreshold = 1.0f;
    float optimalDensity = 0.0f;
    for(int i = 0; i < averageRange.size(); ++i)
    {
        float difference = averageRange[i].second - averageDifferenceRange[i].second;
        if(difference > maxDifference)
        {
            maxDifference = difference;
            optimalThreshold = averageRange[i].first;
            optimalDensity = averageRange[i].second;
        }
    }

    Tools::print("<<<<< Results >>>>>", QColorConstants::Green);
    Tools::print("Optimal threshold = " + Tools::floatToString(optimalThreshold, 3));
    Tools::print("Optimal density (1/mm2) = " + Tools::floatToString(optimalDensity, 3));
    return optimalThreshold;
}

void nano::Analyzer::addElementAtPos(QPoint pos)
{
    if(!mask.isNull() && !elementMask.isNull())
    {
        if(mask.pixelColor(pos).alphaF())
        {
            // search for pos in existing nanotubes
            for(const Element& tube : elements)
            {
                for(const nano::ImgPoint& p : tube.points)
                {
                    if(p == pos) return;
                }
            }

            // didn't find pos in existing tubes
            // pos can be added as the new nanotube
            uint32_t numberOfPixels = targetImg->width() * targetImg->height();
            bool* checkArray = new bool[numberOfPixels] { false };
            std::vector<ImgPoint> points = checkPixel(pos.x(), pos.y(), checkArray);
            for(const auto& point : points)
            {
                elementMask.setPixelColor(point.x, point.y, elementMaskColorPos);
            }
            elements.push_back(std::move(points));
            Tools::print("Added element; Number of elements: " + std::to_string(elements.size()));
            delete[] checkArray;
        }
    }
}

void nano::Analyzer::removeElementAtPos(QPoint pos)
{
    if(!mask.isNull() && !elementMask.isNull())
    {
        if(mask.pixelColor(pos).alphaF())
        {
            // search for pos in existing nanotubes
            for(int i = 0; i < elements.size(); ++i)
            {
                for(int j = 0; j < elements[i].points.size(); ++j)
                {
                    if(elements[i].points[j] == pos)
                    {
                        for(const ImgPoint& p : elements[i].points)
                        {
                            elementMask.setPixelColor(p.x, p.y, maskColorNeg);
                        }
                        elements.erase(elements.begin() + i);
                        Tools::print("Removed tube. Remaining tubes: " + std::to_string(elements.size()));
                        return;
                    }
                }
            }
        }
    }
}

void nano::Analyzer::paintMaskAtPos(QPoint pos, float radius)
{
    if(!mask.isNull())
    {
        std::vector<ImgPoint> points(getPointsInRadius(pos, radius));
        for(const ImgPoint& point : points)
        {
            mask.setPixelColor(point, maskColorPos);
        }
    }
}

void nano::Analyzer::eraseMaskAtPos(QPoint pos, float radius)
{
    if(!mask.isNull())
    {
        std::vector<ImgPoint> points(getPointsInRadius(pos, radius));
        for(const ImgPoint& point : points)
        {
            mask.setPixelColor(point, maskColorNeg);
        }
    }
}

const QImage* nano::Analyzer::getMask() const
{
    return &mask;
}

const QImage* nano::Analyzer::getElementMask() const
{
    return &elementMask;
}

const std::vector<nano::Element>* nano::Analyzer::getElements() const
{ 
    return &elements;
}

void nano::Analyzer::setProgress(int prog)
{
    static int lastProgress = 0;
    if(prog != lastProgress)
    {
        lastProgress = prog;
        emit si_progress_changed(prog);
    }
}

float nano::Analyzer::getImageArea()
{
    assert(targetImg);
    return static_cast<float>(targetImg->width()) * pixelSize_nm * static_cast<float>(targetImg->height()) * pixelSize_nm;
}

float nano::Analyzer::getDensity()
{
    return elements.size() / getImageArea();
}

void nano::Analyzer::resetAll()
{
    targetImg = nullptr;
    mask = QImage();
    elementMask = QImage();
    setProgress(0);
    pixelSize_nm = 0.0f;
    elements.clear();
}

bool nano::Analyzer::areElementsCalculated() const
{
    return !elements.empty();
}

void nano::Analyzer::cancelAnalysis()
{
    analysisCancelled = true;
}

std::vector<nano::ImgPoint> nano::Analyzer::getPointsInRadius(Point<float> center, float radius) const
{
    std::vector<ImgPoint> points;
    QRect rect(center.x - radius, center.y - radius, 2.0f * radius, 2.0f * radius);
    center -= 0.5f;
    for(int j = rect.top(); j < rect.bottom(); ++j)
    {
        for(int i = rect.left(); i < rect.right(); ++i)
        {
            if(i < 0 || i >= targetImg->width() || j < 0 || j >= targetImg->height()) continue;
            Point<float> point(i, j);
            if(std::ceil((point - center).length()) < radius) points.push_back(std::move(point));
        }
    }
    return std::move(points);
}
