#ifndef INGREDIENTS_VIEWER_H
#define INGREDIENTS_VIEWER_H

#include "ingredients_staging_viewer.h"
#include "ingredients_model.h"

#include <QWidget>

#include "util_colors.h"
#include "util_qcp_axis_dollars.h"
#include "util_qcp_axis_percentage.h"

namespace Ui {
class Ingredients_Viewer;
}

class Ingredients_Viewer : public QWidget
{
    Q_OBJECT

public:

    explicit Ingredients_Viewer(QWidget *parent = 0);
    ~Ingredients_Viewer();

    Ingredients_Staging_Viewer* m_view_ingredient_staging;

private:

    Ui::Ingredients_Viewer* ui;
    Ingredients_Model*      m_model;

    QItemSelectionModel*    m_selection_model;
    QModelIndex             m_current_selection;
    QString                 m_current_category;

    QList<QModelIndex>      m_pricing_history_model_link;
    QCPTextElement*         m_price_history_title;
    QCPTextElement*         m_volatility_history_title;

private slots:

    void slot_open_ingredient_staging();
    void slot_selection_change(const QItemSelection selected, const QItemSelection deselected);
    void slot_request_ingredient_data_response(QString supplier, QString category, QString name, QString brand, QString uom,
                                                             double amount, double price, double volatility, QString number, QString description);
    void slot_request_granular_category_pricing_histories_response(QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double> > > > > pricing_histories);
    void slot_request_ingredient_volatility_histories_response(QString ingredient_name, QMap<int, QList<QList<double> > > volatility_histories);

    void slot_update_viz_response();

signals:

    void sig_request_ingredient_data(const QModelIndex&);
    void sig_request_aggregate_category_volatility(QString);
    void sig_request_granular_category_pricing_histories(QString);
    void sig_request_ingredient_volatility_histories(QModelIndex);

};

#endif // INGREDIENTS_VIEWER_H
