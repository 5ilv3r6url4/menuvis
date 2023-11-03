#include "ingredients_staging_viewer.h"
#include "ui_ingredients_staging_viewer.h"

#include <QListView>

Ingredients_Staging_Viewer::Ingredients_Staging_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ingredients_Staging_Viewer)
{
    ui->setupUi(this);
    setWindowTitle(tr("Ingredient Staging Area"));

    this->m_input_model = new Ingredients_Staging_Model(1);
    ui->table_input_ingredients->setModel(m_input_model);

    this->m_output_model = new Ingredients_Staging_Model(0);
    ui->table_output_ingredients->setModel(m_output_model);

    this->m_input_selection_model = ui->table_input_ingredients->selectionModel();
    ui->table_input_ingredients->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_input_ingredients->setSelectionMode(QAbstractItemView::SingleSelection);

    this->m_output_selection_model = ui->table_output_ingredients->selectionModel();
    ui->table_output_ingredients->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_output_ingredients->setSelectionMode(QAbstractItemView::SingleSelection);

    // ---------------------------------------------------------------------------
    // combobox options come from ingredient global datas
    // disable staging inputs at start when no ingredient is in staging
    ui->cbox_staged_category->addItem("--Select Category--");
    ui->cbox_staged_category->addItems(Ingredients_Data_Globals::INGREDIENT_CATEGORIES);
    QStandardItemModel* cbox_staged_category_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_category->model());
    cbox_staged_category_model->item(0)->setEnabled(false);

    ui->cbox_staged_uom->addItem("--Select UOM--");
    ui->cbox_staged_uom->addItems(Ingredients_Data_Globals::ACCEPTED_UOMS);
    QStandardItemModel* cbox_staged_uom_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_uom->model());
    cbox_staged_uom_model->item(0)->setEnabled(false);
    clear_disable_staging_box();

    ui->cbox_create_category->addItem("--Select Category--");
    ui->cbox_create_category->addItems(Ingredients_Data_Globals::INGREDIENT_CATEGORIES);
    QStandardItemModel* cbox_create_category_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_category->model());
    cbox_create_category_model->item(0)->setEnabled(false);

    ui->cbox_create_uom->addItem("--Select UOM--");
    ui->cbox_create_uom->addItems(Ingredients_Data_Globals::ACCEPTED_UOMS);
    QStandardItemModel* cbox_create_uom_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_uom->model());
    cbox_create_uom_model->item(0)->setEnabled(false);

    clear_disable_staging_box();
    // ---------------------------------------------------------------------------
    // pricing guide uploads
    connect(ui->btn_browse_file,    SIGNAL(clicked()),
            this,                   SLOT(slot_open_filebrowser()));

    connect(ui->btn_upload_file,    SIGNAL(clicked()),
            this,                   SLOT(slot_upload_file()));

    QString default_folder = QString((QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)));
    ui->input_filepath->setPlaceholderText(QDir::toNativeSeparators(default_folder));
    ui->input_filepath->setReadOnly(true);

    ui->cbox_supplier->addItem("--Select Supplier--");
    ui->cbox_supplier->addItem("GFS");
    ui->cbox_supplier->addItem("SYSCO");

    QStandardItemModel* cbox_supplier_model = qobject_cast<QStandardItemModel *>(ui->cbox_supplier->model());
    cbox_supplier_model->item(0)->setEnabled(false);

    connect(ui->cbox_supplier,      SIGNAL(currentTextChanged(const QString&)),
            this,                   SLOT(slot_verify_supplier(const QString&)));

    connect(this,                   SIGNAL(sig_upload_gfs(QString)),
            this->m_input_model,    SLOT(slot_upload_gfs(QString)));

    connect(this,                   SIGNAL(sig_upload_sysco(QString)),
            this->m_input_model,    SLOT(slot_upload_sysco(QString)));

    connect(this->m_input_model,    SIGNAL(sig_error_message(QString)),
            this,                   SLOT(slot_error_message_display(QString)));

    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // selection changes at input widget as input to staging widget in realtime
    connect(m_input_selection_model,SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
            this,                   SLOT(slot_selection_change(const QItemSelection, const QItemSelection)));

    connect(this,                   SIGNAL(sig_request_staging_input_ingredient_data(const QModelIndex&)),
            this->m_input_model,    SLOT(slot_request_staging_input_ingredient_data(const QModelIndex&)));

    connect(this->m_input_model,    SIGNAL(sig_request_staging_input_ingredient_data_response(QString, QString, QString, QString, QString, double, double, QString, QString)),
            this,                   SLOT(slot_request_staging_input_ingredient_data_response(QString, QString, QString, QString, QString, double, double, QString, QString)));

    // ---------------------------------------------------------------------------
    // staging edits of ingredients
    QDoubleValidator* double_validator = new QDoubleValidator(0, 999, 4, this);
    QDoubleValidator* double_validator_price = new QDoubleValidator(0, 999, 2, this);
    ui->input_staged_amount->setValidator(double_validator);
    ui->input_create_amount->setValidator(double_validator);
    ui->input_create_price_total->setValidator(double_validator_price);

    m_staging_original_uom = "INVALID";
    m_staging_original_amount = -1.0;
    m_staging_original_price = -1.0;

    connect(ui->cbox_staged_uom,    SIGNAL(currentTextChanged(const QString&)),
            this,                   SLOT(slot_uom_change_response(const QString&)));
    connect(ui->input_staged_amount,SIGNAL(textEdited(const QString&)),
            this,                   SLOT(slot_amount_change_response(const QString&)));

    // refresh stylesheets after error resolutions
    connect(ui->input_staged_name,  &QLineEdit::textEdited,
            this,                   [this](){ui->input_staged_name->setStyleSheet(""); });
    connect(ui->input_staged_brand, &QLineEdit::textEdited,
            this,                   [this](){ui->input_staged_brand->setStyleSheet(""); });
    connect(ui->cbox_staged_category,       &QComboBox::currentTextChanged,
            this,                           [this](){ui->cbox_staged_category->setStyleSheet(""); });
    connect(ui->input_create_name,  &QLineEdit::textEdited,
            this,                   [this](){ui->input_create_name->setStyleSheet(""); });
    connect(ui->input_create_brand, &QLineEdit::textEdited,
            this,                   [this](){ui->input_create_brand->setStyleSheet(""); });
    connect(ui->input_create_supplier,      &QLineEdit::textEdited,
            this,                           [this](){ui->input_create_supplier->setStyleSheet(""); });
    connect(ui->input_create_price_total,   &QLineEdit::textEdited,
            this,                           [this](){ui->input_create_price_total->setStyleSheet(""); });
    connect(ui->input_create_amount,        &QLineEdit::textEdited,
            this,                           [this](){ui->input_create_amount->setStyleSheet(""); });
    connect(ui->cbox_create_category,       &QComboBox::currentTextChanged,
            this,                           [this](){ui->cbox_create_category->setStyleSheet(""); });
    connect(ui->cbox_create_uom,            &QComboBox::currentTextChanged,
            this,                           [this](){ui->cbox_create_uom->setStyleSheet(""); });

    // ---------------------------------------------------------------------------
    // adding to output list
    connect(ui->btn_staged_add,     SIGNAL(clicked()),
            this,                   SLOT(slot_remove_input_add_output_ingredient()));
    connect(ui->btn_create_create,  SIGNAL(clicked()),
            this,                   SLOT(slot_add_output_ingredient()));
    connect(this,                   SIGNAL(sig_remove_input_ingredient(const QModelIndex&)),
            this->m_input_model,    SLOT(slot_remove_input_ingredient(const QModelIndex&)));
    connect(this,                   SIGNAL(sig_add_output_ingredient(QString, QString, QString, QString, QString, double, double, QString, QString)),
            this->m_output_model,   SLOT(slot_add_output_ingredient(QString, QString, QString, QString, QString, double, double, QString, QString)));

    // ---------------------------------------------------------------------------
    // saving output list to working ingredient list
    // must do existence checks in order to update already existing at the upload step, pre-populate!
    connect(ui->btn_save_output_ingredients,    SIGNAL(clicked()),
            this,                               SLOT(slot_save()));
    connect(this,                   SIGNAL(sig_save_output_to_working(QString)),
            this->m_output_model,   SLOT(slot_save_output_to_working(QString)));

    // ---------------------------------------------------------------------------
    // clearing data and prepping, auto detection
    connect(this,                   SIGNAL(sig_clear_inputs()),
            this->m_input_model,    SLOT(slot_clear_inputs()));
    connect(this->m_input_model,    SIGNAL(sig_auto_detected_existing_ingredients()),
            this->m_output_model,   SLOT(slot_auto_detected_existing_ingredients_response()));

    connect(ui->btn_cancel,         SIGNAL(clicked()),
            this,                   SLOT(slot_clicked_cancel_response()));

    connect(this,                   SIGNAL(sig_clear_outputs()),
            this->m_output_model,   SLOT(slot_clear_outputs()));
}

Ingredients_Staging_Viewer::~Ingredients_Staging_Viewer()
{
    delete ui;
}

void Ingredients_Staging_Viewer::slot_open_filebrowser()
{
    QString autoexec =  QString("%1/autoexec.cfg").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find File"), autoexec, "xlsx file (*.xlsx)"));

    if (!file.isEmpty()) {
        ui->input_filepath->setText(file);
        ui->input_filepath->setStyleSheet("");
    }
}

void Ingredients_Staging_Viewer::slot_verify_supplier(const QString&)
{
    if (ui->cbox_supplier->currentIndex() != 0)
        ui->cbox_supplier->setStyleSheet("");
}

void Ingredients_Staging_Viewer::slot_upload_file()
{
    bool submit_ready = true;
    QString temp_fpath = ui->input_filepath->text();

    if (temp_fpath.isEmpty()) {
        ui->input_filepath->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else
        ui->input_filepath->setStyleSheet("");

    QString temp_supplier = ui->cbox_supplier->currentText();
    if (temp_supplier == "--Select Supplier--") {
        ui->cbox_supplier->setStyleSheet("color: red; padding: ;");
        submit_ready = false;
    }
    else
        ui->cbox_supplier->setStyleSheet("");

    if (submit_ready && temp_supplier == "GFS") {
        emit sig_clear_inputs();
        emit sig_upload_gfs(ui->input_filepath->text());
        ui->input_filepath->clear();
    }

    else if (submit_ready && temp_supplier == "SYSCO") {
        emit sig_clear_inputs();
        emit sig_upload_sysco(ui->input_filepath->text());
        ui->input_filepath->clear();
    }
}

void Ingredients_Staging_Viewer::slot_selection_change(const QItemSelection selected, const QItemSelection deselected)
{
    if (selected.indexes().isEmpty()) //deslection guard
        return;

    m_current_input_selection = m_input_selection_model->selection().indexes().first();
    emit sig_request_staging_input_ingredient_data(m_current_input_selection);
}

void Ingredients_Staging_Viewer::slot_request_staging_input_ingredient_data_response(QString supplier, QString category, QString name, QString brand, QString uom,
                                                                                     double amount, double price, QString number, QString description)
{
    clear_disable_staging_box();

    m_staging_original_uom = uom;
    m_staging_original_amount = amount;
    m_staging_original_price = price;

    ui->input_staged_name->setEnabled(true);
    ui->input_staged_description->setEnabled(true);
    ui->input_staged_amount->setEnabled(true);
    ui->input_staged_brand->setEnabled(true);
    ui->btn_staged_add->setEnabled(true);

    QStandardItemModel* cbox_staged_category_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_category->model());
    for (int i = 1; i <= Ingredients_Data_Globals::INGREDIENT_CATEGORIES.size(); ++i) {
        cbox_staged_category_model->item(i)->setEnabled(true);
    }

    ui->input_staged_name->setText(name);
    if (name == "MISSING")
        ui->input_staged_name->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    else
        ui->input_staged_name->setStyleSheet("");

    ui->out_staged_supplier->setText(supplier);
    ui->input_staged_brand->setText(brand);
    ui->input_staged_description->setText(description);

    if (uom == "INVALID") {
        QStandardItemModel* cbox_staged_uom_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_uom->model());
        for (int j = 1; j <= Ingredients_Data_Globals::ACCEPTED_UOMS.size(); ++j) {
            cbox_staged_uom_model->item(j)->setEnabled(true);
        }
        ui->cbox_staged_uom->setStyleSheet("QComboBox {color: red; padding: ;}");
    }
    else {
        QList<int> enable_uom_options = UOM_Utils::get_related_uom(Ingredients_Data_Globals::ACCEPTED_UOMS, uom);
        QStandardItemModel* cbox_staged_uom_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_uom->model());
        for (int i = 0; i < enable_uom_options.size(); ++i) {
            cbox_staged_uom_model->item(enable_uom_options[i] + 1)->setEnabled(true);
        }
        ui->cbox_staged_uom->setStyleSheet("");
        bool old_state = ui->cbox_staged_uom->blockSignals(true);
        ui->cbox_staged_uom->setCurrentIndex(Ingredients_Data_Globals::ACCEPTED_UOMS.indexOf(uom) + 1);
        ui->cbox_staged_uom->blockSignals(old_state);
    }

    if (amount < 0.0) {
        ui->input_staged_amount->setText(QString::number(0.0));
        ui->input_staged_amount->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else {
        ui->input_staged_amount->setText(QString::number(amount, 'f', 4));
        ui->input_staged_amount->setStyleSheet("");
    }

    ui->out_staged_price->setText(QString::number(price, 'f', 2));
    ui->out_staged_item->setText(number);
}

void Ingredients_Staging_Viewer::slot_uom_change_response(const QString& new_uom)
{
    double new_price = 0.0;
    if (m_staging_original_uom != "INVALID") {
        double new_amount = UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, m_staging_original_uom, new_uom, m_staging_original_amount);

        double original_total_price = m_staging_original_amount * m_staging_original_price;
        new_price = original_total_price / new_amount;

        ui->input_staged_amount->setText(QString::number(new_amount, 'f', 4));
        ui->out_staged_price->setText(QString::number(new_price, 'f', 2));
    }
    else if (m_staging_original_amount > 0.00009) {
        new_price = m_staging_original_price / m_staging_original_amount;
        ui->out_staged_price->setText(QString::number(new_price, 'f', 2));
    }
    if (new_price < 0.01 || new_price > 9999.99) {
        ui->out_staged_price->setStyleSheet("background-color: rgba(255, 0, 0, 255);");
    }
    else
        ui->out_staged_price->setStyleSheet("");
}

void Ingredients_Staging_Viewer::slot_amount_change_response(const QString &new_amount)
{
    double new_price = 0.00;
    if (m_staging_original_uom != "INVALID") {
        QString current_uom = ui->cbox_staged_uom->currentText();

        double original_amount_converted = UOM_Utils::convert_uoms(Ingredients_Data_Globals::ACCEPTED_UOMS, m_staging_original_uom, current_uom, m_staging_original_amount);
        double original_price_converted = UOM_Utils::convert_prices(Ingredients_Data_Globals::ACCEPTED_UOMS, m_staging_original_uom, current_uom, m_staging_original_price);
        double new_total_price = original_amount_converted * original_price_converted;
        new_price = new_total_price / new_amount.toDouble();
        ui->out_staged_price->setText(QString::number(new_price, 'f', 2));
    }
    else {
        m_staging_original_amount = new_amount.toDouble();
        if (ui->cbox_staged_uom->currentIndex() != 0) {
            new_price = m_staging_original_price / m_staging_original_amount;
            ui->out_staged_price->setText(QString::number(new_price, 'f', 2));
        }
    }
    if (new_price < 0.01 || new_price > 9999.99) {
        ui->out_staged_price->setStyleSheet("background-color: rgba(255, 0, 0, 255);");
    }
    else
        ui->out_staged_price->setStyleSheet("");
}

void Ingredients_Staging_Viewer::slot_remove_input_add_output_ingredient()
{
    QString out_name;
    QString out_supplier;
    QString out_brand;
    QString out_description;
    QString out_category;
    QString out_uom;
    QString out_number;
    double out_amount;
    double out_price;

    bool submit_ready = true;

    out_supplier = ui->out_staged_supplier->text();
    out_number = ui->out_staged_item->text();

    if (ui->input_staged_name->text().isEmpty()) {
        ui->input_staged_name->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_name = ui->input_staged_name->text();

    if (ui->input_staged_brand->text().isEmpty()) {
        ui->input_staged_brand->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        return;
    }
    else out_brand = ui->input_staged_brand->text();

    out_price = ui->out_staged_price->text().toDouble();
    if (out_price < 0.01 || out_price > 9999.99) {
        submit_ready = false;
    }

    if (ui->input_staged_description->toPlainText().isEmpty()) {
        out_description = "";
    }
    else out_description = ui->input_staged_description->toPlainText();

    if (ui->input_staged_amount->text().isEmpty() || ui->input_staged_amount->text().toDouble() == 0.0000) {
        ui->input_staged_amount->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_amount = ui->input_staged_amount->text().toDouble();

    if (ui->cbox_staged_category->currentIndex() == 0) {
        ui->cbox_staged_category->setStyleSheet("QComboBox {color: red; padding: ;}");
        submit_ready = false;
    }
    else out_category = ui->cbox_staged_category->currentText();

    if (ui->cbox_staged_uom->currentIndex() == 0) {
        ui->cbox_staged_uom->setStyleSheet("QComboBox {color: red; padding: ;}");
        submit_ready = false;
    }
    else out_uom = ui->cbox_staged_uom->currentText();

    if (!submit_ready)
        return;

    emit sig_remove_input_ingredient(m_current_input_selection);
    emit sig_add_output_ingredient(out_supplier, out_category, out_name, out_brand, out_uom, out_amount, out_price, out_number, out_description);

    ui->table_input_ingredients->clearSelection();
    clear_disable_staging_box();
}

void Ingredients_Staging_Viewer::slot_add_output_ingredient()
{
    QString out_name;
    QString out_supplier;
    QString out_brand;
    QString out_description;
    QString out_category;
    QString out_uom;
    QString out_number;
    double out_amount;
    double out_price;

    bool submit_ready = true;

    if (ui->input_create_supplier->text().isEmpty()) {
        ui->input_create_supplier->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_supplier = ui->input_create_supplier->text();

    if (ui->input_create_item_number->text().isEmpty()) {
        out_number = "9999999";
    }
    else out_number = ui->input_create_item_number->text();

    if (ui->input_create_name->text().isEmpty()) {
        ui->input_create_name->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_name = ui->input_create_name->text();

    if (ui->input_create_brand->text().isEmpty()) {
        ui->input_create_brand->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }
    else out_brand = ui->input_create_brand->text();

    out_amount = ui->input_create_amount->text().toDouble();
    if (ui->input_create_amount->text().isEmpty() || ui->input_create_amount->text().toDouble() == 0.0000) {
        ui->input_create_amount->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }

    double temp_total_price = ui->input_create_price_total->text().toDouble();
    if (temp_total_price == 0.00 || temp_total_price > 9999.99) {
        ui->input_create_price_total->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }

    out_price = temp_total_price / out_amount;
    if (out_price < 0.01 || out_price > 9999.99) {
        ui->input_create_price_total->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        ui->input_create_amount->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
        submit_ready = false;
    }

    if (ui->input_create_description->toPlainText().isEmpty()) {
        out_description = "";
    }
    else out_description = ui->input_create_description->toPlainText();

    if (ui->cbox_create_category->currentIndex() == 0) {
        ui->cbox_create_category->setStyleSheet("QComboBox {color: red; padding: ;}");
        submit_ready = false;
    }
    else out_category = ui->cbox_create_category->currentText();

    if (ui->cbox_create_uom->currentIndex() == 0) {
        ui->cbox_create_uom->setStyleSheet("QComboBox {color: red; padding: ;}");
        submit_ready = false;
    }
    else out_uom = ui->cbox_create_uom->currentText();

    if (!submit_ready)
        return;

    emit sig_add_output_ingredient(out_supplier, out_category, out_name, out_brand, out_uom, out_amount, out_price, out_number, out_description);
    clear_create_box();
}

void Ingredients_Staging_Viewer::slot_save()
{
    emit sig_save_output_to_working(ui->label_current_date->text());
    emit sig_clear_inputs();
    clear_disable_staging_box();
    clear_create_box();
    ui->cbox_supplier->setCurrentIndex(0);
    this->hide();
}

void Ingredients_Staging_Viewer::update_date_display()
{
    QString s = QDate::currentDate().toString("dd-MM-yyyy");
    ui->label_current_date->setText(s);
}

void Ingredients_Staging_Viewer::slot_error_message_display(QString message)
{
    Dialog_Error_Popup* dialog_error = new Dialog_Error_Popup(message);
    dialog_error->setAttribute( Qt::WA_DeleteOnClose );
    dialog_error->show();
}

void Ingredients_Staging_Viewer::clear_create_box()
{
    ui->input_create_name->clear();
//    ui->input_create_name->setStyleSheet("");
    ui->input_create_description->clear();
    ui->input_create_amount->clear();
//    ui->input_create_amount->setStyleSheet("");
    ui->input_create_brand->clear();
//    ui->input_create_brand->setStyleSheet("");
    ui->input_create_supplier->clear();
//    ui->input_create_supplier->setStyleSheet("");
    ui->input_create_price_total->clear();
//    ui->input_create_price_total->setStyleSheet("");
    ui->input_create_item_number->clear();
    ui->cbox_create_category->setCurrentIndex(0);
    ui->cbox_create_category->setStyleSheet("");
    ui->cbox_create_uom->setCurrentIndex(0);
    ui->cbox_create_uom->setStyleSheet("");
}

void Ingredients_Staging_Viewer::clear_disable_staging_box()
{
    m_staging_original_uom = "INVALID";
    m_staging_original_amount = -1.0;
    m_staging_original_price = -1.0;

    ui->input_staged_name->clear();
    ui->input_staged_description->clear();
    ui->input_staged_amount->clear();
    ui->input_staged_brand->clear();
    ui->out_staged_supplier->clear();

    ui->cbox_staged_category->setCurrentIndex(0);
    ui->cbox_staged_uom->setCurrentIndex(0);

    ui->input_staged_name->setEnabled(false);
    ui->input_staged_description->setEnabled(false);
    ui->input_staged_amount->setEnabled(false);
    ui->input_staged_brand->setEnabled(false);
    ui->btn_staged_add->setEnabled(false);

    ui->out_staged_price->setText("0.00");
    ui->out_staged_item->setText("0000000");

    ui->out_staged_price->setStyleSheet("");
    ui->input_staged_amount->setStyleSheet("");

    QStandardItemModel* cbox_staged_category_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_category->model());
    for (int i = 1; i <= Ingredients_Data_Globals::INGREDIENT_CATEGORIES.size(); ++i) {
        cbox_staged_category_model->item(i)->setEnabled(false);
    }

    QStandardItemModel* cbox_staged_uom_model = qobject_cast<QStandardItemModel *>(ui->cbox_staged_uom->model());
    for (int j = 1; j <= Ingredients_Data_Globals::ACCEPTED_UOMS.size(); ++j) {
        cbox_staged_uom_model->item(j)->setEnabled(false);
    }
}

void Ingredients_Staging_Viewer::slot_clicked_cancel_response()
{
    emit sig_clear_inputs();
    emit sig_clear_outputs();
    clear_disable_staging_box();
    clear_create_box();
    ui->input_filepath->clear();
    ui->cbox_supplier->setCurrentIndex(0);
    this->hide();
}
