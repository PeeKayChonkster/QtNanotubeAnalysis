#include "imageconfig.h"
#include "ui_imageconfig.h"
#include "tools.hpp"

ImageConfig::ImageConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageConfig)
{
    ui->setupUi(this);
    setFixedSize(size());
    setLocale(QLocale::C);
    setWindowTitle("Image config");
}

ImageConfig::~ImageConfig()
{
    delete ui;
}

void ImageConfig::on_buttonBox_accepted()
{
    tools::getMainWindow()->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
}

void ImageConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(tools::getMainWindow()->analyzer.pixelSize_nm);
}

