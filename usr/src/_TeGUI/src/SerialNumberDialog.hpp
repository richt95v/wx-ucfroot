#ifndef SERIALNUMBERDIALOG_HPP
#define SERIALNUMBERDIALOG_HPP

#include <QDialog>

namespace Ui {
class SerialNumberDialog;
}

class SerialNumberDialog : public QDialog {

    Q_OBJECT

	public:
    explicit SerialNumberDialog(QWidget *parent = 0);
    ~SerialNumberDialog();

    QString Sn, ExSn;

	private slots:
    void on_ExSnInput_returnPressed();

    void on_SnInput_returnPressed();

    void on_SnInput_editingFinished();

    void on_ExSnInput_editingFinished();

	private:
    Ui::SerialNumberDialog *ui;
};

#endif // SERIALNUMBERDIALOG_HPP
