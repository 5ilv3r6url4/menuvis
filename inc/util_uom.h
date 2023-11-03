#ifndef UTIL_UOM_H
#define UTIL_UOM_H

#include <QString>
#include <QList>

namespace UOM_Utils
{

static double convert_uoms(QStringList source, QString from, QString to, double amount)
{
    if (!source.contains(from) || !source.contains(to))
        return -2.0;

    switch(source.indexOf(from)) {
    case 0: switch(source.indexOf(to)) {
        case 0: return amount;
        case 1: return amount * 2.0;
        case 2: return amount * 0.16666666666;
        default: return -1.0;
        }
    case 1: switch(source.indexOf(to)) {
        case 0: return amount * 0.5;
        case 1: return amount;
        case 2: return amount * 0.08333333333;
        default: return -1.0;
        }
    case 2: switch(source.indexOf(to)) {
        case 0: return amount * 2.0;
        case 1: return amount * 12.0;
        case 2: return amount;
        default: return -1.0;
        }
    case 3: switch(source.indexOf(to)) {
        case 3: return amount;
        case 4: return amount * 0.001;
        case 5: return amount * 0.00220462;
        case 6: return amount * 0.035274;
        default: return -1.0;
        }
    case 4: switch(source.indexOf(to)) {
        case 3: return amount * 1000.0;
        case 4: return amount;
        case 5: return amount * 2.20462;
        case 6: return amount * 35.274;
        default: return -1.0;
        }
    case 5: switch(source.indexOf(to)) {
        case 3: return amount * 453.592;
        case 4: return amount * 0.453592;
        case 5: return amount;
        case 6: return amount * 16.0;
        default: return -1.0;
        }
    case 6: switch(source.indexOf(to)) {
        case 3: return amount * 28.3495;
        case 4: return amount * 0.0283495;
        case 5: return amount * 0.0625;
        case 6: return amount;
        default: return -1.0;
        }
    case 7: switch(source.indexOf(to)) {
        case 7: return amount;
        case 8: return amount * 0.001;
        case 9: return amount * 0.00105669;
        case 10: return amount * 0.000264172;
        default: return -1.0;
        }
    case 8: switch(source.indexOf(to)) {
        case 7: return amount * 1000.0;
        case 8: return amount;
        case 9: return amount * 1.05669;
        case 10: return amount * 0.264172;
        default: return -1.0;
        }
    case 9: switch(source.indexOf(to)) {
        case 7: return amount * 946.353;
        case 8: return amount * 0.946353;
        case 9: return amount;
        case 10: return amount * 0.25;
        default: return -1.0;
        }
    case 10: switch(source.indexOf(to)) {
        case 7: return amount * 3785.41;
        case 8: return amount * 3.78541;
        case 9: return amount * 4.0;
        case 10: return amount;
        default: return -1.0;
        }
    default: return -2.0;
    }
}

static double convert_prices(QStringList source, QString from, QString to, double amount)
{
    if (!source.contains(from) || !source.contains(to))
        return -2.0;

    switch(source.indexOf(from)) {
    case 0: switch(source.indexOf(to)) {
        case 0: return amount;
        case 1: return amount / 2.0;
        case 2: return amount / 0.16666666666;
        default: return -1.0;
        }
    case 1: switch(source.indexOf(to)) {
        case 0: return amount / 0.5;
        case 1: return amount;
        case 2: return amount / 0.08333333333;
        default: return -1.0;
        }
    case 2: switch(source.indexOf(to)) {
        case 0: return amount / 2.0;
        case 1: return amount / 12.0;
        case 2: return amount;
        default: return -1.0;
        }
    case 3: switch(source.indexOf(to)) {
        case 3: return amount;
        case 4: return amount / 0.001;
        case 5: return amount / 0.00220462;
        case 6: return amount / 0.035274;
        default: return -1.0;
        }
    case 4: switch(source.indexOf(to)) {
        case 3: return amount / 1000.0;
        case 4: return amount;
        case 5: return amount / 2.20462;
        case 6: return amount / 35.274;
        default: return -1.0;
        }
    case 5: switch(source.indexOf(to)) {
        case 3: return amount / 453.592;
        case 4: return amount / 0.453592;
        case 5: return amount;
        case 6: return amount / 16.0;
        default: return -1.0;
        }
    case 6: switch(source.indexOf(to)) {
        case 3: return amount / 28.3495;
        case 4: return amount / 0.0283495;
        case 5: return amount / 0.0625;
        case 6: return amount;
        default: return -1.0;
        }
    case 7: switch(source.indexOf(to)) {
        case 7: return amount;
        case 8: return amount / 0.001;
        case 9: return amount / 0.00105669;
        case 10: return amount / 0.000264172;
        default: return -1.0;
        }
    case 8: switch(source.indexOf(to)) {
        case 7: return amount / 1000.0;
        case 8: return amount;
        case 9: return amount / 1.05669;
        case 10: return amount / 0.264172;
        default: return -1.0;
        }
    case 9: switch(source.indexOf(to)) {
        case 7: return amount / 946.353;
        case 8: return amount / 0.946353;
        case 9: return amount;
        case 10: return amount / 0.25;
        default: return -1.0;
        }
    case 10: switch(source.indexOf(to)) {
        case 7: return amount / 3785.41;
        case 8: return amount / 3.78541;
        case 9: return amount / 4.0;
        case 10: return amount;
        default: return -1.0;
        }
    default: return -2.0;
    }
}

static QList<int> get_related_uom(QStringList source, QString key)
{
    switch(source.indexOf(key)) {
    case 0: return {0, 1, 2};
    case 1: return {0, 1, 2};
    case 2: return {0, 1, 2};
    case 3: return {3, 4, 5, 6};
    case 4: return {3, 4, 5, 6};
    case 5: return {3, 4, 5, 6};
    case 6: return {3, 4, 5, 6};
    case 7: return {7, 8, 9, 10};
    case 8: return {7, 8, 9, 10};
    case 9: return {7, 8, 9, 10};
    case 10: return {7, 8, 9, 10};
    default: return {};
    }
}
}

#endif // UTIL_UOM_H
