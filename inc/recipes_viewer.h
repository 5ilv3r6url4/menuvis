#ifndef RECIPES_VIEWER_H
#define RECIPES_VIEWER_H

#include "recipe_engineer_viewer.h"
#include "recipes_model.h"

#include <QWidget>

#include "util_qcp_axis_dollars.h"
#include "util_qcp_axis_percentage.h"
#include "donutbreakdownchart.h"

namespace Ui {
class Recipes_Viewer;
}

class Recipes_Viewer : public QWidget
{
    Q_OBJECT

public:

    explicit Recipes_Viewer(QWidget *parent = 0);
    ~Recipes_Viewer();

private:

    void update_recipe_composition_viz(QString name,  QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>> ingredients);
    void update_ingredient_info(Ingredients_Data::Ingredient* ingredient);

    Ui::Recipes_Viewer*     ui;
    Recipes_Model*          m_model;
    Recipe_Engineer_Viewer* m_dialog_engineer;

    QItemSelectionModel*    m_selection_model;
    QModelIndex             m_current_selection;

    QCPTextElement*         m_cost_price_history_title;
    QCPTextElement*         m_volatility_history_title;

    DonutBreakdownChart*    m_composition_viz;

private slots:

    void slot_open_recipe_engineer();
    void slot_selection_change(const QItemSelection selected, const QItemSelection deselected);
    void slot_request_recipe_data_response(QString name, QString category, double price, double cost, double volatility,
                                           QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>> ingredients);
    void slot_request_granular_cost_price_histories_response(QString recipe_name, QMap<int, QList<QList<double>>> cost_histories, QMap<int, QList<QList<double>>> price_histories);
    void slot_request_granular_volatility_histories_response(QString ingredient_name, QMap<int, QList<QList<double>>> volatility_histories);
    void slot_aight_we_be_updating(const QModelIndex &index);

signals:

    void sig_request_recipe_data(const QModelIndex&);
    void sig_request_granular_cost_price_histories(const QModelIndex&);
    void sig_request_granular_volatility_histories(const QModelIndex&);

signals:

};

#endif // RECIPES_VIEWER_H
