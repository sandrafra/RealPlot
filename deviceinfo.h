#ifndef DEVICEINFO_H
#define DEVICEINFO_H
#include <QObject>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include <QList>


class DeviceInfo : public QObject
{
    Q_OBJECT
public:
    DeviceInfo();
    DeviceInfo(const QBluetoothDeviceInfo &device);
    ~DeviceInfo();
    QString getName();
    QString getAddress() const;

    QBluetoothDeviceInfo getDevice() const;
    void setDevice(const QBluetoothDeviceInfo &device);

private:
    QBluetoothDeviceInfo m_infoDevice;

};

#endif // DEVICEINFO_H
