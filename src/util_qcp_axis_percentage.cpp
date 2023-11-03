#include "util_qcp_axis_percentage.h"

Util_QCP_Axis_Percentage::Util_QCP_Axis_Percentage() { }

QString Util_QCP_Axis_Percentage::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
    QLocale l;
    QString percentage = "%" + l.toString(tick, 'f', 2);
    return percentage;
}
