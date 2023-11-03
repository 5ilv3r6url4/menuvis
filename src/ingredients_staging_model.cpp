#include "ingredients_staging_model.h"

Ingredients_Staging_Model::Ingredients_Staging_Model(bool type, QObject* parent) :
    QAbstractTableModel(parent)
{
    m_data = &Data_Delegate::get_instance();
    m_switch = type;
    delete_later = 0;
}

Ingredients_Staging_Model::~Ingredients_Staging_Model() {}

int Ingredients_Staging_Model::rowCount(const QModelIndex &) const
{
        return m_data->get_num_staging_ingredients(m_switch);
}

int Ingredients_Staging_Model::columnCount(const QModelIndex &) const
{
    if (m_switch)
        return m_data->get_num_staging_ingredient_attributes();
    if (!m_switch)
        return m_data->get_num_staging_ingredient_attributes() + 1;
}

QVariant Ingredients_Staging_Model::data(const QModelIndex &index, int role) const
{
    if (m_switch) {
        if (role != Qt::DisplayRole)
            return {};

        const Ingredients_Data::Ingredient* temp_item = m_data->get_staging_ingredient(m_switch, index.row());
        switch (index.column()) {
        case 0: return temp_item->s_supplier;
        case 1: return temp_item->s_category;
        case 2: return temp_item->s_name;
        case 3: return temp_item->s_description;
        case 4: return temp_item->s_brand;
        case 5: return temp_item->s_price;
        case 6: return temp_item->s_amount;
        case 7: return temp_item->s_uom;
        case 8: return temp_item->s_number;
        default: return {};
        };
    }

    else if (!m_switch) {
        if (role != Qt::DisplayRole)
            return {};


        const Ingredients_Data::Ingredient* temp_item = m_data->get_staging_ingredient(m_switch, index.row());

        double previous_price = m_data->get_staging_ingredient_previous_price(index.row());
        if (previous_price < 0.0)
            previous_price = temp_item->s_price;

        switch (index.column()) {
        case 0: return temp_item->s_supplier;
        case 1: return temp_item->s_category;
        case 2: return temp_item->s_name;
        case 3: return temp_item->s_description;
        case 4: return temp_item->s_brand;
        case 5: return previous_price;
        case 6: return temp_item->s_price;
        case 7: return temp_item->s_amount;
        case 8: return temp_item->s_uom;
        case 9: return temp_item->s_number;
        default: return {};
        };
    }
}

QVariant Ingredients_Staging_Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (m_switch) {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};

        switch (section) {
        case 0: return "Supplier";
        case 1: return "Category";
        case 2: return "Name";
        case 3: return "Description";
        case 4: return "Brand";
        case 5: return "Price (/UOM)";
        case 6: return "Amount (UOM)";
        case 7: return "UOM";
        case 8: return "Item Number";
        default: return {};
        };
    }

    else if (!m_switch) {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};

        switch (section) {
        case 0: return "Supplier";
        case 1: return "Category";
        case 2: return "Name";
        case 3: return "Description";
        case 4: return "Brand";
        case 5: return "Price (/UOM)";
        case 6: return "New Price (/UOM)";
        case 7: return "Amount (UOM)";
        case 8: return "UOM";
        case 9: return "Item Number";
        default: return {};
        }
    }
    return {};
}

Qt::ItemFlags Ingredients_Staging_Model::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (!m_ingredients_out.isEmpty()) {
        if (m_ingredients_out.contains(index.row())) {
            flags.setFlag(Qt::ItemIsEnabled, false).setFlag(Qt::ItemIsSelectable, false);
        }
    }
    else {
        flags.setFlag(Qt::ItemIsEnabled, true).setFlag(Qt::ItemIsSelectable, true);
    }
    return flags;
}

void Ingredients_Staging_Model::slot_clear_inputs()
{
    if (m_switch) {
        beginResetModel();
        m_data->clear_staging_input_ingredients();
        m_ingredients_out.clear();
        endResetModel();
    }
}

// uploads GFS inventory pricing sheet into program via parsing and building items
void Ingredients_Staging_Model::slot_upload_gfs(QString fpath)
{
    int count_potentially_bad = 0;
    int count_bad = 0;

    // read the xlsx price sheet
    QXlsx::Document xlsx(fpath);
    int nrows = xlsx.dimension().lastRow();

    if (nrows == 0) {
        emit sig_error_message("XLSX file is empty!");
        return;
    }

    // check if this is really a gfs pricing guide
    QString test_category = xlsx.cellAt(2, 1)->value().toString();
    QString test_item_number = xlsx.cellAt(2, 2)->value().toString();
    QString test_description = xlsx.cellAt(2, 3)->value().toString();
    QString test_unit_size = xlsx.cellAt(2, 4)->value().toString();

    if (test_category != "Category" ||
            test_item_number != "Item Number" ||
            test_description != "Item Description" ||
            test_unit_size != "Unit size") {
        emit sig_error_message("PRICING GUIDE NOT GFS OR FORMAT CHANGED");
        return;
    }

    // item values
    QString temp_supplier = "GFS";  // item.supplier
    QString temp_category;          // item.category
    QString temp_number;            // item.id
    QString temp_name;              // item.name
    QString temp_description;       // item.description
    QString temp_brand;             // item.brand
    QString temp_uom;               // item.unit
    double temp_price;              // item.price
    double temp_amount;             // item.amount

    // intermediate values
    QString temp_item_description;  // item name and description
    QString temp_unit_size;         // unit uom and amount
    QString temp_case_price_uom;    // /kg or /Case
    double temp_case_price;         // CAD in xlsx is styled
    double temp_pack_size;          // units per Case
    double temp_size;               // unit amount

    // string splits
    QStringList split_item_description;
    QStringList split_case_price;


    // regex
    QRegularExpression rxp("[0-9]+[a-z]", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match_unit_size;

    // read price sheet and build each item
    // column numbers are organized by known keys
    for (int row = 3; row <= nrows; ++row) {
        temp_category = xlsx.cellAt(row, 1)->value().toString();
        temp_number = xlsx.cellAt(row, 2)->value().toString();
        temp_item_description = xlsx.cellAt(row, 3)->value().toString();
        temp_unit_size = xlsx.cellAt(row, 4)->value().toString();
        temp_pack_size = xlsx.cellAt(row, 5)->value().toDouble();
        temp_brand = xlsx.cellAt(row, 7)->value().toString();
        temp_case_price = xlsx.cellAt(row, 9)->value().toDouble();
        temp_case_price_uom = xlsx.cellAt(row, 10)->value().toString();
        temp_case_price_uom.remove(0,1);

        // name and description
        split_item_description = temp_item_description.split(",");
        temp_name = split_item_description.value(0);
        split_item_description.removeAt(0);
        temp_description = split_item_description.join(",");
        temp_description.remove(0, 1);

        // unit size and uom
        match_unit_size = rxp.match(temp_unit_size);
        if (match_unit_size.hasMatch()) {
            int offset = match_unit_size.capturedEnd(0) - 1;
            temp_size = temp_unit_size.left(offset).toDouble();
            temp_uom = temp_unit_size.mid(offset);
        }
        else {
            temp_size = -1;
            temp_uom = "error";
        }

        // fix weird uom bug GFS produces
        if (temp_uom == "KGA")
            temp_uom = "KG";
        if (temp_uom == "UNA")
            temp_uom = "UN";
        if (temp_uom == "GA")
            temp_uom = "GR";
        if (temp_uom == "G")
            temp_uom = "GR";
        if (temp_uom == "Z")
            temp_uom = "OZ";

        // order amount
        temp_amount = temp_size * temp_pack_size;

        // case or unit price
        if (temp_case_price_uom == "Case")
            temp_price = temp_case_price / temp_amount;
        else if (temp_uom != temp_case_price_uom.toUpper() || !Ingredients_Data_Globals::ACCEPTED_UOMS.contains(temp_uom, Qt::CaseInsensitive)) {
            temp_uom = "INVALID";
            temp_amount = -1.0;
            temp_price = temp_case_price;
            count_bad++;
        }
        else
            temp_price = temp_case_price;

        if (temp_price <= 0.02)
            count_potentially_bad++;

        QString rounded_price_temp = QString::number(temp_price, 'f', 2);
        temp_price = rounded_price_temp.toDouble();

        // make new item and add to pricing data
        int status = m_data->add_staging_ingredient(m_switch, temp_supplier, temp_category, temp_name, temp_brand, temp_uom, temp_amount, temp_price, temp_number, temp_description);
        if (status == 1) {
            beginInsertRows( {}, row, row );
            endInsertRows();
        }
        else if (status == 2) {
            m_ingredients_out.append(row - 3);
            beginInsertRows( {}, row, row );
            endInsertRows();
            emit sig_auto_detected_existing_ingredients();
        }
    }

    QString error_message = "";
    if (count_bad > 0) error_message += "Bad ingredients detected: " + QString::number(count_bad) + "\n";
    if (count_potentially_bad > 0) error_message += "Potentially bad ingredients detected: " + QString::number(count_potentially_bad) + "\n";
    if (!error_message.isEmpty()) emit sig_error_message(error_message);

}

// uploads SYSCO inventory pricing sheet into program via parsing and building items
void Ingredients_Staging_Model::slot_upload_sysco(QString fpath)
{
    int count_potentially_bad = 0;
    int count_bad = 0;
    int count_ns = 0;

    // read the xlsx price sheet
    QXlsx::Document xlsx(fpath);
    int nrows = xlsx.dimension().lastRow();

    if (nrows == 0) {
        emit sig_error_message("XLSX file is empty!");
        return;
    }

    // check if this is really a sysco pricing guide
    QString test_item_number = xlsx.cellAt(2, 1)->value().toString();
    QString test_pack = xlsx.cellAt(2, 2)->value().toString();
    QString test_size = xlsx.cellAt(2, 3)->value().toString();
    QString test_brand = xlsx.cellAt(2, 4)->value().toString();

    if (test_item_number != "Item" ||
            test_pack != "Pack" ||
            test_size != "Size" ||
            test_brand != "Brand") {
        emit sig_error_message("PRICING GUIDE NOT SYSCO OR FORMAT CHANGED");
        return;
    }

    // item values
    QString temp_supplier = "SYSCO";    // item.supplier
    QString temp_category = "MISSING";  // item.category
    QString temp_number;                // item.id
    QString temp_name = "MISSING";      // item.name
    QString temp_description;           // item.description
    QString temp_brand;                 // item.brand
    QString temp_uom = "PROG";          // item.unit
    double temp_price;                  // item.price
    double temp_amount;                 // item.amount

    // intermediate values
    QString temp_unit_size;         // unit uom and amount
    double temp_case_price;         // CAD
    double temp_pack_size;          // units per Case
    double temp_size = -1.0;        // unit amount

    // string splits
    QStringList split_unit_size;

    // regex
    QRegularExpression rxp_uom("[0-9]+[a-z]", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression rxp_sysco_formatting("/[~%&\\;:\"',<>?#\\s]", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match_unit_size;

    // read price sheet and build each item
    // column numbers are organized by known keys
    for (int row = 3; row <= nrows; ++row) {
        temp_number = xlsx.cellAt(row, 1)->value().toString();
        temp_pack_size = xlsx.cellAt(row, 2)->value().toDouble();
        temp_unit_size = xlsx.cellAt(row, 3)->value().toString();
        temp_brand = xlsx.cellAt(row, 4)->value().toString();
        temp_description = xlsx.cellAt(row, 5)->value().toString();
        temp_case_price = xlsx.cellAt(row, 9)->value().toDouble();

        // fixing sysco formatting ="value"
        temp_description.replace(rxp_sysco_formatting, "");
        temp_number.replace(rxp_sysco_formatting, "");
        temp_unit_size.replace(rxp_sysco_formatting, "");
        temp_brand.replace(rxp_sysco_formatting, "");

        // unit size and uom
        split_unit_size = temp_unit_size.split(" ");
        if (split_unit_size.size() > 1) {
            temp_size = split_unit_size.value(0).toDouble();
            temp_uom = split_unit_size.value(1);
        }
        else {
            match_unit_size = rxp_uom.match(temp_unit_size);
            if (match_unit_size.hasMatch()) {
                int offset = match_unit_size.capturedEnd(0) - 1;
                temp_size = temp_unit_size.left(offset).toDouble();
                temp_uom = temp_unit_size.mid(offset);
            }
        }

        // fix SYSCO non-uniform formatting
        if (temp_uom == "PC")
            temp_uom = "UN";
        if (temp_uom == "EA")
            temp_uom = "UN";
        if (temp_uom == "CT")
            temp_uom = "UN";

        if (temp_uom == "G")
            temp_uom = "GR";
        if (temp_uom == "GM")
            temp_uom = "GR";
        if (temp_uom == "GA")
            temp_uom = "GR";

        if (temp_uom == "LTR.")
            temp_uom = "L";
        if (temp_uom == "LTR")
            temp_uom = "L";
        if (temp_uom == "LT")
            temp_uom = "L";

        if (!Ingredients_Data_Globals::ACCEPTED_UOMS.contains(temp_uom, Qt::CaseInsensitive) || temp_size < 0.0) {
            temp_uom = "INVALID";
            temp_amount = -1.0;
            temp_price = temp_case_price;
            count_bad++;
        }

        else {
            temp_amount = temp_size * temp_pack_size;
            temp_price = temp_case_price/temp_amount;
        }

        if (temp_price == 0.0 && xlsx.cellAt(row, 6)) {
            count_ns++;
        }

        if (temp_price <= 0.02 && temp_price > 0.0)
            count_potentially_bad++;

        QString rounded_price_temp = QString::number(temp_price, 'f', 2);
        temp_price = rounded_price_temp.toDouble();

        // make new item and add to pricing data
        int status = m_data->add_staging_ingredient(m_switch, temp_supplier, temp_category, temp_name, temp_brand, temp_uom, temp_amount, temp_price, temp_number, temp_description);
        if (status == 1) {
            beginInsertRows( {}, row, row );
            endInsertRows();
        }
        else if (status == 2) {
            m_ingredients_out.append(row - 3);
            beginInsertRows( {}, row, row );
            endInsertRows();
            emit sig_auto_detected_existing_ingredients();
        }
    }

    QString error_message = "";
    if (count_bad > 0) error_message += "Bad ingredients detected: " + QString::number(count_bad) + "\n";
    if (count_potentially_bad > 0) error_message += "Potentially bad ingredients detected: " + QString::number(count_potentially_bad) + "\n";
    if (count_ns > 0) error_message += "N/S ingredients imported: " + QString::number(count_ns) + "\n";
    if (!error_message.isEmpty()) emit sig_error_message(error_message);
}

void Ingredients_Staging_Model::slot_auto_detected_existing_ingredients_response()
{
    beginInsertRows( {}, m_data->get_num_staging_ingredients(m_switch), m_data->get_num_staging_ingredients(m_switch) );
    endInsertRows();
}

void Ingredients_Staging_Model::slot_request_staging_input_ingredient_data(const QModelIndex& index)
{
    const Ingredients_Data::Ingredient* temp_item = m_data->get_staging_ingredient(m_switch, index.row());
    emit sig_request_staging_input_ingredient_data_response(temp_item->s_supplier, temp_item->s_category, temp_item->s_name, temp_item->s_brand,
                                                            temp_item->s_uom, temp_item->s_amount, temp_item->s_price, temp_item->s_number, temp_item->s_description);

}

void Ingredients_Staging_Model::slot_add_output_ingredient(QString supplier, QString category, QString name, QString brand, QString uom, double amount,
                                                           double price, QString number, QString description)
{
    beginInsertRows( {}, m_data->get_num_staging_ingredients(m_switch), m_data->get_num_staging_ingredients(m_switch) );
    m_data->add_staging_ingredient(m_switch, supplier, category, name, brand, uom, amount, price, number, description);
    endInsertRows();
}

void Ingredients_Staging_Model::slot_remove_input_ingredient(const QModelIndex& index)
{
    m_ingredients_out.append(index.row());
}

void Ingredients_Staging_Model::slot_clear_outputs()
{
    if (!m_switch) {
        beginResetModel();
        m_data->clear_staging_output_ingredients();
        endResetModel();
    }
}

void Ingredients_Staging_Model::slot_save_output_to_working(QString date)
{
    if (!m_switch) {
    QStringList day_year_month = date.split("-");
    m_data->save_staged_to_working_ingredients(day_year_month[0].toInt() + delete_later, day_year_month[1].toInt(), day_year_month[2].toInt());
    beginResetModel();
    m_data->clear_staging_output_ingredients();
    endResetModel();
    delete_later++;
    }
}
