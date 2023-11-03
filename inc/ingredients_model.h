#ifndef INGREDIENTS_MODEL_H
#define INGREDIENTS_MODEL_H

#include "data_delegate.h"

#include "xlsxdocument.h"
#include "xlsxcell.h"

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QAbstractTableModel>
#include <QMap>
#include <QDebug>

class Ingredients_Model : public QAbstractTableModel
{
    Q_OBJECT

public:

    Ingredients_Model(QObject *parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:

    void print_ingredients_data();

    Data_Delegate*           m_data;

public slots:

    void slot_request_ingredient_data(const QModelIndex& index);

    void slot_request_aggregate_category_volatility(QString category);
    void slot_request_granular_category_pricing_histories(QString category);
    void slot_request_ingredient_volatility_histories(const QModelIndex &index);

    void slot_update_ingredients_list();
    void slot_ingredient_changed_response(int row);

signals:

    void sig_update_viz();
    void sig_request_ingredient_data_response(QString, QString, QString, QString, QString, double, double, double, QString, QString);
    void sig_request_aggregate_category_volatility_response(double);
    void sig_request_granular_category_pricing_histories_response(QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>>>);
    void sig_request_ingredient_volatility_histories_response(QString, QMap<int, QList<QList<double>>>);

};

#endif // INGREDIENTS_MODEL_H
