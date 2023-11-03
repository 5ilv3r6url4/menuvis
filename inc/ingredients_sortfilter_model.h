#ifndef INGREDIENTS_SORTFILTER_MODEL_H
#define INGREDIENTS_SORTFILTER_MODEL_H

#include <QSortFilterProxyModel>

class Ingredients_SortFilter_Model : public QSortFilterProxyModel
{

    Q_OBJECT

public:

    Ingredients_SortFilter_Model(QObject *parent = 0);

protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:

    bool m_sort_supplier;
    bool m_sort_category;
    bool m_sort_number;
    bool m_sort_name;
    bool m_sort_description;
    bool m_sort_brand;
    bool m_sort_unit;
    bool m_sort_price;
    bool m_sort_volatility;

    QString m_filter_string;

};

#endif // INGREDIENTS_SORTFILTER_MODEL_H
