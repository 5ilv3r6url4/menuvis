#ifndef UTIL_QCP_AXIS_DOLLARS_H
#define UTIL_QCP_AXIS_DOLLARS_H

#include <QDebug>

#include <qcustomplot.h>

class Util_QCP_Axis_Dollars : public QCPAxisTicker
{
public:

    Util_QCP_Axis_Dollars();

    QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override;
};

#endif // UTIL_QCP_AXIS_DOLLARS_H
