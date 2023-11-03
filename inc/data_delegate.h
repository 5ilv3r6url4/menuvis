#ifndef DATA_DELEGATE_H
#define DATA_DELEGATE_H

#include "ingredients_data.h"
#include "recipes_data.h"

#include <QString>
#include <QStringList>
#include <QItemDelegate>
#include <QDebug>
#include <QDate>

#include "util_uom.h"

class Data_Delegate : public QItemDelegate
{
    Q_OBJECT

public:

    static Data_Delegate& get_instance();

    Data_Delegate(Data_Delegate const&)           = delete;
    void operator=(Data_Delegate const&)          = delete;

    // ==============================================================================
    // INGREDIENTS SECTION
    int get_num_ingredients();
    int get_num_ingredient_attributes();
    const Ingredients_Data::Ingredient* get_ingredient(int row);
    bool add_ingredient(QString supplier, QString category, QString name, QString brand, QString uom, double amount,
                        double price, QString number = "-1", QString description = "", double volatility = 0.0);
    bool set_ingredient_attribute(int index, QString attribute, QString string_value = "", double num_value = -1.0);

    int get_num_staging_ingredients(bool flag);
    int get_num_staging_ingredient_attributes();
    const Ingredients_Data::Ingredient* get_staging_ingredient(bool flag, int row);
    int add_staging_ingredient(bool flag, QString supplier, QString category, QString name, QString brand, QString uom, double amount,
                                double price, QString number = "-1", QString description = "");
    void clear_staging_input_ingredients();
    void clear_staging_output_ingredients();
    void save_staged_to_working_ingredients(int day, int month, int year);

    double get_staging_ingredient_previous_price(int row);

    double aggregate_ingredient_category_volatilties(QString category);
    QList<int> get_ingredient_indices_by_category(QString category);


    // ==============================================================================
    // RECIPES SECTION
    int get_num_recipe_categories();
    QString get_recipe_category(int index);
    int add_new_category(QString category);

    int get_num_recipes();
    bool add_recipe(int day, int month, int year, QString name, QString category, double price, double cost,
                         QList<std::tuple<QModelIndex, double, QString> > ingredients, double volatility = 0.0);
    Recipes_Data::Recipe* get_recipe(int index);
    void update_recipe(int day, int month, int year, int row);

    QList<std::tuple<QString, double, QString>> get_tree_map_data(int row);

private:

    Data_Delegate() {}
    ~Data_Delegate();

    Ingredients_Data*   m_ingredients_data;
    Recipes_Data*       m_recipes_data;

signals:

    void sig_ingredient_added();
    void sig_ingredient_changed(int);
    void sig_recipe_changed(int);

};

#endif // DATA_DELEGATE_H
