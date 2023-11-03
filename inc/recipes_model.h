#ifndef RECIPES_MODEL_H
#define RECIPES_MODEL_H

#include "data_delegate.h"

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QAbstractListModel>
#include <QDebug>

class Recipes_Model : public QAbstractListModel
{
    Q_OBJECT

public:

    Recipes_Model(QObject* parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:

    Data_Delegate*       m_data;

public slots:

    void slot_update_recipes(int index);
    void slot_request_recipe_data(const QModelIndex& index);
    void slot_request_granular_cost_price_histories(const QModelIndex& index);
    void slot_request_granular_volatility_histories(const QModelIndex& index);
    void slot_recipe_changed(int row);

signals:

    void sig_request_recipe_data_response(QString, QString, double, double, double, QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>>);
    void sig_request_granular_cost_price_histories_response(QString, QMap<int, QList<QList<double>>>, QMap<int, QList<QList<double>>>);
    void sig_request_granular_volatility_histories_response(QString, QMap<int, QList<QList<double>>>);
    void sig_recipe_changed(const QModelIndex&);
    void sig_update_yoself_my_dudes(const QModelIndex&);

};

#endif // RECIPES_MODEL_H
