# RealTimePlot
Application created using Qt version 5.12.0.

## About app
The application is used to acquire biomedical data. Data is received via a Bluetooth Low Energy connection. 

After connecting to the device, the data is added to the container (QVector). Pressing the Start button adds a widget with a graph where data is displayed in real time. The application also has three functionalities that allow to stop drawing a chart, save plot to a JPG and save the value to a .txt file.

![Algorithm schema](/realplot2.png)
![Algorithm schema](/realplot.png)


## Installation 
To run app without using QtCreator:

on macOS:
```
git clone [path to repo]
qmake
make
open *app
```
on Linux:
```
git clone [path to repo]
qmake
make
./BioPlot
```
## Technologies:
 
 ### Qt contains developer tools that were used to create the project:
 
 Qt Creator - a cross-platform development environment for C ++, Phyton and QML
 
 qmake - for building applications for different target platforms
 
 Qt Desiner - Qt Designer is the Qt tool for designing and building graphical user interfaces (GUIs)
 
 ### Classes:
 
QCustomPlot - full documentation https://www.qcustomplot.com

QBluetooth - http://doc.qt.io/qt-5/qbluetooth.html
 
 
