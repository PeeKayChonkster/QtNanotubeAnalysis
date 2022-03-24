#include "manualanalysisconfig.h"
#include "ui_manualanalysisconfig.h"
#include "mainwindow.h"

ManualAnalysisConfig::ManualAnalysisConfig(MainWindow *parent) :
    QDialog(parent),
    parent(parent),
    ui(new Ui::ManualAnalysisConfig)
{
    ui->setupUi(this);
}

ManualAnalysisConfig::~ManualAnalysisConfig()
{
    delete ui;
}

void ManualAnalysisConfig::accept()
{
    parent->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
    parent->analyzer.minPixelsInTube = ui->minPixelsInput->value();
    parent->startManualAnalysis(ui->thresholdInput->value());
    if(!ui->leaveWindowOpenCheckbox->isChecked()) close();
}


void ManualAnalysisConfig::on_thresholdSlider_valueChanged(int value)
{
    float fValue = value / (float)maxSliderValue;
    ui->thresholdInput->setValue(fValue);
    parent->analyzer.calculateMask(fValue);
    parent->setMask();
    parent->tubeMaskVisible = false;
    parent->renderImages();
}

void ManualAnalysisConfig::on_thresholdInput_valueChanged(double arg1)
{
    ui->thresholdSlider->setValue(static_cast<int>(arg1 * maxSliderValue));
}

