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

void ManualAnalysisConfig::on_buttonBox_accepted()
{
    tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    tools::getMainWindow()->setMinPixelInTube(ui->minPixelsInput->value());
    tools::getMainWindow()->startManualAnalysis(ui->thresholdInput->value());
}

void ManualAnalysisConfig::accept()
{
    if(!ui->leaveWindowOpenCheckbox->isChecked()) close();
}


void ManualAnalysisConfig::on_thresholdSlider_valueChanged(int value)
{
    float fValue = value / (float)maxSliderValue;
    ui->thresholdInput->setValue(fValue);
    tools::getMainWindow()->calculateMask(fValue);
}

void ManualAnalysisConfig::on_thresholdInput_valueChanged(double arg1)
{
    ui->thresholdSlider->setValue(static_cast<int>(arg1 * maxSliderValue));
}

void ManualAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(tools::getMainWindow()->getPixelSize());
}
