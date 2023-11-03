#include "util_qcp_axis_dollars.h"

Util_QCP_Axis_Dollars::Util_QCP_Axis_Dollars() { }

QString Util_QCP_Axis_Dollars::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
    QLocale l;
    QString dollar = "$" + l.toString(tick, 'f', 2);
    return dollar;
}
