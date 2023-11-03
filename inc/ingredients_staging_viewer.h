#ifndef INGREDIENTS_STAGING_VIEWER_H
#define INGREDIENTS_STAGING_VIEWER_H

#include "ingredients_staging_model.h"

#include "dialog_error_popup.h"

#include <QWidget>
#include <QDir>
#include <QDesktopServices>
#include <QShortcut>
#include <QFileDialog>
#include <QApplication>
#include <QItemSelection>
#include <QStandardItemModel>
#include <QDate>
#include <QDebug>

#include "util_uom.h"

namespace Ui {
class Ingredients_Staging_Viewer;
}

class Ingredients_Staging_Viewer : public QWidget
{
    Q_OBJECT

public:

    explicit Ingredients_Staging_Viewer(QWidget *parent = 0);
    ~Ingredients_Staging_Viewer();

    void update_date_display();

private:

    void clear_disable_staging_box();
    void clear_create_box();

    Ui::Ingredients_Staging_Viewer* ui;
    Ingredients_Staging_Model* m_input_model;
    Ingredients_Staging_Model* m_output_model;

    QDir m_current_directory;

    QItemSelectionModel*    m_input_selection_model;
    QItemSelectionModel*    m_output_selection_model;
    QModelIndex             m_current_input_selection;

    QString m_staging_original_uom;
    double m_staging_original_amount;
    double m_staging_original_price;

private slots:

    void slot_open_filebrowser();
    void slot_verify_supplier(const QString&);
    void slot_upload_file();

    void slot_selection_change(const QItemSelection selected, const QItemSelection deselected);
    void slot_request_staging_input_ingredient_data_response(QString supplier, QString category, QString name, QString brand, QString uom,
                                                             double amount, double price, QString number, QString description);

    void slot_uom_change_response(const QString& new_uom);
    void slot_amount_change_response(const QString& new_amount);

    void slot_remove_input_add_output_ingredient();
    void slot_add_output_ingredient();

    void slot_save();

    void slot_clicked_cancel_response();
    void slot_error_message_display(QString message);

signals:

    void sig_clear_inputs();
    void sig_clear_outputs();
    void sig_upload_gfs(QString file);
    void sig_upload_sysco(QString file);
    void sig_request_staging_input_ingredient_data(const QModelIndex&);

    void sig_remove_input_ingredient(const QModelIndex&);
    void sig_add_output_ingredient(QString, QString, QString, QString, QString, double, double, QString, QString);

    void sig_save_output_to_working(QString);

};

#endif // INGREDIENTS_STAGING_VIEWER_H
