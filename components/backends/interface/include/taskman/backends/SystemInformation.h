#ifndef SystemInformation_INCLUDED
#define SystemInformation_INCLUDED

#include <QString>

struct SystemInformation {
    QString machineType;
    QString osKernelType;
    QString osKernelVersion;
    QString osFamilyName;
    QString osName;
    QString osVersion;
};

#endif // SystemInformation_INCLUDED
