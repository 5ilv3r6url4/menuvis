#include "recipes_model.h"

Recipes_Model::Recipes_Model(QObject* parent) :
    QAbstractListModel(parent)
{
    m_data = &Data_Delegate::get_instance();
    connect(this->m_data,   SIGNAL(sig_recipe_changed(int)),
            this,           SLOT(slot_recipe_changed(int)));
}

int Recipes_Model::rowCount(const QModelIndex &) const
{
    return m_data->get_num_recipes();
}

QVariant Recipes_Model::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return { };

    const Recipes_Data::Recipe* temp_recipe = m_data->get_recipe(index.row());

    switch (index.column()) {
    case 0: return "[$" + QString::number(temp_recipe->s_cost, 'f', 2) + "/$" + QString::number(temp_recipe->s_price, 'f', 2) + "] " + temp_recipe->s_name;
    default: return {};
    };
}

void Recipes_Model::slot_recipe_changed(int row)
{
    QModelIndex temp_qindex = this->index(row, 0);
    emit dataChanged(temp_qindex, temp_qindex);
    emit sig_update_yoself_my_dudes(temp_qindex);
}

void Recipes_Model::slot_update_recipes(int index)
{
    beginInsertRows( {}, index, index);
    endInsertRows();
}

void Recipes_Model::slot_request_recipe_data(const QModelIndex& index)
{
    const Recipes_Data::Recipe* temp_recipe = m_data->get_recipe(index.row());

    emit sig_request_recipe_data_response(temp_recipe->s_name, temp_recipe->s_category, temp_recipe->s_price, temp_recipe->s_cost, temp_recipe->s_volatility,
                                          temp_recipe->s_ingredients);
}

void Recipes_Model::slot_request_granular_cost_price_histories(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    const Recipes_Data::Recipe* temp_recipe = m_data->get_recipe(index.row());
    emit sig_request_granular_cost_price_histories_response(temp_recipe->s_name, temp_recipe->s_historic_costs, temp_recipe->s_historic_prices);
}

void Recipes_Model::slot_request_granular_volatility_histories(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    const Recipes_Data::Recipe* temp_recipe = m_data->get_recipe(index.row());
    emit sig_request_granular_volatility_histories_response(temp_recipe->s_name, temp_recipe->s_historic_volatilities);
}
