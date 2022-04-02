#include "currentmaskanalysisconfig.h"
#include "ui_currentmaskanalysisconfig.h"
#include "tools.hpp"

CurrentMaskAnalysisConfig::CurrentMaskAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurrentMaskAnalysisConfig)
{
    ui->setupUi(this);
    setWindowTitle("Current mask analysis config");
    setLocale(QLocale::C);
}

CurrentMaskAnalysisConfig::~CurrentMaskAnalysisConfig()
{
    delete ui;
}

void CurrentMaskAnalysisConfig::on_buttonBox_accepted()
{
    Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    Tools::getMainWindow()->setMinPixelInTube(ui->minPixelsInput->value());
    Tools::getMainWindow()->startCurrentMaskAnalysis();
}

void CurrentMaskAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(Tools::getMainWindow()->getPixelSize());
    ui->minPixelsInput->setValue(Tools::getMainWindow()->getMinPixelInTube());
}

void CurrentMaskAnalysisConfig::accept()
{
    if(!ui->leaveWindowOpenCheckbox->isChecked()) close();
}

