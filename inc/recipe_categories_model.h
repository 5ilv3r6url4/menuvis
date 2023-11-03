#ifndef RECIPE_CATEGORIES_MODEL_H
#define RECIPE_CATEGORIES_MODEL_H

#include "data_delegate.h"

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QAbstractListModel>
#include <QDebug>

class Recipe_Categories_Model : public QAbstractListModel
{
    Q_OBJECT

public:

    Recipe_Categories_Model(QObject* parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:

    Data_Delegate*       m_data;

public slots:

    void slot_create_category(QString category);

signals:

    void sig_create_category_response(int);

};

#endif // RECIPE_CATEGORIES_MODEL_H
