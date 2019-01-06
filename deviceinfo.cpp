#include "deviceinfo.h"
#include <qbluetoothuuid.h>

DeviceInfo::DeviceInfo(){
}

DeviceInfo::DeviceInfo(const QBluetoothDeviceInfo &device)
{
    m_infoDevice = device;
}



QString DeviceInfo::getName(){
    return  m_infoDevice.name();
}

QString DeviceInfo::getAddress() const
{

#ifdef Q_OS_MAC

    return m_infoDevice.deviceUuid().toString();
#else
    return m_infoDevice.address().toString();
#endif
}

QBluetoothDeviceInfo DeviceInfo::getDevice() const
{
    return m_infoDevice;
}

void DeviceInfo::setDevice(const QBluetoothDeviceInfo &device)
{
    m_infoDevice = device;
}

DeviceInfo::~DeviceInfo(){
}

