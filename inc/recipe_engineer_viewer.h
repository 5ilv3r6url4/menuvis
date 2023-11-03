#ifndef RECIPE_ENGINEER_VIEWER_H
#define RECIPE_ENGINEER_VIEWER_H

#include "recipe_engineer_model.h"
#include "dialog_recipe_category_add.h"
#include "recipe_categories_model.h"

#include <QtCharts>
#include <QWidget>

#include "util_colors.h"
#include "util_uom.h"
#include "util_qcp_axis_dollars.h"

typedef std::tuple<QModelIndex, double, QString> Recipe_Ingredient;

namespace Ui {
class Recipe_Engineer_Viewer;
}

class Recipe_Engineer_Viewer : public QWidget
{
    Q_OBJECT

public:

    explicit Recipe_Engineer_Viewer(QWidget *parent = 0);
    ~Recipe_Engineer_Viewer();

    void response_recipe_info_change();

private:

    void response_selection_change(QString category, QString name, QString supplier, QString brand, QString number, QString uom, double price, double volatility);
    void response_ingredient_info_change(QString category, QString name, QString supplier, QString brand, QString number, QString uom, double price, double volatility);
    void response_ingredient_add(QString supplier, QString category, QString number, QString name, QString description,
                                 QString brand, QString uom, double price, double volatility);

    void response_ingredient_remove(const QModelIndex& index);
    void response_ingredient_amount_change(const QModelIndex& index, double amount);
    void response_ingredient_uom_change(const QModelIndex& index, QLineEdit* amount, QString new_uom);

    void clear_widgets(QLayout* layout);

    Ui::Recipe_Engineer_Viewer*     ui;
    Dialog_Recipe_Category_Add*     m_dialog_create_category;
    Recipe_Categories_Model*        m_cbox_model;
    Recipe_Engineer_Model*          m_model;
    QItemSelectionModel*            m_selection_model;
    QModelIndex                     m_current_selection;

    QList<Recipe_Ingredient>  m_out_ingredients;

    QChart*             m_chart_ingredients_costs;
    QList<QBarSet*>     m_stacked_ingredient_names;
    QStackedBarSeries*  m_stacked_ingredients_costs;


    bool m_pending_selection_change;
    bool m_pending_ingredient_box_change;
    bool m_pending_recipe_box_change;
    bool m_pending_ingredient_add;

    QCPTextElement*     m_recipe_cost_title;

private slots:

    void slot_combobox_category_selection(const QString &text);
    void slot_create_category_response(int index);

    void slot_selection_change(const QItemSelection selected, const QItemSelection deselected);
    void slot_ingredient_change(const QModelIndex& changed);
    void slot_ingredient_add();

    void slot_request_ingredient_data_response(QString supplier, QString category, QString number, QString name, QString description,
                                               QString brand, QString uom, double price, double volatility);

    void slot_request_recipe_cost_response(QString new_cost);
    void slot_request_recipe_volatility_response(QString new_volatility);

    void slot_save_recipe();
    void slot_recipe_added(int index);

    void slot_update_costing_viz(QList<std::tuple<QModelIndex, QString, double>> costs);

    void slot_construct_treemap_viz(QList<std::tuple<QString, double, QString>> recipe_amounts);

signals:

    void sig_request_ingredient_data(const QModelIndex&);
    void sig_request_recipe_volatility(QList<Recipe_Ingredient>);
    void sig_request_recipe_cost(QList<Recipe_Ingredient>);
    void sig_save_recipe(QString, QString, double, double, double, QList<Recipe_Ingredient>);
    void sig_get_treemap_data(const QModelIndex&);
    void sig_recipe_added(int);

};

#endif // RECIPE_ENGINEER_VIEWER_H
