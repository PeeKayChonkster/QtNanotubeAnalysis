#ifndef IMAGECONFIG_H
#define IMAGECONFIG_H

#include <QDialog>

namespace Ui {
class ImageConfig;
}

class ImageConfig : public QDialog
{
    Q_OBJECT

public:
    explicit ImageConfig(QWidget *parent = nullptr);
    ~ImageConfig();

private slots:
    void on_buttonBox_accepted();

protected:
    void showEvent(QShowEvent* event) override;

private:
    Ui::ImageConfig *ui;
};

#endif // IMAGECONFIG_H
