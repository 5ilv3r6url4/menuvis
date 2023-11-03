#ifndef RECIPE_ENGINEER_MODEL_H
#define RECIPE_ENGINEER_MODEL_H


#include "data_delegate.h"

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QAbstractListModel>
#include <QItemSelection>
#include <QDebug>

#include "util_uom.h"

typedef std::tuple<QModelIndex, double, QString> Recipe_Ingredient;

class Recipe_Engineer_Model : public QAbstractListModel
{
    Q_OBJECT

public:

    Recipe_Engineer_Model(QObject* parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:

    Data_Delegate* m_data;

private slots:

    void slot_update_ingredients_list();
    void slot_ingredient_changed(int index);
    void slot_request_ingredient_data(const QModelIndex& index);

    void slot_request_recipe_cost(QList<Recipe_Ingredient> recipe_ingredients);
    void slot_request_recipe_volatility(QList<Recipe_Ingredient> recipe_ingredients);
    void slot_get_tree_map_data(const QModelIndex& index);

    void slot_save_recipe(QString category, QString name, double price, double cost, double volatility, QList<Recipe_Ingredient> ingredients);

signals:

    void sig_test_category_create(bool);
    void sig_ingredient_changed(const QModelIndex&);
    void sig_request_ingredient_data_response(QString, QString, QString, QString, QString, QString, QString, double, double);

    void sig_request_recipe_cost_response(QString);
    void sig_request_recipe_volatility_response(QString);

    void sig_recipe_added(int);

    void sig_update_costing_viz(QList<std::tuple<QModelIndex, QString, double>>);
    void sig_get_treemap_data_response(QList<std::tuple<QString, double, QString>>);

};

#endif // RECIPE_ENGINEER_MODEL_H
