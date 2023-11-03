#include "ingredients_viewer.h"
#include "ui_ingredients_viewer.h"

Ingredients_Viewer::Ingredients_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ingredients_Viewer)
{
    ui->setupUi(this);

    this->m_model = new Ingredients_Model();
    ui->pricing_table->setModel(m_model);

    this->m_selection_model = ui->pricing_table->selectionModel();
    ui->pricing_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pricing_table->setSelectionMode(QAbstractItemView::SingleSelection);

    this->m_view_ingredient_staging  = new Ingredients_Staging_Viewer();

    connect(ui->btn_upload,     SIGNAL(pressed()),
            this,               SLOT(slot_open_ingredient_staging()));

    connect(m_selection_model,  SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
            this,               SLOT(slot_selection_change(const QItemSelection, const QItemSelection)));

    connect(this,               SIGNAL(sig_request_ingredient_data(const QModelIndex&)),
            this->m_model,      SLOT(slot_request_ingredient_data(const QModelIndex&)));

    connect(this->m_model,      SIGNAL(sig_request_ingredient_data_response(QString, QString, QString, QString, QString, double, double, double, QString, QString)),
            this,               SLOT(slot_request_ingredient_data_response(QString, QString, QString, QString, QString, double, double, double, QString, QString)));

    connect(this,               SIGNAL(sig_request_aggregate_category_volatility(QString)),
            this->m_model,      SLOT(slot_request_aggregate_category_volatility(QString)));

    connect(this,               SIGNAL(sig_request_granular_category_pricing_histories(QString)),
            this->m_model,      SLOT(slot_request_granular_category_pricing_histories(QString)));

    connect(this->m_model,      SIGNAL(sig_request_granular_category_pricing_histories_response(QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>>>)),
            this,               SLOT(slot_request_granular_category_pricing_histories_response(QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>>>)));

    connect(this,               SIGNAL(sig_request_ingredient_volatility_histories(const QModelIndex&)),
            this->m_model,      SLOT(slot_request_ingredient_volatility_histories(const QModelIndex&)));

    connect(this->m_model,      SIGNAL(sig_request_ingredient_volatility_histories_response(QString, QMap<int, QList<QList<double>>>)),
            this,               SLOT(slot_request_ingredient_volatility_histories_response(QString, QMap<int, QList<QList<double>>>)));

    connect(this->m_model,      SIGNAL(sig_update_viz()),
            this,               SLOT(slot_update_viz_response()));

    // ---------------------------------------------------------------------------
    // historic prices viz
    m_current_category = "NA";
    ui->viz_historic_prices->legend->setVisible(true);
    ui->viz_historic_prices->legend->setFont(QFont("Helvetica", 9));
    QPen main_grid_pen(QColor(0, 0, 0));
    main_grid_pen.setWidth(2);

    ui->viz_historic_prices->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    QSharedPointer<QCPAxisTickerDateTime> date_ticker(new QCPAxisTickerDateTime);
    date_ticker->setDateTimeFormat("dd/MM/yy");
    ui->viz_historic_prices->xAxis->setTicker(date_ticker);
    ui->viz_historic_prices->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_historic_prices->xAxis->setTicks(true);
    ui->viz_historic_prices->xAxis->setTickLabels(true);
    ui->viz_historic_prices->xAxis->setLabel("Date");
    ui->viz_historic_prices->xAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_historic_prices->xAxis->setRangeUpper(QDateTime::currentDateTime().toTime_t());
    ui->viz_historic_prices->xAxis->setRangeLower(QCPAxisTickerDateTime::dateTimeToKey(QDate(2017, 1, 1)));

    QSharedPointer<Util_QCP_Axis_Dollars> dollar_ticker(new Util_QCP_Axis_Dollars);
    ui->viz_historic_prices->yAxis->setTicker(dollar_ticker);
    ui->viz_historic_prices->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_historic_prices->yAxis->setTicks(true);
    ui->viz_historic_prices->yAxis->setTickLabels(true);
    ui->viz_historic_prices->yAxis->setLabel("Price");
    ui->viz_historic_prices->yAxis->setLabelFont(QFont(QFont().family(), 9));

    ui->viz_historic_prices->axisRect()->setupFullAxesBox();

    m_price_history_title = new QCPTextElement(ui->viz_historic_prices, "Price History for Category [No Category Selected]", QFont("sans", 10, QFont::Bold));
    ui->viz_historic_prices->plotLayout()->insertRow(0);
    ui->viz_historic_prices->plotLayout()->addElement(0, 0, m_price_history_title);

    ui->viz_historic_prices->setInteractions(QCP::iSelectPlottables);

    ui->viz_historic_volatilities->legend->setVisible(true);
    ui->viz_historic_volatilities->legend->setFont(QFont("Helvetica", 9));

    ui->viz_historic_volatilities->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    ui->viz_historic_volatilities->xAxis->setTicker(date_ticker);
    ui->viz_historic_volatilities->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_historic_volatilities->xAxis->setSubTicks(true);
    ui->viz_historic_volatilities->xAxis->setTicks(true);
    ui->viz_historic_volatilities->xAxis->setTickLabels(true);
    ui->viz_historic_volatilities->xAxis->grid()->setSubGridVisible(true);
    ui->viz_historic_volatilities->yAxis->grid()->setZeroLinePen(main_grid_pen);
    ui->viz_historic_volatilities->xAxis->setLabel("Date");
    ui->viz_historic_volatilities->xAxis->setLabelFont(QFont(QFont().family(), 9));
    ui->viz_historic_volatilities->xAxis->setRangeUpper(QDateTime::currentDateTime().toTime_t());
    ui->viz_historic_volatilities->xAxis->setRangeLower(QCPAxisTickerDateTime::dateTimeToKey(QDate(2017, 1, 1)));

    QSharedPointer<Util_QCP_Axis_Percentage> percentage_ticker(new Util_QCP_Axis_Percentage);
    ui->viz_historic_volatilities->yAxis->setTicker(percentage_ticker);
    ui->viz_historic_volatilities->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->viz_historic_volatilities->yAxis->setSubTicks(true);
    ui->viz_historic_volatilities->yAxis->setTicks(true);
    ui->viz_historic_volatilities->yAxis->setTickLabels(true);
    ui->viz_historic_volatilities->yAxis->grid()->setSubGridVisible(true);
    ui->viz_historic_volatilities->yAxis->setLabel("Volatility");
    ui->viz_historic_volatilities->yAxis->setLabelFont(QFont(QFont().family(), 9));

    ui->viz_historic_volatilities->axisRect()->setupFullAxesBox();

    m_volatility_history_title = new QCPTextElement(ui->viz_historic_volatilities, "Volatility History for [No Ingredient Selected]", QFont("sans", 10, QFont::Bold));
    ui->viz_historic_volatilities->plotLayout()->insertRow(0);
    ui->viz_historic_volatilities->plotLayout()->addElement(0, 0, m_volatility_history_title);

    ui->viz_historic_volatilities->setInteractions(QCP::iSelectPlottables);

}

Ingredients_Viewer::~Ingredients_Viewer()
{
    delete this->m_view_ingredient_staging;
    delete this->m_model;
    delete ui;
}

void Ingredients_Viewer::slot_open_ingredient_staging()
{
    m_view_ingredient_staging->show();
    m_view_ingredient_staging->raise();
    m_view_ingredient_staging->update_date_display();

}

void Ingredients_Viewer::slot_selection_change(const QItemSelection selected, const QItemSelection deselected)
{
    if (selected.indexes().isEmpty()) //deslection guard
        return;

    emit sig_request_ingredient_data(selected.indexes().first());
}

void Ingredients_Viewer::slot_request_ingredient_data_response(QString supplier, QString category, QString name, QString brand, QString uom,
                                                               double amount, double price, double volatility, QString number, QString description)
{
    ui->out_ingredient_name->setText(name);
    ui->out_ingredient_supplier->setText(supplier);
    ui->out_ingredient_brand->setText(brand);
    ui->out_ingredient_category->setText(category);
    ui->out_ingredient_amount->setText(QString::number(amount, 'f', 4) + uom);
    ui->out_ingredient_price->setText("$" + QString::number(price, 'f', 2));
    ui->out_ingredient_volatility->setText(QString::number(volatility, 'f', 1));

    if (m_current_category != category) {
        m_price_history_title->setText("Price History for Category [" + category + "]");
        ui->viz_historic_prices->replot();
        m_current_category = category;
        emit sig_request_granular_category_pricing_histories(category);
    }

    if (m_current_selection != m_selection_model->selection().indexes().first()) {
        m_current_selection = m_selection_model->selection().indexes().first();
        for (int i = 0; i < m_pricing_history_model_link.size(); ++i) {
            if (m_pricing_history_model_link[i] == m_current_selection) {
                ui->viz_historic_prices->graph(i)->setSelection(QCPDataSelection(ui->viz_historic_prices->graph(i)->data()->dataRange()));
                continue;
            }
            ui->viz_historic_prices->graph(i)->setSelection(QCPDataSelection());
        }
        ui->viz_historic_prices->replot();
    }

    m_volatility_history_title->setText("Volatility History for [" + name + "]");
    ui->viz_historic_volatilities->replot();
    emit sig_request_ingredient_volatility_histories(m_current_selection);
}

void Ingredients_Viewer::slot_update_viz_response()
{
    emit sig_request_granular_category_pricing_histories(m_current_category);
    emit sig_request_ingredient_volatility_histories(m_current_selection);
}

void Ingredients_Viewer::slot_request_granular_category_pricing_histories_response(QList<std::tuple<QModelIndex, QString, QMap<int, QList<QList<double>>>>> pricing_histories)
{
    if (pricing_histories.size() == 0)
        return;

    ui->viz_historic_prices->clearGraphs();
    m_pricing_history_model_link.clear();

    double largest_value = 0.0;

    QDate start = QDate::currentDate();
    QDate end = QDate::currentDate();

    for (int i = 0; i < pricing_histories.size(); ++i) {

        ui->viz_historic_prices->addGraph();
        m_pricing_history_model_link.append(std::get<0>(pricing_histories[i]));

        if (i < COLOR_UTIL.size())
            ui->viz_historic_prices->graph(i)->setPen(QPen(QColor(COLOR_UTIL[i])));
        else
            ui->viz_historic_prices->graph(i)->setPen(QPen(QColor(qSin(i*1+1.2)*80+80, qSin(i*0.3+0)*80+80, qSin(i*0.3+1.5)*80+80)));
        ui->viz_historic_prices->graph(i)->setLineStyle(QCPGraph::LineStyle::lsLine);
        ui->viz_historic_prices->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
        ui->viz_historic_prices->graph(i)->setSelectable(QCP::stWhole);
        ui->viz_historic_prices->graph(i)->setName(std::get<1>(pricing_histories[i])); // ingredient name

        QMap<int, QList<QList<double>>> pricing_history = std::get<2>(pricing_histories[i]);
        QMap<int, QList<QList<double>>>::iterator year;

        int count_limit = 0; // we only want to display the most recent 12 months, any more and the graph gets wack.
        QVector<QCPGraphData> prices_over_years;

        // backwards! no use if we only show the first year when they're on their xth year (wishful thinking)
        for (year = pricing_history.end() - 1; year != pricing_history.begin() - 1; --year) {
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
        ui->viz_historic_prices->graph(i)->data()->set(prices_over_years);
    }
    ui->viz_historic_prices->xAxis->setRange(QCPAxisTickerDateTime::dateTimeToKey(start),
                                             QCPAxisTickerDateTime::dateTimeToKey(end));
    ui->viz_historic_prices->xAxis->ticker()->setTickOrigin(QCPAxisTickerDateTime::dateTimeToKey(start));
    ui->viz_historic_prices->yAxis->setRangeUpper(largest_value);
    ui->viz_historic_prices->yAxis->setRangeLower(0.0);
    ui->viz_historic_prices->replot();
}

void Ingredients_Viewer::slot_request_ingredient_volatility_histories_response(QString ingredient_name, QMap<int, QList<QList<double>>> volatility_histories)
{
    ui->viz_historic_volatilities->clearPlottables();

    QDate start = QDate::currentDate();
    QDate end = QDate::currentDate();

    double largest_volatility = 0.0;

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(255, 0, 0));
    gradient.setColorAt(0.5, QColor(100, 100, 100));
    gradient.setColorAt(1, QColor(0, 0, 255));

    QCPBars* historic_volatilities = new QCPBars(ui->viz_historic_volatilities->xAxis, ui->viz_historic_volatilities->yAxis);
    historic_volatilities->setAntialiased(false);
    historic_volatilities->setName(ingredient_name);
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
    ui->viz_historic_volatilities->xAxis->setRange(QCPAxisTickerDateTime::dateTimeToKey(start),
                                                   QCPAxisTickerDateTime::dateTimeToKey(end));
    historic_volatilities->setWidth((QCPAxisTickerDateTime::dateTimeToKey(end) - QCPAxisTickerDateTime::dateTimeToKey(start))/(15 * volatility_histories.size()));
    ui->viz_historic_prices->xAxis->ticker()->setTickOrigin(QCPAxisTickerDateTime::dateTimeToKey(start));
    //    ui->viz_historic_volatilities->rescaleAxes(true);
    ui->viz_historic_volatilities->yAxis->setRangeLower(-1.0 * largest_volatility - 0.5);
    ui->viz_historic_volatilities->yAxis->setRangeUpper(largest_volatility + 0.5);
    ui->viz_historic_volatilities->replot();
}

