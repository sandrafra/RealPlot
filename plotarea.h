#ifndef PLOTAREA_H
#define PLOTAREA_H

#include <QWidget>
#include <qcustomplot.h>
#include <QFile>


namespace Ui {
class PlotArea;
}

class PlotArea : public QCustomPlot
{
    Q_OBJECT

public:
    explicit PlotArea(QWidget *parent = nullptr);
    ~PlotArea();

    QString m_lineFromFile;
    QVector<qreal> m_myVector;
    QScopedPointer<QPushButton> m_pauseButton;
    QScopedPointer<QPushButton> m_acquisitionButton;
    QPushButton *m_startButton;

    bool isAcquisition = false;
    void readFile();


private:
    Ui::PlotArea *ui;

    void resizeXAxis();
    const QTime  m_currentTime = QTime::currentTime();

    QTimer *myTimer;
    static qreal m_secValue;
    static qreal m_timeValue;
    static qreal m_xValue;

    QVector<qreal> m_XData;
    QVector<qreal> m_YData;


public  slots:
    void update();
    void pauseButton_pressed();
    void acqusitonButton_pressed();
    void addPlot();
    void writeFile();


};

#endif // PLOTAREA_H
