#include "recipe_engineer_viewer.h"
#include "ui_recipe_engineer_viewer.h"

Recipe_Engineer_Viewer::Recipe_Engineer_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Recipe_Engineer_Viewer)
{
    ui->setupUi(this);
    setWindowTitle(tr("Create New Recipe"));

    this->m_model = new Recipe_Engineer_Model();
    ui->list_select_ingredients->setModel(m_model);

    this->m_cbox_model = new Recipe_Categories_Model();
    ui->cbox_category->setModel(m_cbox_model);

    this->m_selection_model = ui->list_select_ingredients->selectionModel();

    this->m_dialog_create_category = new Dialog_Recipe_Category_Add();

    // ---------------------------------------------------------------------------
    // request and response for ingredient data
    connect(this,           SIGNAL(sig_request_ingredient_data(const QModelIndex&)),
            this->m_model,  SLOT(slot_request_ingredient_data(const QModelIndex&)));

    connect(this->m_model,  SIGNAL(sig_request_ingredient_data_response(QString, QString, QString, QString, QString, QString, QString, double, double)),
            this,           SLOT(slot_request_ingredient_data_response(QString, QString, QString, QString, QString, QString, QString, double, double)));
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // set pending flags and request data at signals for ingredient changes...
    // selection changes...
    // and adding ingredient to recipe
    connect(this->m_model,  SIGNAL(sig_ingredient_changed(const QModelIndex&)),
            this,           SLOT(slot_ingredient_change(const QModelIndex&)));

    connect(m_selection_model,      SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
            this,                   SLOT(slot_selection_change(const QItemSelection, const QItemSelection)));

    connect(ui->btn_add_ingredient, SIGNAL(pressed()),
            this,                   SLOT(slot_ingredient_add()));
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // categories combo box signals to open dialog...
    // and, request and respond to new category creates
    connect(ui->cbox_category,  SIGNAL(activated(const QString)),
            this,               SLOT(slot_combobox_category_selection(const QString)));

    connect(m_dialog_create_category,   SIGNAL(sig_create_category(QString)),
            this->m_cbox_model,         SLOT(slot_create_category(QString)));

    connect(this->m_cbox_model,         SIGNAL(sig_create_category_response(int)),
            m_dialog_create_category,   SLOT(slot_create_category_response(int)));

    connect(this->m_cbox_model, SIGNAL(sig_create_category_response(int)),
            this,               SLOT(slot_create_category_response(int)));
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // updating cost and volatility needs to be in real time for cost exploration
    connect(this,           SIGNAL(sig_request_recipe_cost(QList<Recipe_Ingredient>)),
            this->m_model,  SLOT(slot_request_recipe_cost(QList<Recipe_Ingredient>)));

    connect(this->m_model,  SIGNAL(sig_request_recipe_cost_response(QString)),
            this,           SLOT(slot_request_recipe_cost_response(QString)));

    connect(this,           SIGNAL(sig_request_recipe_volatility(QList<Recipe_Ingredient>)),
            this->m_model,  SLOT(slot_request_recipe_volatility(QList<Recipe_Ingredient>)));

    connect(this->m_model,  SIGNAL(sig_request_recipe_volatility_response(QString)),
            this,           SLOT(slot_request_recipe_volatility_response(QString)));
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // saving recipe
    QDoubleValidator* double_validator_price = new QDoubleValidator(0, 999, 2, this);
    ui->input_recipe_price->setValidator(double_validator_price);
    connect(ui->btn_save_recipe,    SIGNAL(clicked()),
            this,                   SLOT(slot_save_recipe()));
    connect(this,                   SIGNAL(sig_save_recipe(QString, QString, double, double, double, QList<Recipe_Ingredient>)),
            this->m_model,          SLOT(slot_save_recipe(QString, QString, double, double, double, QList<Recipe_Ingredient>)));
    connect(this->m_model,          SIGNAL(sig_recipe_added(int)),
            this,                   SLOT(slot_recipe_added(int)));

    // ---------------------------------------------------------------------------
    // ui specific aesthetic and spacing adjustments
    ui->output_ingredient_category->setWordWrap(true);
    ui->cbox_category->setCurrentIndex(0);

    connect(ui->cbox_category,      &QComboBox::currentTextChanged,
            this,                   [this](){ui->cbox_category->setStyleSheet(""); });
    connect(ui->input_recipe_name,  &QLineEdit::textEdited,
            this,                   [this](){ui->input_recipe_name->setStyleSheet(""); });
    connect(ui->input_recipe_price, &QLineEdit::textEdited,
            this,                   [this](){ui->input_recipe_price->setStyleSheet(""); });

    // ---------------------------------------------------------------------------
    // request and response pending flags
    m_pending_selection_change      = false;
    m_pending_ingredient_box_change = false;
    m_pending_recipe_box_change     = false;
    m_pending_ingredient_add        = false;

    // ---------------------------------------------------------------------------
    // ingredient cost viz is a dynamic stacked bar chart
    ui->viz_recipe_cost->legend->setVisible(true);
    ui->viz_recipe_cost->legend->setFont(QFont("Helvetica", 9));

    ui->viz_recipe_cost->xAxis->setSubTicks(false);
    ui->viz_recipe_cost->xAxis->setTicks(false);
    ui->viz_recipe_cost->xAxis->setTickLabels(false);
    ui->viz_recipe_cost->xAxis->grid()->setSubGridVisible(false);
    ui->viz_recipe_cost->xAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_recipe_cost->xAxis->setRangeUpper(2.0);
    ui->viz_recipe_cost->xAxis->setRangeLower(0.0);

    QSharedPointer<Util_QCP_Axis_Dollars> dollars_ticker(new Util_QCP_Axis_Dollars);
    ui->viz_recipe_cost->yAxis->setTicker(dollars_ticker);
    ui->viz_recipe_cost->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_recipe_cost->yAxis->setSubTicks(true);
    ui->viz_recipe_cost->yAxis->setTicks(true);
    ui->viz_recipe_cost->yAxis->setTickLabels(true);
    ui->viz_recipe_cost->yAxis->grid()->setSubGridVisible(true);
    ui->viz_recipe_cost->yAxis->setLabel("Cost");
    ui->viz_recipe_cost->yAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_recipe_cost->yAxis->setRangeUpper(100.0);
    ui->viz_recipe_cost->yAxis->setRangeLower(0.0);

    ui->viz_recipe_cost->axisRect()->setupFullAxesBox();

    m_recipe_cost_title = new QCPTextElement(ui->viz_recipe_cost, "Running Recipe Cost", QFont("sans", 10, QFont::Bold));
    ui->viz_recipe_cost->plotLayout()->insertRow(0);
    ui->viz_recipe_cost->plotLayout()->addElement(0, 0, m_recipe_cost_title);

    ui->viz_recipe_cost->setInteractions(QCP::iSelectPlottables);

    connect(this->m_model,  SIGNAL(sig_update_costing_viz(QList<std::tuple<QModelIndex, QString, double>>)),
            this,           SLOT(slot_update_costing_viz(QList<std::tuple<QModelIndex, QString, double>>)));

    connect(this,           SIGNAL(sig_get_treemap_data(const QModelIndex&)),
            this->m_model,  SLOT(slot_get_tree_map_data(const QModelIndex&)));

    connect(this->m_model,  SIGNAL(sig_get_treemap_data_response(QList<std::tuple<QString, double, QString>>)),
            this,           SLOT(slot_construct_treemap_viz(QList<std::tuple<QString, double, QString>>)));
}

Recipe_Engineer_Viewer::~Recipe_Engineer_Viewer()
{
    delete this->m_model;
    delete this->m_cbox_model;
    delete this->m_dialog_create_category;
    delete ui;

}

void Recipe_Engineer_Viewer::slot_ingredient_add()
{
    if (!m_current_selection.isValid())
        return;

    m_pending_ingredient_add = true;
    emit sig_request_ingredient_data(m_current_selection);
}

void Recipe_Engineer_Viewer::slot_selection_change(const QItemSelection selected, const QItemSelection deselected)
{
    m_pending_selection_change = true;
    m_current_selection = m_selection_model->selection().indexes().first();
    emit sig_request_ingredient_data(m_current_selection);
}

void Recipe_Engineer_Viewer::slot_ingredient_change(const QModelIndex& changed)
{
    if (m_current_selection.isValid() && m_current_selection == changed)
        m_pending_ingredient_box_change = true;

    for (int i = 0; i < m_out_ingredients.size(); ++i) {
        if (std::get<0>(m_out_ingredients[i]) == changed) {
            m_pending_recipe_box_change = true;
        }
    }

    if (m_pending_ingredient_box_change || m_pending_recipe_box_change)
        emit sig_request_ingredient_data(m_current_selection);
}

void Recipe_Engineer_Viewer::slot_request_ingredient_data_response(QString supplier, QString category, QString number, QString name,
                                                                   QString description, QString brand, QString uom, double price, double volatility)
{
    if (m_pending_ingredient_add) {
        response_ingredient_add(supplier, category, number, name, description, brand, uom, price, volatility);
    }

    if (m_pending_selection_change) {
        response_selection_change(category, name, supplier, brand, number, uom, price, volatility);
    }

    if (m_pending_ingredient_box_change) {
        response_ingredient_info_change(category, name, supplier, brand, number, uom, price, volatility);
    }

    if (m_pending_recipe_box_change) {
        response_recipe_info_change();
    }
}

void Recipe_Engineer_Viewer::response_ingredient_add(QString supplier, QString category, QString number, QString name, QString description,
                                                     QString brand, QString uom, double price, double volatility)
{
    m_pending_ingredient_add = false;

    for (int i = 0; i < m_out_ingredients.size(); ++i) {
        if (std::get<0>(m_out_ingredients[i]) == m_current_selection)
            return;
    }

    m_out_ingredients.append(std::make_tuple(m_current_selection, 0.0, uom));

    QSizePolicy policy_fixed_size(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QDoubleValidator* double_validator = new QDoubleValidator(0, 9999, 4, this);

    QPushButton* temp_x = new QPushButton("x");
    temp_x->setSizePolicy(policy_fixed_size);
    temp_x->setMinimumWidth(15);
    temp_x->setMinimumHeight(15);
    temp_x->setMaximumWidth(15);
    temp_x->setMaximumHeight(15);
    temp_x->setStyleSheet("QPushButton{ border: none; background-color: white; padding-bottom:2px; } "
                          "QPushButton:pressed{ background-color: rgba(255,0,0,100);}");

    QLabel* temp_label = new QLabel(name + ":");

    QLineEdit* temp_input = new QLineEdit();
    temp_input->setSizePolicy(policy_fixed_size);
    temp_input->setPlaceholderText("0.0000");
    temp_input->setValidator(double_validator);
    temp_input->setMinimumWidth(60);
    temp_input->setMaximumWidth(60);

    QComboBox* temp_cbox = new QComboBox();
    QList<int> uom_options = UOM_Utils::get_related_uom(Ingredients_Data_Globals::ACCEPTED_UOMS, uom);
    for (int i = 0; i < uom_options.size(); ++i) {
        temp_cbox->addItem(Ingredients_Data_Globals::ACCEPTED_UOMS[uom_options[i]]);
    }

    temp_cbox->setCurrentText(uom);

    ui->layout_recipe_ingredients->addWidget(temp_x,     m_out_ingredients.size() - 1, 0);
    ui->layout_recipe_ingredients->addWidget(temp_label, m_out_ingredients.size() - 1, 1);
    ui->layout_recipe_ingredients->addWidget(temp_input, m_out_ingredients.size() - 1, 2);
    ui->layout_recipe_ingredients->addWidget(temp_cbox,  m_out_ingredients.size() - 1, 3);

    QModelIndex temp_qindex = m_current_selection;
    connect(temp_x, &QPushButton::clicked, this, [this, temp_qindex](){ response_ingredient_remove(temp_qindex); });
    connect(temp_input, &QLineEdit::textEdited, [this, temp_qindex, temp_input](){ response_ingredient_amount_change(temp_qindex, temp_input->text().toDouble()); });
    connect(temp_cbox, &QComboBox::currentTextChanged, [this, temp_input, temp_qindex, temp_cbox](){ response_ingredient_uom_change(temp_qindex, temp_input, temp_cbox->currentText()); });

    emit sig_request_recipe_cost(m_out_ingredients);
}

void Recipe_Engineer_Viewer::response_selection_change(QString category, QString name, QString supplier, QString brand, QString number, QString uom, double price, double volatility)
{
    m_pending_selection_change = false;

    ui->output_ingredient_category->setText(category);
    ui->output_ingredient_name->setText(name);
    ui->output_ingredient_supplier->setText(supplier);
    ui->output_ingredient_brand->setText(brand);
    ui->output_ingredient_number->setText(number);
    ui->output_ingredient_price->setText("$" + QString::number(price) + "/" + uom);
    ui->label_volatility_ingredient_info->setText(QString::number(volatility, 'f', 2));

    emit sig_get_treemap_data(m_current_selection);
}

void Recipe_Engineer_Viewer::response_ingredient_info_change(QString category, QString name, QString supplier, QString brand, QString number, QString uom, double price, double volatility)
{
    m_pending_ingredient_box_change = false;

    ui->output_ingredient_category->setText(category);
    ui->output_ingredient_name->setText(name);
    ui->output_ingredient_supplier->setText(supplier);
    ui->output_ingredient_brand->setText(brand);
    ui->output_ingredient_number->setText(number);
    ui->output_ingredient_price->setText("$" + QString::number(price) + "/" + uom);
    ui->label_volatility_ingredient_info->setText(QString::number(volatility, 'f', 2));
}

void Recipe_Engineer_Viewer::response_recipe_info_change()
{
    m_pending_recipe_box_change = false;

    emit sig_request_recipe_cost(m_out_ingredients);
    emit sig_request_recipe_volatility(m_out_ingredients);
}

void Recipe_Engineer_Viewer::slot_combobox_category_selection(const QString &text)
{
    if (text == "--Create New--") {
        m_dialog_create_category->show();
        m_dialog_create_category->raise();
    }
}

void Recipe_Engineer_Viewer::response_ingredient_remove(const QModelIndex& index)
{
    int temp_index = 0;
    int temp_num_ingredients = m_out_ingredients.size();
    for (int i = 0; i < temp_num_ingredients; ++i) {
        if (std::get<0>(m_out_ingredients[i]) == index) {
            m_out_ingredients.removeAt(i);
            break;
        }
        temp_index++;
    }
    QWidget* to_remove_x = ui->layout_recipe_ingredients->itemAtPosition(temp_index, 0)->widget();
    ui->layout_recipe_ingredients->removeWidget(to_remove_x);
    to_remove_x->deleteLater();

    QWidget* to_remove_label = ui->layout_recipe_ingredients->itemAtPosition(temp_index, 1)->widget();
    ui->layout_recipe_ingredients->removeWidget(to_remove_label);
    to_remove_label->deleteLater();

    QWidget* to_remove_input = ui->layout_recipe_ingredients->itemAtPosition(temp_index, 2)->widget();
    ui->layout_recipe_ingredients->removeWidget(to_remove_input);
    to_remove_input->deleteLater();

    QWidget* to_remove_cbox = ui->layout_recipe_ingredients->itemAtPosition(temp_index, 3)->widget();
    ui->layout_recipe_ingredients->removeWidget(to_remove_cbox);
    to_remove_cbox->deleteLater();

    for (int j = temp_index + 1; j < temp_num_ingredients; ++j) {
        QWidget* to_move_x      = ui->layout_recipe_ingredients->itemAtPosition(j, 0)->widget();
        QWidget* to_move_label  = ui->layout_recipe_ingredients->itemAtPosition(j, 1)->widget();
        QWidget* to_move_input  = ui->layout_recipe_ingredients->itemAtPosition(j, 2)->widget();
        QWidget* to_move_cbox   = ui->layout_recipe_ingredients->itemAtPosition(j, 3)->widget();
        ui->layout_recipe_ingredients->removeWidget(to_move_x);
        ui->layout_recipe_ingredients->removeWidget(to_move_label);
        ui->layout_recipe_ingredients->removeWidget(to_move_input);
        ui->layout_recipe_ingredients->removeWidget(to_move_cbox);
        ui->layout_recipe_ingredients->addWidget(to_move_x, j - 1, 0);
        ui->layout_recipe_ingredients->addWidget(to_move_label, j - 1, 1);
        ui->layout_recipe_ingredients->addWidget(to_move_input, j - 1, 2);
        ui->layout_recipe_ingredients->addWidget(to_move_cbox, j - 1, 3);
    }

    sig_request_recipe_volatility(m_out_ingredients);
    emit sig_request_recipe_cost(m_out_ingredients);
}

void Recipe_Engineer_Viewer::response_ingredient_amount_change(const QModelIndex& index, double amount)
{
    for (int i = 0; i < m_out_ingredients.size(); ++i) {
        if (std::get<0>(m_out_ingredients[i]) == index) {
            std::get<1>(m_out_ingredients[i]) = amount;
            break;
        }
    }

    emit sig_request_recipe_cost(m_out_ingredients);
    emit sig_request_recipe_volatility(m_out_ingredients);
}

void Recipe_Engineer_Viewer::response_ingredient_uom_change(const QModelIndex& index, QLineEdit* amount, QString new_uom)
{
    int list_index = 0;
    for (int i = 0; i < m_out_ingredients.size(); ++i) {
        if (std::get<0>(m_out_ingredients[i]) == index) {
            list_index = i;
            break;
        }
    }

    QString curr_uom = std::get<2>(m_out_ingredients[list_index]);
    QString text_curr_amount = amount->text();
    std::get<2>(m_out_ingredients[list_index]) = new_uom;

    emit sig_request_recipe_cost(m_out_ingredients);
    emit sig_request_recipe_volatility(m_out_ingredients);
}

void Recipe_Engineer_Viewer::slot_request_recipe_cost_response(QString new_cost)
{
    this->ui->label_out_recipe_cost->setText(new_cost);
}

void Recipe_Engineer_Viewer::slot_request_recipe_volatility_response(QString new_volatility)
{
    this->ui->label_out_recipe_volatility->setText(new_volatility);
}

void Recipe_Engineer_Viewer::slot_save_recipe()
{
    bool submit_ready = true;

    QString     out_category;
    QString     out_name;
    double      out_price = 0.0;
    double      out_cost = 0.0;
    double      out_volatility;

    if (ui->cbox_category->currentIndex() == 0) {
        ui->cbox_category->setStyleSheet("QComboBox {color: red; padding: ;}");
        submit_ready = false;
    }
    else out_category = ui->cbox_category->currentText();

    if (ui->input_recipe_name->text().isEmpty()) {
        ui->input_recipe_name->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_name = ui->input_recipe_name->text();

    if (ui->input_recipe_price->text() == "0.00" || ui->input_recipe_price->text().isEmpty()) {
        ui->input_recipe_price->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_price = ui->input_recipe_price->text().toDouble();

    if (ui->label_out_recipe_cost->text() == "0.00") {
        ui->label_out_recipe_cost->setStyleSheet("color: red;");
        submit_ready = false;
    }
    else {
        ui->label_out_recipe_cost->setStyleSheet("");
        out_cost = ui->label_out_recipe_cost->text().toDouble();
    }

    out_volatility = ui->label_out_recipe_volatility->text().toDouble();

    if (!submit_ready)
        return;

    emit sig_save_recipe(out_category, out_name, out_price, out_cost, out_volatility, m_out_ingredients);

    ui->cbox_category->setCurrentIndex(0);
    ui->label_out_recipe_cost->setText("0.00");
    ui->input_recipe_name->clear();
    ui->input_recipe_price->clear();

    while (m_out_ingredients.size() > 0) {
        response_ingredient_remove(std::get<0>(m_out_ingredients[0]));
    }
    this->hide();
    ui->label_out_recipe_volatility->setText("0.00");
}

void Recipe_Engineer_Viewer::slot_recipe_added(int index)
{
    emit sig_recipe_added(index);
}

void Recipe_Engineer_Viewer::slot_update_costing_viz(QList<std::tuple<QModelIndex, QString, double> > costs)
{
    ui->viz_recipe_cost->clearPlottables();

    if (costs.size() == 0) {
        ui->viz_recipe_cost->replot();
        return;
    }

    QVector<QCPBars*> stacked_bars (QVector<QCPBars*>(costs.size()));

    stacked_bars[0] = new QCPBars(ui->viz_recipe_cost->xAxis, ui->viz_recipe_cost->yAxis);
    stacked_bars[0]->setName(std::get<1>(costs[0]));
    stacked_bars[0]->setAntialiased(false);
    stacked_bars[0]->setSelectable(QCP::stSingleData);
    stacked_bars[0]->setBrush(QColor(COLOR_UTIL[0]));
    QVector<double> data;
    QVector<double> tick;
    tick << 1;
    data << std::get<2>(costs[0]);
    stacked_bars[0]->addData(tick, data); // so hacky... friggin library

    double running_cost = std::get<2>(costs[0]);

    for (int i = 1; i < costs.size(); ++i) {
        stacked_bars[i] = new QCPBars(ui->viz_recipe_cost->xAxis, ui->viz_recipe_cost->yAxis);
        if (i < COLOR_UTIL.size())
            stacked_bars[i]->setBrush(QColor(COLOR_UTIL[i]));
        else
            stacked_bars[i]->setBrush(QColor(qSin(i*1+1.2)*80+80, qSin(i*0.3+0)*80+80, qSin(i*0.3+1.5)*80+80));
        stacked_bars[i]->setName(std::get<1>(costs[i]));
        stacked_bars[i]->setAntialiased(false);
        stacked_bars[i]->setSelectable(QCP::stSingleData);
        QVector<double> data;
        QVector<double> tick;
        tick << 1;
        data << std::get<2>(costs[i]);
        stacked_bars[i]->addData(tick, data);
        stacked_bars[i]->moveAbove(stacked_bars[i-1]);
        stacked_bars[i]->setStackingGap(1);
        running_cost += std::get<2>(costs[i]);
    }
    if (running_cost > 100.0)
        ui->viz_recipe_cost->yAxis->setRangeUpper(running_cost + 100.0);
    else
        ui->viz_recipe_cost->yAxis->setRangeUpper(running_cost + 5.0);
    ui->viz_recipe_cost->replot();
}

void Recipe_Engineer_Viewer::slot_construct_treemap_viz(QList<std::tuple<QString, double, QString>> recipe_amounts)
{
    if (recipe_amounts.isEmpty()) {
        ui->label_amount_used->clear();
        return;
    }

//    clear_widgets(ui->viz_treemap_ingredient_usage);
//    ui->viz_treemap_ingredient_usage->setContentsMargins(0, 0, 0, 0);
//    ui->viz_treemap_ingredient_usage->setVerticalSpacing(0);
//    ui->viz_treemap_ingredient_usage->setHorizontalSpacing(0);
    QList<int> uoms = UOM_Utils::get_related_uom(Ingredients_Data_Globals::ACCEPTED_UOMS, std::get<2>(recipe_amounts[0]));
    QString uom = Ingredients_Data_Globals::ACCEPTED_UOMS[uoms[0]];
    double running_amount = 0.0;
    for (int i = 0; i < recipe_amounts.size(); ++i) {
        running_amount += UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, std::get<2>(recipe_amounts[i]), uom, std::get<1>(recipe_amounts[i]));
    }
//    QLabel* element = new QLabel("Total Usage: " + QString::number(running_amount) + uom + "\n" + "Number of Recipes: " + QString::number(recipe_amounts.size()));
    ui->label_amount_used->setText(QString::number(running_amount) + uom + + "\n" + QString::number(recipe_amounts.size()) + " Recipe(s)");
}

void Recipe_Engineer_Viewer::slot_create_category_response(int index)
{
    ui->cbox_category->setCurrentIndex(index);
}

void Recipe_Engineer_Viewer::clear_widgets(QLayout* layout) {
   if (! layout)
      return;
   while (auto item = layout->takeAt(0)) {
      delete item->widget();
      clear_widgets(item->layout());
   }
}
