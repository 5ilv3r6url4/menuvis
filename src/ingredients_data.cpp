#include "ingredients_data.h"

Ingredients_Data::Ingredients_Data() { }
Ingredients_Data::~Ingredients_Data()
{
    qDeleteAll(m_ingredients);
}

bool Ingredients_Data::contains(QString supplier, QString brand, QString name, QString number)
{
    for (int i = 0; i < m_ingredients.size(); ++i) {
        Ingredient* temp_ingredient = m_ingredients[i];
        if (number == "-1" || number == "9999999") {
            if (temp_ingredient->s_supplier == supplier &&
                    temp_ingredient->s_brand == brand &&
                    temp_ingredient->s_name == name)
                return true;
        }
        else if (temp_ingredient->s_supplier == supplier &&
                 temp_ingredient->s_number == number)
            return true;
    }
    return false;
}

int Ingredients_Data::contains(Ingredients_Data::Ingredient* ingredient)
{
    for (int i = 0; i < m_ingredients.size(); ++i) {
        Ingredient* temp_ingredient = m_ingredients[i];
        if (ingredient->s_number == "9999999") {
            if (temp_ingredient->s_supplier == ingredient->s_supplier &&
                    temp_ingredient->s_brand == ingredient->s_brand &&
                    temp_ingredient->s_name == ingredient->s_name)
                return i;
        }
        else if (temp_ingredient->s_supplier == ingredient->s_supplier &&
                 temp_ingredient->s_number == ingredient->s_number)
            return i;
    }
    return -1;
}

bool Ingredients_Data::contains_staging_input(QString supplier, QString brand, QString name, QString number)
{
    for (int i = 0; i < m_staging_input_ingredients.size(); ++i) {
        Ingredient* temp_ingredient = m_staging_input_ingredients[i];
        if (number == "-1") {
            if (temp_ingredient->s_supplier == supplier &&
                    temp_ingredient->s_brand == brand &&
                    temp_ingredient->s_name == name)
                return true;
        }
        else if (temp_ingredient->s_supplier == supplier &&
                 temp_ingredient->s_number == number)
            return true;
    }
    return false;
}

Ingredients_Data::Ingredient::Ingredient() { }
Ingredients_Data::Ingredient::~Ingredient() { }

Ingredients_Data::Ingredient::Ingredient(QString supplier, QString category, QString name, QString brand, QString unit, double amount,
                                         double price, QString number, QString description, double volatility)
{
    this->s_supplier = supplier;
    this->s_category = category;
    this->s_number = number;
    this->s_name = name;
    this->s_description = description;
    this->s_brand = brand;
    this->s_uom = unit;
    this->s_amount = amount;
    this->s_volatility = volatility;
    this->s_price = price;
}

Ingredients_Data::Ingredient::Ingredient(Ingredients_Data::Ingredient* copy_from)
{
    this->s_supplier = copy_from->s_supplier;
    this->s_category = copy_from->s_category;
    this->s_number = copy_from->s_number;
    this->s_name = copy_from->s_name;
    this->s_description = copy_from->s_description;
    this->s_brand = copy_from->s_brand;
    this->s_uom = copy_from->s_uom;
    this->s_amount = copy_from->s_amount;
    this->s_volatility = copy_from->s_volatility;
    this->s_price = copy_from->s_price;
}
