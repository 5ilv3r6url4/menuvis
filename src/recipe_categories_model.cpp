#include "recipe_categories_model.h"

Recipe_Categories_Model::Recipe_Categories_Model(QObject* parent) :
    QAbstractListModel(parent)
{
    m_data = &Data_Delegate::get_instance();
}

int Recipe_Categories_Model::rowCount(const QModelIndex &) const
{
    return m_data->get_num_recipe_categories();
}

QVariant Recipe_Categories_Model::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || index.column() != 0) return { };
    return m_data->get_recipe_category(index.row());
}

Qt::ItemFlags Recipe_Categories_Model::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (index.row() == 0) {
            flags.setFlag(Qt::ItemIsEnabled, false).setFlag(Qt::ItemIsSelectable, false);
        }
    else {
        flags.setFlag(Qt::ItemIsEnabled, true).setFlag(Qt::ItemIsSelectable, true);
    }
    return flags;
}

void Recipe_Categories_Model::slot_create_category(QString category)
{
    int new_index = m_data->add_new_category(category);
    if (new_index != -1) {
        beginInsertRows( {}, m_data->get_num_recipe_categories(), m_data->get_num_recipe_categories());
        endInsertRows();
    }
    emit sig_create_category_response(new_index);
}
