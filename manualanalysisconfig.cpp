#include "manualanalysisconfig.h"
#include "ui_manualanalysisconfig.h"
#include "mainwindow.h"
#include "tools.hpp"

ManualAnalysisConfig::ManualAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualAnalysisConfig)
{
    ui->setupUi(this);
    setLocale(QLocale::C);
}

ManualAnalysisConfig::~ManualAnalysisConfig()
{
    delete ui;
}

void ManualAnalysisConfig::accept()
{
    tools::getMainWindow()->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
    tools::getMainWindow()->analyzer.minPixelsInTube = ui->minPixelsInput->value();
    tools::getMainWindow()->startManualAnalysis(ui->thresholdInput->value());
    if(!ui->leaveWindowOpenCheckbox->isChecked()) close();
}


void ManualAnalysisConfig::on_thresholdSlider_valueChanged(int value)
{
    float fValue = value / (float)maxSliderValue;
    ui->thresholdInput->setValue(fValue);
    tools::getMainWindow()->analyzer.calculateMask(fValue);
    tools::getMainWindow()->setMask();
    tools::getMainWindow()->tubeMaskVisible = false;
    tools::getMainWindow()->renderImages();
}

void ManualAnalysisConfig::on_thresholdInput_valueChanged(double arg1)
{
    ui->thresholdSlider->setValue(static_cast<int>(arg1 * maxSliderValue));
}

