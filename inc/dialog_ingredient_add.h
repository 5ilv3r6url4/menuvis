#ifndef DIALOG_INGREDIENT_ADD_H
#define DIALOG_INGREDIENT_ADD_H

#include <QWidget>
#include <QShortcut>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>

class Dialog_Ingredient_Add : public QWidget
{
    Q_OBJECT

public:

    Dialog_Ingredient_Add(QWidget *parent = 0);
    ~Dialog_Ingredient_Add();

private:

    QLineEdit* create_line_edit(QString text);

    QPushButton*    m_button_confirm;

    QLabel*         m_label_supplier;       QLineEdit*      m_input_supplier;
    QLabel*         m_label_category;       QLineEdit*      m_input_category;
    QLabel*         m_label_number;         QLineEdit*      m_input_number;
    QLabel*         m_label_name;           QLineEdit*      m_input_name;
    QLabel*         m_label_description;    QLineEdit*      m_input_description;
    QLabel*         m_label_brand;          QLineEdit*      m_input_brand;
    QLabel*         m_label_uom;            QLineEdit*      m_input_uom;
    QLabel*         m_label_amount;         QLineEdit*      m_input_amount;
    QLabel*         m_label_price;          QLineEdit*      m_input_price;

private slots:

    void slot_confirm();

signals:

    void signal_add_ingredient(QString, QString, QString, QString, QString, QString, QString, double, double);

};

#endif // DIALOG_INGREDIENT_ADD_H
