#ifndef INGREDIENTS_STAGING_MODEL_H
#define INGREDIENTS_STAGING_MODEL_H

#include "data_delegate.h"

#include "xlsxdocument.h"
#include "xlsxcell.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QRegularExpression>
#include <QAbstractTableModel>
#include <QDebug>

#include "util_uom.h"

class Ingredients_Staging_Model : public QAbstractTableModel
{
    Q_OBJECT

public:

    Ingredients_Staging_Model(bool type, QObject *parent = nullptr);
    ~Ingredients_Staging_Model();

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:

    Data_Delegate*  m_data;
    bool            m_switch;

    QList<int> m_ingredients_out; //rows

    int delete_later;

public slots:

    void slot_clear_inputs();
    void slot_clear_outputs();
    void slot_upload_gfs(QString fpath);
    void slot_upload_sysco(QString fpath);

    void slot_request_staging_input_ingredient_data(const QModelIndex& index);

    void slot_remove_input_ingredient(const QModelIndex& index);
    void slot_add_output_ingredient(QString supplier, QString category, QString name, QString brand, QString uom, double amount,
                                    double price, QString number, QString description);

    void slot_auto_detected_existing_ingredients_response();
    void slot_save_output_to_working(QString date);

signals:

    void sig_request_staging_input_ingredient_data_response(QString, QString, QString, QString, QString, double, double, QString, QString);
    void sig_auto_detected_existing_ingredients();
    void sig_error_message(QString);

};

#endif // INGREDIENTS_STAGING_MODEL_H
