#include "recipes_data.h"

Recipes_Data::Recipes_Data()
{
    m_recipe_categories.append("--Select Category--");
    m_recipe_categories.append("Prep Recipe");
    m_recipe_categories.append("--Create New--");
}
Recipes_Data::~Recipes_Data()
{
    qDeleteAll(m_recipes);
}

bool Recipes_Data::contains_name(QString name)
{
    for (int i = 0; i < m_recipes.size(); ++i) {
        if (m_recipes[i]->s_name == name)
            return true;
    }
    return false;
}

bool Recipes_Data::Recipe::contains_ingredient(QString number, QString supplier)
{
    for (int i = 0; i < s_ingredients.size(); ++i) {
        if (std::get<0>(s_ingredients[i])->s_number == number &&
                std::get<0>(s_ingredients[i])->s_supplier == supplier) {
//            std::get<0>(s_ingredients[i])->s_volatility = ingredient.s_volatility;
//            std::get<0>(s_ingredients[i])->s_price = ingredient.s_price;
            return true;
        }
    }

    return false;
}

bool Recipes_Data::contains_category(QString category)
{
    if (m_recipe_categories.contains(category))
            return true;
    return false;
}

Recipes_Data::Recipe::Recipe() { }
Recipes_Data::Recipe::~Recipe() { }

Recipes_Data::Recipe::Recipe(QString name, QString category, double price, double cost,
             double volatility, QList<std::tuple<Ingredients_Data::Ingredient *, double, QString>> ingredients)
{
    this->s_name = name;
    this->s_category = category;
    this->s_price = price;
    this->s_cost = cost;
    this->s_volatility = volatility;
    this->s_ingredients = ingredients;
}
