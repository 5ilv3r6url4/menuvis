#ifndef INGREDIENTS_DATA_H
#define INGREDIENTS_DATA_H

#include <QString>
#include <QList>
#include <QStringList>

namespace Ingredients_Data_Globals
{
static QStringList INGREDIENT_CATEGORIES({"Proteins", "Dairy/Eggs", "Produce", "Frozen", "Canned/Dry", "Spices", "Other"});
static QStringList ACCEPTED_UOMS({"PR", "UN", "DZ", "GR", "KG", "LB", "OZ", "ML", "L", "QT", "GAL"});
}

class Ingredients_Data
{

    static const int NUM_INGREDIENT_ATTRIBUTES = 10;

public:

    Ingredients_Data();
    ~Ingredients_Data();

    struct Ingredient
    {
        QString     s_supplier;
        QString     s_category;
        QString     s_number;
        QString     s_name;
        QString     s_description;
        QString     s_brand;
        QString     s_uom;
        double      s_price;
        double      s_amount;
        double      s_volatility;

        // index into map by year, access histories by month (int)
        // intended to both dipslay avg prices by month and display full year low level granularity by month
        // important: prompt confirmations for deletions, remind users that losing this data is expensive!
        QMap<int, QList<QList<double>>> s_historic_volatilities;
        QMap<int, QList<QList<double>>> s_historic_prices;

        Ingredient();
        ~Ingredient();

        Ingredient(QString supplier, QString category, QString name, QString brand, QString uom, double amount,
                   double price, QString number = "-1", QString description = "", double volatility = 0.0);
        Ingredient(Ingredient* copy_from);
    };

    friend class Data_Delegate;

private:

    bool contains(QString supplier, QString brand, QString name, QString number);
    int contains(Ingredients_Data::Ingredient* ingredient);
    bool contains_staging_input(QString supplier, QString brand, QString name, QString number);
    bool contains_staging_output(QString supplier, QString brand, QString name, QString number) { return false; }

    QList<Ingredients_Data::Ingredient*> m_ingredients;

    // emptied after each upload with qDeleteAll() to minimize footprint
    QList<Ingredients_Data::Ingredient*> m_staging_input_ingredients;
    QList<Ingredients_Data::Ingredient*> m_staging_output_ingredients;
};

#endif // INGREDIENTS_DATA_H
