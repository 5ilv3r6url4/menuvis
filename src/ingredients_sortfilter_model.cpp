#include "ingredients_sortfilter_model.h"

Ingredients_SortFilter_Model::Ingredients_SortFilter_Model(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_sort_supplier = false;
    m_sort_category = false;
    m_sort_number = false;
    m_sort_name = false;
    m_sort_description = false;
    m_sort_brand = false;
    m_sort_unit = false;
    m_sort_price = false;
    m_sort_volatility = false;

    m_filter_string = "";
}

bool Ingredients_SortFilter_Model::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{

}

bool Ingredients_SortFilter_Model::lessThan(const QModelIndex &left, const QModelIndex &right) const
{

}
