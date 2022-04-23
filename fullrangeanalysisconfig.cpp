#include "fullrangeanalysisconfig.h"
#include "ui_fullrangeanalysisconfig.h"
#include "tools.hpp"
#include <QMessageBox>

FullRangeAnalysisConfig::FullRangeAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullRangeAnalysisConfig)
{
    ui->setupUi(this);
    setWindowTitle("Full range analysis config");
    setLocale(QLocale::C);
    setFixedSize(size());
}

FullRangeAnalysisConfig::~FullRangeAnalysisConfig()
{
    delete ui;
}

void FullRangeAnalysisConfig::on_buttonBox_accepted()
{
    if(ui->deltaStepInput->value() <= 0.0f)
    {
        QMessageBox::critical(this, "Cancelling", "Delta step shoud be only positive! Cancelling...");
        return;
    }
    Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    Tools::getMainWindow()->setMinPixelInElement(ui->minPixelsInput->value());
    Tools::getMainWindow()->startFullRangeAnalysis(ui->deltaStepInput->value(), ui->createTableCheckbox->isChecked());
}

void FullRangeAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(Tools::getMainWindow()->getPixelSize());
    ui->minPixelsInput->setValue(Tools::getMainWindow()->getMinPixelInElement());
}

