#ifndef UTIL_QCP_AXIS_PERCENTAGE_H
#define UTIL_QCP_AXIS_PERCENTAGE_H

#include <QDebug>

#include <qcustomplot.h>

class Util_QCP_Axis_Percentage : public QCPAxisTicker
{
public:

    Util_QCP_Axis_Percentage();

    QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override;
};

#endif // UTIL_QCP_AXIS_PERCENTAGE_H
