#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class ChartWindow;
}

class ChartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    ~ChartWindow();

    void addSeries(const std::vector<std::pair<float, float>>& series, QString legendName, QString xName, QString yName, QColor color);
    void clear();

private:
    Ui::ChartWindow *ui;

    QChart* chart;
    QChartView* chartView;
};

#endif // CHARTWINDOW_H
