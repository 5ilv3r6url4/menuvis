#include "recipe_engineer_model.h"

Recipe_Engineer_Model::Recipe_Engineer_Model(QObject* parent) :
    QAbstractListModel(parent)
{
    this->m_data = &Data_Delegate::get_instance();

    connect(this->m_data,   SIGNAL(sig_ingredient_added()),
            this,           SLOT(slot_update_ingredients_list()));

    connect(this->m_data,   SIGNAL(sig_ingredient_changed(int)),
            this,           SLOT(slot_ingredient_changed(int)));
}

int Recipe_Engineer_Model::rowCount(const QModelIndex &) const
{
    return m_data->get_num_ingredients();
}

QVariant Recipe_Engineer_Model::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) return { };

    const Ingredients_Data::Ingredient* temp_ingredient = m_data->get_ingredient(index.row());

    if (role == Qt::DisplayRole && index.column() == 0)
        return "[" + temp_ingredient->s_category + "] " + temp_ingredient->s_name + ": " + temp_ingredient->s_description;

    else if (role == Qt::ToolTipRole)
        return temp_ingredient->s_supplier;

    else return {};
}

void Recipe_Engineer_Model::slot_update_ingredients_list()
{
    beginInsertRows( {}, m_data->get_num_ingredients(), m_data->get_num_ingredients());
    endInsertRows();
}

void Recipe_Engineer_Model::slot_ingredient_changed(int index)
{
    QModelIndex temp_qindex = this->index(index, 0);
    emit dataChanged(temp_qindex, temp_qindex);
    emit sig_ingredient_changed(temp_qindex);
}

void Recipe_Engineer_Model::slot_request_ingredient_data(const QModelIndex& index)
{
    const Ingredients_Data::Ingredient* temp_item = m_data->get_ingredient(index.row());
    emit sig_request_ingredient_data_response(temp_item->s_supplier, temp_item->s_category, temp_item->s_number, temp_item->s_name,
                                                 temp_item->s_description, temp_item->s_brand, temp_item->s_uom, temp_item->s_price, temp_item->s_volatility);
}

void Recipe_Engineer_Model::slot_request_recipe_cost(QList<Recipe_Ingredient> recipe_ingredients)
{
    double running_cost = 0.0;

    int temp_index = 0;
    double temp_amount = 0.0;
    QString temp_uom = "";
    QModelIndex temp_qmi;

    QList<std::tuple<QModelIndex, QString, double>> costing_info;

    for (int i = 0; i < recipe_ingredients.size(); ++i) {

        temp_qmi = std::get<0>(recipe_ingredients[i]);
        temp_index = temp_qmi.row();
        temp_amount = std::get<1>(recipe_ingredients[i]);
        temp_uom = std::get<2>(recipe_ingredients[i]);

        const Ingredients_Data::Ingredient* temp_item = m_data->get_ingredient(temp_index);

        double new_price = UOM_Utils::convert_prices(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_item->s_uom, temp_uom, temp_item->s_price);
        double cost = new_price * temp_amount;

        costing_info.append(std::make_tuple(temp_qmi, temp_item->s_name, cost));

        running_cost += cost;
    }

    emit sig_request_recipe_cost_response(QString::number(running_cost, 'f', 2));
    emit sig_update_costing_viz(costing_info);
}

void Recipe_Engineer_Model::slot_request_recipe_volatility(QList<Recipe_Ingredient> recipe_ingredients)
{
    double running_volatility = 0.0;
    double running_cost = 0.0;
    int temp_index = 0;
    double temp_amount = 0.0;
    QString temp_uom = "";



    for (int i = 0; i < recipe_ingredients.size(); ++i) {

        temp_index = std::get<0>(recipe_ingredients[i]).row();
        temp_amount = std::get<1>(recipe_ingredients[i]);
        temp_uom = std::get<2>(recipe_ingredients[i]);

        const Ingredients_Data::Ingredient* temp_item = m_data->get_ingredient(temp_index);

        double actual_volatility = UOM_Utils::convert_prices(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_item->s_uom, temp_uom, temp_item->s_volatility);
        double new_price = UOM_Utils::convert_prices(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_item->s_uom, temp_uom, temp_item->s_price);

        running_volatility += actual_volatility * temp_amount;
        running_cost += new_price * temp_amount;
    }

    emit sig_request_recipe_volatility_response(QString::number((running_volatility/running_cost) * 100, 'f', 2));
}

void Recipe_Engineer_Model::slot_save_recipe(QString category, QString name, double price, double cost, double volatility, QList<Recipe_Ingredient> ingredients)
{
    QString s = QDate::currentDate().toString("dd-MM-yyyy");
    QStringList day_year_month = s.split("-");
    if (m_data->add_recipe(day_year_month[0].toInt(), day_year_month[1].toInt(), day_year_month[2].toInt(), name, category, price, cost, ingredients, volatility)) {
        emit sig_recipe_added(m_data->get_num_recipes());
    }
}

void Recipe_Engineer_Model::slot_get_tree_map_data(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    QList<std::tuple<QString, double, QString>> triplet_data = m_data->get_tree_map_data(index.row());

    emit sig_get_treemap_data_response(triplet_data);
}
