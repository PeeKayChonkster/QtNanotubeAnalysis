#include "thresholdanalysisconfig.h"
#include "ui_thresholdanalysisconfig.h"
#include "tools.hpp"
#include <QMessageBox>

ThresholdAnalysisConfig::ThresholdAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThresholdAnalysisConfig)
{
    ui->setupUi(this);
    setWindowTitle("Threshold analysis config");
    setLocale(QLocale::C);
    setFixedSize(size());
}

ThresholdAnalysisConfig::~ThresholdAnalysisConfig()
{
    delete ui;
}

void ThresholdAnalysisConfig::on_buttonBox_accepted()
{
    if(ui->pixelSizeInput->value() <= 0.0f)
    {
        QMessageBox::critical(this, "Warning!", "Pixel size must be greater than zero. Cancelling...");
        return;
    }
    if(ui->deltaStepInput->value() <= 0.0f)
    {
        QMessageBox::critical(this, "Warning!", "Delta step must must be greater than zero. Cancelling...");
        return;
    }

    Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    Tools::getMainWindow()->setMinPixelInElement(ui->minPixelsInput->value());
    Tools::getMainWindow()->startThresholdAnalysis(ui->deltaStepInput->value());
}

void ThresholdAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(Tools::getMainWindow()->getPixelSize());
    ui->minPixelsInput->setValue(Tools::getMainWindow()->getMinPixelInElement());
}

