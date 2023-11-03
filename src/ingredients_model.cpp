#include "ingredients_model.h"

Ingredients_Model::Ingredients_Model(QObject* parent) :
    QAbstractTableModel(parent)
{
    m_data = &Data_Delegate::get_instance();
    connect(m_data, SIGNAL(sig_ingredient_added()),
            this,   SLOT(slot_update_ingredients_list()));
    connect(m_data, SIGNAL(sig_ingredient_changed(int)),
            this,   SLOT(slot_ingredient_changed_response(int)));
}

int Ingredients_Model::rowCount(const QModelIndex &) const
{
    return m_data->get_num_ingredients();
}

int Ingredients_Model::columnCount(const QModelIndex &) const
{
    return m_data->get_num_ingredient_attributes();
}

QVariant Ingredients_Model::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    const Ingredients_Data::Ingredient* temp_item = m_data->get_ingredient(index.row());
    switch (index.column()) {
    case 0: return temp_item->s_supplier;
    case 1: return temp_item->s_category;
    case 2: return temp_item->s_name;
    case 3: return temp_item->s_description;
    case 4: return temp_item->s_brand;
    case 5: return temp_item->s_price;
    case 6: return temp_item->s_uom;
    case 7: return temp_item->s_volatility;
    case 8: return temp_item->s_number;
    default: return {};
    };
}
QVariant Ingredients_Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
    case 0: return "Supplier";
    case 1: return "Category";
    case 2: return "Name";
    case 3: return "Description";
    case 4: return "Brand";
    case 5: return "Price/UOM";
    case 6: return "UOM";
    case 7: return "Volatility";
    case 8: return "Item Number";
    default: return {};
    }
}

bool Ingredients_Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (!hasIndex(index.row(), index.column()))
        return false;

    if (index.column() == 0 || index.column() == 1 || index.column() == 5 || index.column() == 6 || index.column() == 7 || index.column() == 8)
        return false;

    switch (index.column()) {
    case 0: return m_data->set_ingredient_attribute(index.row(), "supplier", value.toString(), {});
    case 1: return m_data->set_ingredient_attribute(index.row(), "category", value.toString(), {});
    case 2: return m_data->set_ingredient_attribute(index.row(), "name", value.toString(), {});
    case 3: return m_data->set_ingredient_attribute(index.row(), "description", value.toString(), {});
    case 4: return m_data->set_ingredient_attribute(index.row(), "brand", value.toString(), {});
    case 5: return m_data->set_ingredient_attribute(index.row(), "price", {}, value.toDouble());
    case 7: return m_data->set_ingredient_attribute(index.row(), "volatility", {} ,value.toDouble());
    default: return false;
    }
}

Qt::ItemFlags Ingredients_Model::flags(const QModelIndex &index) const
{
    if (index.column() == 6 || index.column() == 8)
        return QAbstractTableModel::flags(index);

    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void Ingredients_Model::slot_update_ingredients_list()
{
    beginInsertRows( {}, m_data->get_num_ingredients(), m_data->get_num_ingredients());
    endInsertRows();
    emit sig_update_viz();
}

void Ingredients_Model::slot_ingredient_changed_response(int row)
{
    QModelIndex temp_qindex = this->index(row, 0);
    emit dataChanged(temp_qindex, temp_qindex);
    emit sig_update_viz();
}

void Ingredients_Model::print_ingredients_data()
{
    // writing out new data sheet
    QXlsx::Document xlsx_write;
    QXlsx::Format format_text;
    format_text.setFontName("Arial");
    format_text.setFontSize(16);

    xlsx_write.write(1, 1, "menuVis Output from Gamma Implementation", format_text);
    xlsx_write.write(2, 1, "item.supplier", format_text);
    xlsx_write.write(2, 2, "item.category", format_text);
    xlsx_write.write(2, 3, "item.id", format_text);
    xlsx_write.write(2, 4, "item.name", format_text);
    xlsx_write.write(2, 5, "item.description", format_text);
    xlsx_write.write(2, 6, "item.brand", format_text);
    xlsx_write.write(2, 7, "item.unit", format_text);
    xlsx_write.write(2, 8, "item.amount", format_text);
    xlsx_write.write(2, 9, "item.price", format_text);

    for (int row = 0; row < m_data->get_num_ingredients(); ++row) {
        // writing out new data sheet
        xlsx_write.write(row + 3, 1, m_data->get_ingredient(row)->s_supplier, format_text);
        xlsx_write.write(row + 3, 2, m_data->get_ingredient(row)->s_category, format_text);
        xlsx_write.write(row + 3, 3, m_data->get_ingredient(row)->s_number, format_text);
        xlsx_write.write(row + 3, 4, m_data->get_ingredient(row)->s_name, format_text);
        xlsx_write.write(row + 3, 5, m_data->get_ingredient(row)->s_description, format_text);
        xlsx_write.write(row + 3, 6, m_data->get_ingredient(row)->s_brand, format_text);
        xlsx_write.write(row + 3, 7, m_data->get_ingredient(row)->s_uom, format_text);
        xlsx_write.write(row + 3, 8, m_data->get_ingredient(row)->s_volatility);
        xlsx_write.write(row + 3, 9, m_data->get_ingredient(row)->s_price);
    }
    xlsx_write.saveAs("/Users/silverburla/Desktop/output_gamma_0.xlsx");
}

void Ingredients_Model::slot_request_ingredient_data(const QModelIndex& index)
{
    const Ingredients_Data::Ingredient* temp_item = m_data->get_ingredient(index.row());
    emit sig_request_ingredient_data_response(temp_item->s_supplier, temp_item->s_category, temp_item->s_name, temp_item->s_brand,
                                              temp_item->s_uom, temp_item->s_amount, temp_item->s_price, temp_item->s_volatility, temp_item->s_number, temp_item->s_description);
}

void Ingredients_Model::slot_request_aggregate_category_volatility(QString category)
{
    double aggregate_volatilities = m_data->aggregate_ingredient_category_volatilties(category);
    emit sig_request_aggregate_category_volatility_response(aggregate_volatilities);
}

void Ingredients_Model::slot_request_ingredient_volatility_histories(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    const Ingredients_Data::Ingredient* temp_ingredient = m_data->get_ingredient(index.row());
    emit sig_request_ingredient_volatility_histories_response(temp_ingredient->s_name, temp_ingredient->s_historic_volatilities);
}


void Ingredients_Model::slot_request_granular_category_pricing_histories(QString category)
{
    QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>>> s_historic_prices;
    QList<int> category_ingredient_indices = m_data->get_ingredient_indices_by_category(category);

    for (int i = 0; i < category_ingredient_indices.size(); ++i) {
        const Ingredients_Data::Ingredient* temp_ingredient = m_data->get_ingredient(category_ingredient_indices[i]);
        std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>> historic_price = std::make_tuple(index(category_ingredient_indices[i], 0),
                                                                                                           temp_ingredient->s_name,
                                                                                                           temp_ingredient->s_historic_prices);
        s_historic_prices.append(historic_price);
    }
    emit sig_request_granular_category_pricing_histories_response(s_historic_prices);
}
