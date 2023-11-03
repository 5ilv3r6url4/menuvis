#include "recipes_viewer.h"
#include "ui_recipes_viewer.h"

Recipes_Viewer::Recipes_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Recipes_Viewer)
{
    ui->setupUi(this);

    this->m_model = new Recipes_Model();
    ui->list_recipes->setModel(m_model);

    this->m_dialog_engineer = new Recipe_Engineer_Viewer();

    this->m_selection_model = ui->list_recipes->selectionModel();
    ui->list_recipes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->list_recipes->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->btn_add,    SIGNAL(pressed()),
            this,           SLOT(slot_open_recipe_engineer()));

    connect(this->m_dialog_engineer,    SIGNAL(sig_recipe_added(int)),
            this->m_model,              SLOT(slot_update_recipes(int)));

    connect(m_selection_model,  SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
            this,               SLOT(slot_selection_change(const QItemSelection, const QItemSelection)));

    connect(this,               SIGNAL(sig_request_recipe_data(const QModelIndex&)),
            this->m_model,      SLOT(slot_request_recipe_data(const QModelIndex&)));

    connect(this->m_model,      SIGNAL(sig_request_recipe_data_response(QString, QString, double, double, double, QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>>)),
            this,               SLOT(slot_request_recipe_data_response(QString, QString, double, double, double, QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>>)));

    m_composition_viz = new DonutBreakdownChart();
    m_composition_viz->setTitle("Recipe Composition for [No Recipe Selected]");
    ui->viz_recipe_composition->setRenderHint(QPainter::Antialiasing);
    ui->viz_recipe_composition->setChart(m_composition_viz);

    connect(this->m_model,  SIGNAL(sig_update_yoself_my_dudes(const QModelIndex&)),
            this,           SLOT(slot_aight_we_be_updating(const QModelIndex&)));

    // recipe cost/price viz
    // ---------------------------------------------------------------------------
    // historic prices viz
    ui->viz_cost_price_history->legend->setVisible(true);
    ui->viz_cost_price_history->legend->setFont(QFont("Helvetica", 9));
    QPen main_grid_pen(QColor(0, 0, 0));
    main_grid_pen.setWidth(2);

    ui->viz_cost_price_history->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    QSharedPointer<QCPAxisTickerDateTime> date_ticker(new QCPAxisTickerDateTime);
    date_ticker->setDateTimeFormat("dd/MM/yy");
    ui->viz_cost_price_history->xAxis->setTicker(date_ticker);
    ui->viz_cost_price_history->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_cost_price_history->xAxis->setTicks(true);
    ui->viz_cost_price_history->xAxis->setTickLabels(true);
    ui->viz_cost_price_history->xAxis->setLabel("Date");
    ui->viz_cost_price_history->xAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_cost_price_history->xAxis->setRangeUpper(QDateTime::currentDateTime().toTime_t());
    ui->viz_cost_price_history->xAxis->setRangeLower(QCPAxisTickerDateTime::dateTimeToKey(QDate(2017, 1, 1)));

    QSharedPointer<Util_QCP_Axis_Dollars> dollar_ticker(new Util_QCP_Axis_Dollars);
    ui->viz_cost_price_history->yAxis->setTicker(dollar_ticker);
    ui->viz_cost_price_history->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_cost_price_history->yAxis->setTicks(true);
    ui->viz_cost_price_history->yAxis->setTickLabels(true);
    ui->viz_cost_price_history->yAxis->setLabel("Price");
    ui->viz_cost_price_history->yAxis->setLabelFont(QFont(QFont().family(), 9));

    ui->viz_cost_price_history->axisRect()->setupFullAxesBox();

    m_cost_price_history_title = new QCPTextElement(ui->viz_cost_price_history, "Price-Cost History for [No Recipe Selected]", QFont("sans", 10, QFont::Bold));
    ui->viz_cost_price_history->plotLayout()->insertRow(0);
    ui->viz_cost_price_history->plotLayout()->addElement(0, 0, m_cost_price_history_title);

    ui->viz_cost_price_history->setInteractions(QCP::iSelectPlottables);

    // volatility viz
    ui->viz_volatility_history->legend->setVisible(true);
    ui->viz_volatility_history->legend->setFont(QFont("Helvetica", 9));

    ui->viz_volatility_history->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    ui->viz_volatility_history->xAxis->setTicker(date_ticker);
    ui->viz_volatility_history->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_volatility_history->xAxis->setSubTicks(true);
    ui->viz_volatility_history->xAxis->setTicks(true);
    ui->viz_volatility_history->xAxis->setTickLabels(true);
    ui->viz_volatility_history->xAxis->grid()->setSubGridVisible(true);
    ui->viz_volatility_history->yAxis->grid()->setZeroLinePen(main_grid_pen);
    ui->viz_volatility_history->xAxis->setLabel("Date");
    ui->viz_volatility_history->xAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_volatility_history->xAxis->setRangeUpper(QDateTime::currentDateTime().toTime_t());
    ui->viz_volatility_history->xAxis->setRangeLower(QCPAxisTickerDateTime::dateTimeToKey(QDate(2017, 1, 1)));

    QSharedPointer<Util_QCP_Axis_Percentage> percentage_ticker(new Util_QCP_Axis_Percentage);
    ui->viz_volatility_history->yAxis->setTicker(percentage_ticker);
    ui->viz_volatility_history->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_volatility_history->yAxis->setSubTicks(true);
    ui->viz_volatility_history->yAxis->setTicks(true);
    ui->viz_volatility_history->yAxis->setTickLabels(true);
    ui->viz_volatility_history->yAxis->grid()->setSubGridVisible(true);
    ui->viz_volatility_history->yAxis->setLabel("Volatility");
    ui->viz_volatility_history->yAxis->setLabelFont(QFont(QFont().family(), 9));

    ui->viz_volatility_history->axisRect()->setupFullAxesBox();

    m_volatility_history_title = new QCPTextElement(ui->viz_volatility_history, "Volatility History for [No Recipe Selected]", QFont("sans", 10, QFont::Bold));
    ui->viz_volatility_history->plotLayout()->insertRow(0);
    ui->viz_volatility_history->plotLayout()->addElement(0, 0, m_volatility_history_title);

    ui->viz_volatility_history->setInteractions(QCP::iSelectPlottables);

    connect(this,           SIGNAL(sig_request_granular_cost_price_histories(const QModelIndex&)),
            this->m_model,  SLOT(slot_request_granular_cost_price_histories(const QModelIndex&)));

    connect(this->m_model,  SIGNAL(sig_request_granular_cost_price_histories_response(QString, QMap<int, QList<QList<double>>>, QMap<int, QList<QList<double>>>)),
            this,           SLOT(slot_request_granular_cost_price_histories_response(QString, QMap<int, QList<QList<double>>>, QMap<int, QList<QList<double>>>)));

    connect(this,           SIGNAL(sig_request_granular_volatility_histories(const QModelIndex&)),
            this->m_model,  SLOT(slot_request_granular_volatility_histories(const QModelIndex&)));

    connect(this->m_model,  SIGNAL(sig_request_granular_volatility_histories_response(QString, QMap<int, QList<QList<double>>>)),
            this,           SLOT(slot_request_granular_volatility_histories_response(QString, QMap<int, QList<QList<double>>>)));
}

Recipes_Viewer::~Recipes_Viewer()
{
    delete ui;
}

void Recipes_Viewer::update_recipe_composition_viz(QString name, QList<std::tuple<Ingredients_Data::Ingredient *, double, QString> > ingredients)
{
    QStringList temp_categories;

    for (int i = 0; i < ingredients.size(); ++i) {
        if (!temp_categories.contains(std::get<0>(ingredients[i])->s_category)) {
            qDebug() << std::get<0>(ingredients[i])->s_category;
            temp_categories.append(std::get<0>(ingredients[i])->s_category);
        }
    }
    qDebug() << "------------";
    QVector<QPieSeries*> temp_series (QVector<QPieSeries*>(temp_categories.size()));

    for (int j = 0; j < temp_categories.size(); ++j) {
        temp_series[j] = new QPieSeries();
        temp_series[j]->setName(temp_categories[j]);
        for (int k = 0; k < ingredients.size(); ++k) {
            if (std::get<0>(ingredients[k])->s_category == temp_categories[j]) {
                double temp_amount =  std::get<1>(ingredients[k]);
                double new_amount = 0.0;
                QString temp_uom = std::get<2>(ingredients[k]);
                if (temp_uom == "PR" || temp_uom == "UN" || temp_uom == "DZ")
                    new_amount = UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_uom, "UN", temp_amount);
                if (temp_uom == "GR" || temp_uom == "KG" || temp_uom == "LB" || temp_uom == "OZ")
                    new_amount = UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_uom, "KG", temp_amount);
                if (temp_uom == "ML" || temp_uom == "L" || temp_uom == "QT" || temp_uom == "GAL")
                    new_amount = UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, temp_uom, "L", temp_amount);
                temp_series[j]->append(std::get<0>(ingredients[k])->s_name, temp_amount);
                Ingredients_Data::Ingredient* ingredient = std::get<0>(ingredients[k]);
                connect(temp_series[j]->slices()[temp_series[j]->slices().size() - 1], &QPieSlice::hovered, this, [this, ingredient](){update_ingredient_info(ingredient);});
            }
        }
    }

    m_composition_viz = new DonutBreakdownChart();
    m_composition_viz->setTitle("Recipe Composition for [" + name + "]");
    m_composition_viz->legend()->setAlignment(Qt::AlignRight);

    for (int i = 0; i < temp_series.size(); ++i) {
        int color_int = Ingredients_Data_Globals::INGREDIENT_CATEGORIES.indexOf(temp_categories[i]);
        m_composition_viz->addBreakdownSeries(temp_series[i], COLOR_UTIL[color_int]);
    }
    ui->viz_recipe_composition->setChart(m_composition_viz);
}

void Recipes_Viewer::slot_open_recipe_engineer()
{
    m_dialog_engineer->show();
    m_dialog_engineer->raise();
}

void Recipes_Viewer::slot_selection_change(const QItemSelection selected, const QItemSelection deselected)
{
    if (selected.indexes().isEmpty()) //deslection guard
        return;

    if (m_current_selection != m_selection_model->selection().indexes().first()) {
        m_current_selection = m_selection_model->selection().indexes().first();
    }

    emit sig_request_granular_cost_price_histories(m_current_selection);
    emit sig_request_granular_volatility_histories(m_current_selection);
    emit sig_request_recipe_data(m_current_selection);
}

void Recipes_Viewer::slot_request_recipe_data_response(QString name, QString category, double price, double cost, double volatility,
                                                       QList<std::tuple<Ingredients_Data::Ingredient*, double, QString>> ingredients)
{
    ui->label_recipe_name->setText(name);
    ui->label_recipe_category->setText(category);
    ui->label_recipe_cost->setText("$ " + QString::number(cost, 'f', 2));
    ui->label_recipe_price->setText("$ " + QString::number(price, 'f', 2));
    ui->label_recipe_volatility->setText("% " + QString::number(volatility, 'f', 2));

    update_recipe_composition_viz(name, ingredients);
}

void Recipes_Viewer::update_ingredient_info(Ingredients_Data::Ingredient* ingredient)
{
    ui->label_ingredient_brand->setText(ingredient->s_brand);
    ui->label_ingredient_category->setText(ingredient->s_category);
    ui->label_ingredient_description->setText(ingredient->s_description);
    ui->label_ingredient_name->setText(ingredient->s_name);
    ui->label_ingredient_number->setText(ingredient->s_number);
    ui->label_ingredient_supplier->setText(ingredient->s_supplier);
    ui->label_ingredient_price->setText(QString::number(ingredient->s_price, 'f', 2));
    ui->label_ingredient_volatility->setText(QString::number(ingredient->s_volatility, 'f', 2));
    ui->label_ingredient_yield_uom->setText(QString::number(ingredient->s_amount, 'f', 2) + " (" + ingredient->s_uom + ")");

}

void Recipes_Viewer::slot_request_granular_cost_price_histories_response(QString recipe_name, QMap<int, QList<QList<double>>> cost_histories, QMap<int, QList<QList<double>>> price_histories)
{
    if (cost_histories.size() == 0 || price_histories.size() == 0) // theyre the same size anyways always (daaaaangerous assumption)
        return;

    ui->viz_cost_price_history->clearGraphs();
    m_cost_price_history_title->setText("Cost-Price History for [" + recipe_name + "]");

    double largest_value = 0.0;

    QDate start = QDate::currentDate();
    QDate end = QDate::currentDate();

    ui->viz_cost_price_history->addGraph();
    ui->viz_cost_price_history->graph()->setPen(QPen(QColor(COLOR_UTIL[0])));
    ui->viz_cost_price_history->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
    ui->viz_cost_price_history->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->viz_cost_price_history->graph()->setSelectable(QCP::stWhole);
    ui->viz_cost_price_history->graph()->setName(recipe_name + " Costs");
    ui->viz_cost_price_history->addGraph();
    ui->viz_cost_price_history->graph()->setPen(QPen(QColor(COLOR_UTIL[1])));
    ui->viz_cost_price_history->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
    ui->viz_cost_price_history->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->viz_cost_price_history->graph()->setSelectable(QCP::stWhole);
    ui->viz_cost_price_history->graph()->setName(recipe_name + " Prices");

    QMap<int, QList<QList<double>>>::iterator year;

    int count_limit = 0; // we only want to display the most recent 12 months, any more and the graph gets wack.
    QVector<QCPGraphData> costs_over_years;
    QVector<QCPGraphData> prices_over_years;

    // backwards! no use if we only show the first year when they're on their xth year (wishful thinking)
    for (year = cost_histories.end() - 1; year != cost_histories.begin() - 1; --year) {
        QList<QList<double>> months = year.value();

        // backwards from december
        for (int m = 11; m > -1; --m) {
            QList<double> days = months[m];

            // forward search
            bool empty = true;
            for (int d = 0; d < days.size(); ++d) {
                if (days[d] > -100000.0) {
                    empty = false;
                    QCPGraphData cost;
                    QDate date = QDate(year.key(), m + 1, d + 1);
                    cost.key = QCPAxisTickerDateTime::dateTimeToKey(date);
                    cost.value = days[d];
                    costs_over_years.append(cost);
                    if (largest_value < cost.value)
                        largest_value = cost.value;
                    if (date > end)
                        end = date;
                    else if (date < start)
                        start = date;
                }
            }
            if (!empty)
                count_limit++;
            if (count_limit == 12)
                break;
        }
        if (count_limit == 12)
            break;
    }
    count_limit = 0;
    // cha cha back to the right for prices because I didnt use QPair ha...
    for (year = price_histories.end() - 1; year != price_histories.begin() - 1; --year) {
        QList<QList<double>> months = year.value();

        // backwards from december
        for (int m = 11; m > -1; --m) {
            QList<double> days = months[m];

            // forward search
            bool empty = true;
            for (int d = 0; d < days.size(); ++d) {
                if (days[d] > -100000.0) {
                    empty = false;
                    QCPGraphData price;
                    QDate date = QDate(year.key(), m + 1, d + 1);
                    price.key = QCPAxisTickerDateTime::dateTimeToKey(date);
                    price.value = days[d];
                    prices_over_years.append(price);
                    if (largest_value < price.value)
                        largest_value = price.value;
                    if (date > end)
                        end = date;
                    else if (date < start)
                        start = date;
                }
            }
            if (!empty)
                count_limit++;
            if (count_limit == 12)
                break;
        }
        if (count_limit == 12)
            break;
    }
    ui->viz_cost_price_history->graph(0)->data()->set(costs_over_years);
    ui->viz_cost_price_history->graph(1)->data()->set(prices_over_years);
    ui->viz_cost_price_history->xAxis->setRange(QCPAxisTickerDateTime::dateTimeToKey(start),
                                                QCPAxisTickerDateTime::dateTimeToKey(end));
    ui->viz_cost_price_history->xAxis->ticker()->setTickOrigin(QCPAxisTickerDateTime::dateTimeToKey(start));
    ui->viz_cost_price_history->yAxis->setRangeUpper(largest_value + 3.0);
    ui->viz_cost_price_history->yAxis->setRangeLower(0.0);
    ui->viz_cost_price_history->replot();
}

void Recipes_Viewer::slot_request_granular_volatility_histories_response(QString recipe_name, QMap<int, QList<QList<double>>> volatility_histories)
{
    ui->viz_volatility_history->clearPlottables();
    m_volatility_history_title->setText("Volatility History for [" + recipe_name + "]");

    QDate start = QDate::currentDate();
    QDate end = QDate::currentDate();

    double largest_volatility = 0.0;

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(255, 0, 0));
    gradient.setColorAt(0.5, QColor(100, 100, 100));
    gradient.setColorAt(1, QColor(0, 0, 255));

    QCPBars* historic_volatilities = new QCPBars(ui->viz_volatility_history->xAxis, ui->viz_volatility_history->yAxis);
    historic_volatilities->setAntialiased(false);
    historic_volatilities->setName(recipe_name);
    historic_volatilities->setSelectable(QCP::stSingleData);
    historic_volatilities->setPen(QPen(QColor(50)));
    historic_volatilities->setBrush(QBrush(gradient));

    QMap<int, QList<QList<double>>>::iterator year;

    int count_limit = 0; // we only want to display the most recent 12 months, any more and the graph gets wack.
    // backwards! no use if we only show the first year when they're on their xth year (wishful thinking)
    for (year = volatility_histories.end() - 1; year != volatility_histories.begin() - 1; --year) {
        QList<QList<double>> months = year.value();

        // backwards from december
        for (int m = 11; m > -1; --m) {
            QList<double> days = months[m];

            // forward search
            bool empty = true;
            for (int d = 0; d < days.size(); ++d) {
                if (days[d] > -100000.0) {
                    empty = false;
                    QDate date = QDate(year.key(), m + 1, d + 1);
                    double volatility = days[d];
                    historic_volatilities->addData(QCPAxisTickerDateTime::dateTimeToKey(date), volatility);
                    if (largest_volatility < abs(volatility))
                        largest_volatility = abs(volatility);
                    if (date > end)
                        end = date;
                    else if (date < start)
                        start = date;
                }
            }
            if (!empty)
                count_limit++;
            if (count_limit == 12)
                break;
        }
        if (count_limit == 12)
            break;
    }
    ui->viz_volatility_history->xAxis->setRange(QCPAxisTickerDateTime::dateTimeToKey(start),
                                                QCPAxisTickerDateTime::dateTimeToKey(end));
    historic_volatilities->setWidth((QCPAxisTickerDateTime::dateTimeToKey(end) - QCPAxisTickerDateTime::dateTimeToKey(start))/(15 * volatility_histories.size()));
    ui->viz_cost_price_history->xAxis->ticker()->setTickOrigin(QCPAxisTickerDateTime::dateTimeToKey(start));
    //    ui->viz_volatility_history->rescaleAxes(true);
    ui->viz_volatility_history->yAxis->setRangeLower(-1.0 * largest_volatility - 0.5);
    ui->viz_volatility_history->yAxis->setRangeUpper(largest_volatility + 0.5);
    ui->viz_volatility_history->replot();
}

void Recipes_Viewer::slot_aight_we_be_updating(const QModelIndex& index)
{
   if (m_current_selection == index) {
       emit sig_request_recipe_data(index);
       emit sig_request_granular_cost_price_histories(index);
       emit sig_request_granular_volatility_histories(index);
   }
}
