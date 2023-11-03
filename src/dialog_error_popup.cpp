#include "dialog_error_popup.h"

Dialog_Error_Popup::Dialog_Error_Popup(QString message, QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("ERROR!"));

    QSizePolicy policy_size_fixed(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout* main_layout = new QGridLayout(this);
    QPushButton* button_confirm = new QPushButton(tr("&OK"), this);
    QLabel* error_message = new QLabel(message);
    QLabel* spacer = new QLabel("");

    connect(button_confirm,   SIGNAL(clicked()),
            this,             SLOT(slot_confirm()));

    button_confirm->setSizePolicy(policy_size_fixed);
    button_confirm->setLayoutDirection(Qt::LayoutDirection::LayoutDirectionAuto);
    error_message->setStyleSheet("color: red; font: bold");
    error_message->setWordWrap(false);

    main_layout->addWidget(error_message, 0, 0, Qt::AlignHCenter);
    main_layout->addWidget(error_message, 1, 0, Qt::AlignHCenter);
    main_layout->addWidget(button_confirm, 2, 0, Qt::AlignHCenter);

    this->setLayout(main_layout);
}

Dialog_Error_Popup::~Dialog_Error_Popup() {}

void Dialog_Error_Popup::slot_confirm()
{
    this->close();
}
