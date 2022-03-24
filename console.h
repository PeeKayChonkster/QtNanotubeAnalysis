#ifndef CONSOLE_H
#define CONSOLE_H

#include <QMainWindow>

namespace Ui {
class Console;
}

class Console : public QMainWindow
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console();

    void print(QString line = "", QColor textColor = {255, 255, 255, 255});

private slots:
    void on_actionClear_triggered();

    void on_actionSave_to_file_triggered();

private:
    Ui::Console *ui;
};

#endif // CONSOLE_H
