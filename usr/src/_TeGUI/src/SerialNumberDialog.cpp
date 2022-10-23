#include "SerialNumberDialog.hpp"
#include "ui_SerialNumberDialog.h"

SerialNumberDialog::SerialNumberDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SerialNumberDialog) {
    ui->setupUi(this);
}

SerialNumberDialog::~SerialNumberDialog() {
    delete ui;
}

void SerialNumberDialog::on_ExSnInput_returnPressed() { 
	ExSn=ui->ExSnInput->text();
}

void SerialNumberDialog::on_SnInput_returnPressed() {

	Sn=ui->SnInput->text();
}

void SerialNumberDialog::on_SnInput_editingFinished() {

	Sn=ui->SnInput->text();
}

void SerialNumberDialog::on_ExSnInput_editingFinished() {

	ExSn=ui->ExSnInput->text();
}
