#include "configwindow.h"
#include "ui_configwindow.h"
#include "tools.hpp"

ConfigWindow::ConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    setWindowTitle("Config");
    setLocale(QLocale::C);
    ui->brushSizeSlider->setMinimum(brushMinSize);
    ui->brushSizeSlider->setMaximum(brushMaxSize);
    ui->brushSizeInput->setMinimum(brushMinSize);
    ui->brushSizeInput->setMaximum(brushMaxSize);
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::on_buttonBox_accepted()
{
    tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    tools::getMainWindow()->setBrushRadius(ui->brushSizeInput->value());
}

void ConfigWindow::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(tools::getMainWindow()->getPixelSize());
    ui->brushSizeInput->setValue(tools::getMainWindow()->getBrushRadius());
    ui->brushSizeSlider->setValue(ui->brushSizeInput->value());
}


void ConfigWindow::on_brushSizeSlider_valueChanged(int value)
{
    ui->brushSizeInput->setValue(value);
}

