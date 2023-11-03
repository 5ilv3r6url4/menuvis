#include "dialog_recipe_category_add.h"

Dialog_Recipe_Category_Add::Dialog_Recipe_Category_Add(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Create New Category"));

    QSizePolicy policy_size_fixed(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_button_create = new QPushButton(tr("&Create"), this);
    m_button_create->setSizePolicy(policy_size_fixed);
    m_button_create->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

    m_input_category = new QLineEdit();
    m_input_category->setPlaceholderText("category.name");
    m_input_category->setFocus();

    m_label_status = new QLabel("Already Exists!");
    m_label_status->setStyleSheet("color: rgba(0, 0, 0, 0);");
    m_label_status->setLayoutDirection(Qt::LayoutDirection::LeftToRight);

    QVBoxLayout* layout_main = new QVBoxLayout(this);
    QGridLayout* layout_status_create = new QGridLayout();

    layout_main->addWidget(m_input_category, 0);
    layout_main->addLayout(layout_status_create, 1);
    layout_status_create->addWidget(m_label_status, 0, 0);
    layout_status_create->addWidget(m_button_create, 0, 1);

    connect(m_button_create,    SIGNAL(clicked()),
            this,               SLOT(slot_create_category()));

    connect(m_input_category,   SIGNAL(returnPressed()),
            this,               SLOT(slot_create_category()));

    connect(new QShortcut(QKeySequence::Quit, this), &QShortcut::activated, qApp, &QApplication::quit);
}

Dialog_Recipe_Category_Add::~Dialog_Recipe_Category_Add()
{
    delete m_button_create;
    delete m_input_category;
    delete m_label_status;
}

void Dialog_Recipe_Category_Add::slot_create_category() {

    bool temp_valid_input = true;
    m_label_status->setStyleSheet("color: rgba(0, 0, 0, 0);");

    if (m_input_category->text().isEmpty()) {
        temp_valid_input = false;
        m_input_category->setStyleSheet("background-color: rgba(255, 0, 0, 50);");
    }
    else
        m_input_category->setStyleSheet("background-color: white;");
    if (!temp_valid_input) {
        return;
    }

    emit sig_create_category(capitalize(m_input_category->text()));
}

void Dialog_Recipe_Category_Add::slot_create_category_response(int index)
{
    if (index == -1) {
        m_label_status->setStyleSheet("color: rgba(255, 0, 0, 255);");
        return;
    }

    m_input_category->clear();
    this->close();
}

QString Dialog_Recipe_Category_Add::capitalize(const QString& s)
{
    QStringList parts = s.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    return parts.join(" ");
}
