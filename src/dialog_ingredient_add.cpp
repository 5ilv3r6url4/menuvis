#include "dialog_ingredient_add.h"

Dialog_Ingredient_Add::Dialog_Ingredient_Add(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Add Ingredient"));

    m_button_confirm = new QPushButton(tr("&Add"), this);

    connect(m_button_confirm,   SIGNAL(clicked()),
            this,               SLOT(slot_confirm()));

    m_input_supplier    = create_line_edit("item.supplier");
    m_input_category    = create_line_edit("item.category");
    m_input_number      = create_line_edit("item.number");
    m_input_name        = create_line_edit("item.name");
    m_input_description = create_line_edit("item.description");
    m_input_brand       = create_line_edit("item.brand");
    m_input_uom         = create_line_edit("item.uom");
    m_input_amount      = create_line_edit("item.amount");
    m_input_price       = create_line_edit("item.price");

    m_input_amount->setValidator(new QDoubleValidator(0, 999, 2, this));
    m_input_price->setValidator(new QDoubleValidator(0, 999, 2, this));

    QGridLayout* layout_main = new QGridLayout(this);

    m_label_supplier    = new QLabel(tr("Supplier:"));
    m_label_category    = new QLabel(tr("Category:"));
    m_label_number      = new QLabel(tr("Item Number:"));
    m_label_name        = new QLabel(tr("Name:"));
    m_label_description = new QLabel(tr("Description:"));
    m_label_brand       = new QLabel(tr("Brand:"));
    m_label_uom         = new QLabel(tr("Units of Measure:"));
    m_label_amount      = new QLabel(tr("Amount:"));
    m_label_price       = new QLabel(tr("Price/UOM:"));

    layout_main->addWidget(m_label_supplier, 1, 0);
    layout_main->addWidget(m_label_category, 2, 0);
    layout_main->addWidget(m_label_number, 3, 0);
    layout_main->addWidget(m_label_name, 4, 0);
    layout_main->addWidget(m_label_description, 5, 0);
    layout_main->addWidget(m_label_brand, 6, 0);
    layout_main->addWidget(m_label_uom, 7, 0);
    layout_main->addWidget(m_label_amount, 8, 0);
    layout_main->addWidget(m_label_price, 9, 0);

    layout_main->addWidget(m_input_supplier, 1, 1);
    layout_main->addWidget(m_input_category, 2, 1);
    layout_main->addWidget(m_input_number, 3, 1);
    layout_main->addWidget(m_input_name, 4, 1);
    layout_main->addWidget(m_input_description, 5, 1);
    layout_main->addWidget(m_input_brand, 6, 1);
    layout_main->addWidget(m_input_uom, 7, 1);
    layout_main->addWidget(m_input_amount, 8, 1);
    layout_main->addWidget(m_input_price, 9, 1);

    layout_main->addWidget(new QLabel(tr("")), 10, 1);
    layout_main->addWidget(m_button_confirm, 11, 1);

    connect(new QShortcut(QKeySequence::Quit, this), &QShortcut::activated, qApp, &QApplication::quit);
}

Dialog_Ingredient_Add::~Dialog_Ingredient_Add()
{
    delete m_button_confirm;
    delete m_label_supplier;
    delete m_input_supplier;
    delete m_label_category;
    delete m_input_category;
    delete m_label_number;
    delete m_input_number;
    delete m_label_name;
    delete m_input_name;
    delete m_label_description;
    delete m_input_description;
    delete m_label_brand;
    delete m_input_brand;
    delete m_label_uom;
    delete m_input_uom;
    delete m_label_amount;
    delete m_input_amount;
    delete m_label_price;
    delete m_input_price;
}

void Dialog_Ingredient_Add::slot_confirm() {

    bool temp_valid_input = true;

    if (m_input_supplier->text().isEmpty()) {
        temp_valid_input = false;
        m_input_supplier->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_supplier->setStyleSheet("background-color: white;");
    if (m_input_category->text().isEmpty()) {
        temp_valid_input = false;
        m_input_category->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_category->setStyleSheet("background-color: white;");
    if (m_input_number->text().isEmpty()) {
        temp_valid_input = false;
        m_input_number->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_number->setStyleSheet("background-color: white;");
    if (m_input_name->text().isEmpty()) {
        temp_valid_input = false;
        m_input_name->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_name->setStyleSheet("background-color: white;");
    if (m_input_description->text().isEmpty()) {
        temp_valid_input = false;
        m_input_description->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_description->setStyleSheet("background-color: white;");
    if (m_input_brand->text().isEmpty()) {
        temp_valid_input = false;
        m_input_brand->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_brand->setStyleSheet("background-color: white;");
    if (m_input_uom->text().isEmpty()) {
        temp_valid_input = false;
        m_input_uom->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_uom->setStyleSheet("background-color: white;");
    if (m_input_amount->text().isEmpty()) {
        temp_valid_input = false;
        m_input_amount->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_amount->setStyleSheet("background-color: white;");
    if (m_input_price->text().isEmpty()) {
        temp_valid_input = false;
        m_input_price->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_price->setStyleSheet("background-color: white;");
    if (!temp_valid_input) {
        return;
    }

    emit signal_add_ingredient(m_input_supplier->text(), m_input_category->text(), m_input_number->text(), m_input_name->text(), m_input_description->text(),
                               m_input_brand->text(), m_input_uom->text(), m_input_amount->text().toDouble(), m_input_price->text().toDouble());

    this->close();

    m_input_supplier->clear();
    m_input_category->clear();
    m_input_number->clear();
    m_input_name->clear();
    m_input_description->clear();
    m_input_brand->clear();
    m_input_uom->clear();
    m_input_amount->clear();
    m_input_price->clear();
}

QLineEdit* Dialog_Ingredient_Add::create_line_edit(QString text)
{
    QLineEdit* in_line_edit = new QLineEdit;
    in_line_edit->setPlaceholderText(text);
    in_line_edit->setFocus();
    return in_line_edit;
}
