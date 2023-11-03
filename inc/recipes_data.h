#ifndef RECIPES_DATA_H
#define RECIPES_DATA_H

#include "ingredients_data.h"

#include <QList>
#include <QString>
#include <QPair>

class Recipes_Data
{
public:
    Recipes_Data();
    ~Recipes_Data();

    struct Recipe
    {
        QString     s_name;
        QString     s_category;
        double      s_price;
        double      s_cost;
        double      s_volatility;

        QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>> s_ingredients;
        QMap<int, QList<QList<double>>> s_historic_prices;
        QMap<int, QList<QList<double>>> s_historic_costs;
        QMap<int, QList<QList<double>>> s_historic_volatilities;

        Recipe();
        ~Recipe();

        Recipe(QString name, QString category, double price, double cost,
               double volatility, QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>> ingredients);

        bool contains_ingredient(QString number, QString supplier);
    };

    friend class Data_Delegate;

private:

    QList<Recipes_Data::Recipe*>    m_recipes;
    QStringList                     m_recipe_categories;

    bool contains_name(QString name);
    bool contains_category(QString category);
};

#endif // RECIPES_DATA_H
