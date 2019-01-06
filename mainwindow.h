#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plotarea.h"
#include <QPushButton>
#include <QGridLayout>
#include <ble.h>
#include <deviceinfo.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



    QPushButton *getSaveButton();
    void setSaveButton(QPushButton *value);
    QScopedPointer<QGridLayout> m_layout;
    bool m_isBleButton;

private:

    Ui::MainWindow *ui;

    QScopedPointer<PlotArea> m_realPlot;
    QScopedPointer<QPushButton> m_exitButton;
    QScopedPointer<QPushButton> m_saveButton;
    QScopedPointer<QPushButton> m_connectButton;
    QScopedPointer<QPushButton> m_bleButton;
    QScopedPointer<QComboBox> m_comboboxOfDevices;
    QScopedPointer<QLabel> m_status;
    QScopedPointer<Ble> blueLE;

    QList<QString> m_searchDevices;

    QWidget *m_window;


private slots:
    void savePlot();
    void startPloting();
    void clickedBlebutton();
    void changedState(Ble::bluetoothleState state);
    void clickedConnectbutton();
    void DataHandler(int s);


signals:
    void changedDevice(int index);

};

#endif // MAINWINDOW_H
