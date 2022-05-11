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
    setFixedSize(size());
    setWindowTitle("Manual analysis config");
}

ManualAnalysisConfig::~ManualAnalysisConfig()
{
    delete ui;
}

void ManualAnalysisConfig::on_buttonBox_accepted()
{
    Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    Tools::getMainWindow()->setMinPixelInElement(ui->minPixelsInput->value());
    Tools::getMainWindow()->startManualAnalysis(ui->thresholdInput->value());
}

void ManualAnalysisConfig::accept()
{
    if(!ui->leaveWindowOpenCheckbox->isChecked()) close();
}


void ManualAnalysisConfig::on_thresholdSlider_valueChanged(int value)
{
    float fValue = value / (float)maxSliderValue;
    ui->thresholdInput->setValue(fValue);
    Tools::getMainWindow()->calculateMask(fValue);
}

void ManualAnalysisConfig::on_thresholdInput_valueChanged(double arg1)
{
    ui->thresholdSlider->setValue(static_cast<int>(arg1 * maxSliderValue));
}

void ManualAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(Tools::getMainWindow()->getPixelSize());
    ui->minPixelsInput->setValue(Tools::getMainWindow()->getMinPixelInElement());
}
