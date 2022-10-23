#include "UniTestExec_1uWindow.hpp"
#include "iTestExec-1u.hpp"
#include "iDbg.hpp"
#include "ui_UniTestExec_1uWindow.h"
#include <SerialNumberDialog.hpp>

#include <QtWidgets>
#include <QMainWindow>
#include <QMessageBox>
#include <thread>
#include <string>
#include <time.h>

#include "IOString.hpp"

using imx::IOString;

UniTestExec_1uWindow::UniTestExec_1uWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::UniTestExec_1uWindow) {

    QApplication::setStyle("Fusion");
    ui->setupUi(this);
	
    setWindowFlags( Qt::WindowMinimizeButtonHint| Qt::WindowMaximizeButtonHint);
    //setWindowFlags( Qt::FramelessWindowHint | Qt::WindowTitleHint );
    /*	get rid of the system menu
	  	1.	user is unable to even 'try' to close this window instead of the management interface
		*/
	
    ui->TestListTable->verticalHeader()->setFixedWidth(25);
    /* set the width of the row id in the table */

	TheTable.SetTable(ui->TestListTable);
    TheTable.SetDimensions(75,1);
    TheTable.SetColumnWidth({175});
    TheTable.SetColumnHeader({"Test Group"});

    SetWindowTitle("UniConsole 8u Test Executive");

    connect(this, &UniTestExec_1uWindow::_InitTestStatus, this, &UniTestExec_1uWindow::InitTestStatus_Slot);

    connect(this, &UniTestExec_1uWindow::_ClearCurrentTestStatus, this, &UniTestExec_1uWindow::ClearCurrentTestStatus_Slot);
    connect(this, &UniTestExec_1uWindow::_UpdateTestStatus, this, &UniTestExec_1uWindow::UpdateTestStatus_Slot);

	connect(this, &UniTestExec_1uWindow::_MarkUutAsTested, this, &UniTestExec_1uWindow::MarkUutAsTested_Slot);
    connect(this, &UniTestExec_1uWindow::_SetUutSn, this, &UniTestExec_1uWindow::SetUutSn_Slot);

    connect(this, &UniTestExec_1uWindow::_AddTest, this, &UniTestExec_1uWindow::AddTest_Slot);
	connect(this, &UniTestExec_1uWindow::_ShowDialog, this, &UniTestExec_1uWindow::ShowDialog_Slot);

    connect(this, &UniTestExec_1uWindow::_GetTextInput, this, &UniTestExec_1uWindow::GetTextInput_Slot);

    connect(this, &UniTestExec_1uWindow::_EnableSessionControls, this, &UniTestExec_1uWindow::EnableSessionControls_Slot);
    connect(this, &UniTestExec_1uWindow::_SetTimerDisplayLabel, this, &UniTestExec_1uWindow::SetTimerDisplayLabel_Slot);

	SetProgressBarPosition(1);
}

void UniTestExec_1uWindow::closeEvent(QCloseEvent *event) {

    //event->ignore();
	//ShowDialog(false, "The Test Executive\ncannot be closed directly");
}

void UniTestExec_1uWindow::keyPressEvent(QKeyEvent *event) {
/*  provide keyboard input control when the test exec gui has focus
    1.	'+' starts a test
    2.	'-' stops a test
    3.	'esc' moves focus to the console
    */

	static unsigned last_tab{0};

    switch(unsigned key_value=event->key(); key_value) {

        case(Qt::Key_Plus):
		/*	Start the test if the '+' key is hit */
			UniTestExec_1uWindow::on_StartButton_clicked();
			break;

        case(Qt::Key_Minus):
		/*	Stop the test if the '-' key is hit */
			UniTestExec_1uWindow::on_StopButton_clicked();
			break;

        case(Qt::Key_Up):
            break;

        case(Qt::Key_Down):
            break;

		case(Qt::Key_Escape):
			TheMgtIx->SetFocus();
			//ManagementInterface->activateWindow();
			break;

		case(Qt::Key_Control):
		/* if the ctrl key is hit
		   1.	...
		   */
			break;

        default:
            break;
    }
}

unsigned UniTestExec_1uWindow::AppendRow(unsigned width) {

	ui->TestListTable->insertRow(ui->TestListTable->rowCount());
	return(0);
}

unsigned UniTestExec_1uWindow::AppendColumn(IOString name,unsigned width) {

	TheTable.AppendColumn(name.ToString(),width);

	return(0);
}

void UniTestExec_1uWindow::AddTest(IOString name,IOString description) {

	emit (_AddTest(QString(name.c_str()), QString(description.c_str())));
}

void UniTestExec_1uWindow::AddTest_Slot(QString name,QString description) {

	std::string str1=name.toStdString();
	std::string str2=description.toStdString();

	TheTable.SetCellText(str1,TestCount,0)
    -> SetCellText(str2,TestCount,TheTable.Columns-1)
    //-> SetCellText(str2,TestCount,2)
    -> SetCellTextColor(TestCount,0,0,0,0)
	-> SetCellTextColor(TestCount,1,0,0,0)
	-> SetCellTextColor(TestCount,2,0,0,0)
	-> SetCellBackgroundColor(TestCount,0,255,255,255)
	-> SetCellBackgroundColor(TestCount,1,255,255,255)
	-> SetCellBackgroundColor(TestCount,2,255,255,255);

	++TestCount;
}

void UniTestExec_1uWindow::InitTestStatus() {

    emit(_InitTestStatus());
}

void UniTestExec_1uWindow::InitTestStatus_Slot() {

    for (unsigned row=0;row<TestCount; row++) {

        for (unsigned col=1;col<=SlotCount;col++) {

            TheTable.SetCellText(IOString(" "),row,col)-> SetCellTextColor(row,col,0,0,0)-> SetCellBackgroundColor(row,col,255,255,255);
        }
	}
}

void UniTestExec_1uWindow::ClearCurrentTestStatus() {

    emit(_ClearCurrentTestStatus());
}

void UniTestExec_1uWindow::ClearCurrentTestStatus_Slot() {

    for (unsigned row=1;row<TestCount; row++) {

        for (unsigned col=1;col<=SlotCount;col++) {

            TheTable.SetCellText(IOString(" "),row,col)-> SetCellTextColor(row,col,0,0,0)-> SetCellBackgroundColor(row,col,255,255,255);
        }
    }
}

void UniTestExec_1uWindow::SetProgressBarPosition(unsigned position) {

}

void UniTestExec_1uWindow::UpdateTestStatus(unsigned test_slot, int test_id, int status) {
    emit(_UpdateTestStatus(test_slot, test_id,status));
}

bool UniTestExec_1uWindow::UpdateTestStatus_Slot(int slot, int test_id, int status) {
/*  update the gui with the current test result
	1.	test_id is the 0 based index into the test list.
	2.	status is the pass/fail status (0=fail, 1=suspect, 2=pass
	*/

	switch(status) {

        case(0):
            if (test_id==0) TheTable.SetCellTextColor(test_id,1+slot,255,255,255)-> SetCellBackgroundColor(test_id,1+slot,220,0,0);
            else 			TheTable.SetCellText(string("Fail"),test_id,1+slot)-> SetCellTextColor(test_id,1+slot,255,255,255)-> SetCellBackgroundColor(test_id,1+slot,220,0,0);
            break;
        case(1):
		case(2):
            if (test_id==0)	TheTable.SetCellTextColor(test_id,1+slot,255,255,255)-> SetCellBackgroundColor(test_id,1+slot,0,140,0);
            else  			TheTable.SetCellText(string("Pass"),test_id,1+slot)-> SetCellTextColor(test_id,1+slot,255,255,255)-> SetCellBackgroundColor(test_id,1+slot,0,140,0);
            break;
        case(3):
            if (test_id==0) TheTable.SetCellTextColor(test_id,1+slot,255,255,255)-> SetCellBackgroundColor(test_id,1+slot,0,140,0);
            else 			TheTable.SetCellText(string("Complete"),test_id,1+slot)-> SetCellTextColor(test_id,1+slot,95,95,95)-> SetCellBackgroundColor(test_id,1+slot,155,155,155);
            break;
        default:
            if (test_id==0) TheTable.SetCellTextColor(test_id,1+slot,55,55,55)-> SetCellBackgroundColor(test_id,1+slot,255,255,255);
            else 			TheTable.SetCellText(string("---"),test_id,1+slot)-> SetCellTextColor(test_id,1+slot,55,55,55)-> SetCellBackgroundColor(test_id,1+slot,255,255,255);
            break;
    }
	
	return(true);
}

void UniTestExec_1uWindow::MarkUutAsTested(unsigned test_slot, int id) {

    emit(_MarkUutAsTested(test_slot,id));
}

void UniTestExec_1uWindow::MarkUutAsTested_Slot(unsigned slot_id, int id) {

    if (id!=0) {

        TheTable.SetCellText(string("+ +"),id,slot_id+1);
    }
}

void UniTestExec_1uWindow::SetUutSn(unsigned test_slot, IOString msg) {

    emit(_SetUutSn(test_slot,msg.c_str()));
}

void UniTestExec_1uWindow::SetUutSn_Slot(unsigned slot_id, QString msg) {

    std::string sn=msg.toStdString();
    int size=sn.size();

    if (size>10) {
    /* 	truncate the first characters if the sn is greater than 10 characers */

        sn.erase(0,size-10);
    }

    TheTable.SetCellText(sn,0,slot_id+1);
}

void UniTestExec_1uWindow::ShowDialog(bool error, IOString msg) {

	PendingUserSync=true;
	emit (_ShowDialog(error, msg.c_str()));

	while (PendingUserSync) {

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void UniTestExec_1uWindow::ShowDialog_Slot(bool error, QString msg) {

	QMessageBox msgBox;
	if (error) {
		msgBox.setIcon(QMessageBox::Critical);
	}
	else {

		msgBox.setIcon(QMessageBox::Information);
	}

	msgBox.setText(msg);
	msgBox.exec();
	PendingUserSync=false;
}

bool UniTestExec_1uWindow::GetUutID(IOString &sn, IOString &extended_sn) {

	return(true);
}

void UniTestExec_1uWindow::SetFocus() {

    //ui->StartButton->setFocus();
    activateWindow();
}

bool UniTestExec_1uWindow::GetUserInput(IOString prompt, IOString &result) {

	PendingTextInput=true;

	QString io=prompt.c_str();
	QString def=result.c_str();
    emit (_GetTextInput(io,def));

	while(PendingTextInput==true) {

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	result=LastTextInput;
	return(LastTextInputValid);
}

void UniTestExec_1uWindow::GetTextInput_Slot(QString prompt,QString default_value) {

    bool ok {false};

	LastTextInputValid=false;
	LastTextInput.clear();

    QString text = 
		QInputDialog::getText(
		   0, 
		   "Input dialog",
		   prompt,
		   QLineEdit::Normal,
		   default_value, 
		   &ok
		);

	if (ok && !text.isEmpty()) {

		LastTextInputValid=true;
		LastTextInput=text.toStdString();
	}

	PendingTextInput=false;
}

UniTestExec_1uWindow::~UniTestExec_1uWindow() {

    delete ui;
}

void UniTestExec_1uWindow::on_StartButton_clicked() {

    unsigned loop_count {1};
	string sn, ex_sn;
    using AppEngineInterfaces::iAppEngine;

	unique_ix<iAppEngine> app_engine(TheRepository);
	unique_ix<iTestExec_1u> exec(TheRepository);

    if (app_engine && app_engine->iErrorManager()->IsLocked()) {

        ShowDialog(true,"The system is currently locked. Testing is disabled");
    }
    else if (
        exec
		&&
		app_engine
		&&
        (!exec->TestSequenceIsCurrentlyInProgress())
		) {

        StopOnFirstFail=ui->GonogoCheckbox->isChecked();
        if (ui->LoopCountCheckbox->isChecked()) {

            loop_count=ui->LoopCountSpinBox->value();
        }

        exec-> LaunchStartTest(StopOnFirstFail,loop_count);
	}
	else {
		
        ShowDialog(true,"A test session is currently in progress");
	}
}

void UniTestExec_1uWindow::EnableSessionControls(bool enable) {

    emit _EnableSessionControls(enable);
}

void UniTestExec_1uWindow::EnableSessionControls_Slot(int value) {

    ui->GonogoCheckbox->setEnabled(value);
    ui->LoopCountCheckbox->setEnabled(value);
    ui->LoopCountSpinBox->setEnabled(value);
    ui->StartButton->setEnabled(value);
}

void UniTestExec_1uWindow::on_HelpButton_clicked() {

    using AppEngineInterfaces::iAppEngine;

    unique_ix<iAppEngine> app_engine(TheRepository);
    unique_ix<CmdRepository::iMethodServer> cmds(TheRepository);

	if (app_engine && cmds) {

        std::string cmd="ShowTestHelp";
        cmds->ExecuteCommand(cmd.c_str(),cmd.c_str());
	}
}

void UniTestExec_1uWindow::on_StopButton_clicked() {

    if (unique_ix<iTestExec_1u> exec(TheRepository); exec) {

        exec->AbortTestSequenceAsync();
	}
}

bool UniTestExec_1uWindow::GetLoopEnable() {

    return(ui->LoopCountCheckbox->isChecked());
}

void UniTestExec_1uWindow::on_GonogoCheckbox_clicked(bool checked) {

}

void UniTestExec_1uWindow::on_TestListTable_itemClicked(QTableWidgetItem *item) {

}

void UniTestExec_1uWindow::Initialize(iIXmgr *ix) {

    char buffer[17];

    TheRepository=ix;

    unique_ix<iTestExec_1u> exec(TheRepository);
    unsigned count=exec->GetSlotCount();

    unique_ix<iUniConsole::Console> uniconsole(TheRepository);

    memset(buffer,0,17);
    for (unsigned index=0;index<count;index++) {

        sprintf(buffer,"Slot %d",index);
        AppendColumn(buffer,100);
        ++SlotCount;
    }

    AppendColumn("Description",1000);
    SetTimerDisplayLabel("hr:min:sec");
}

void UniTestExec_1uWindow::SetWindowTitle(IOString name) {

	setWindowTitle(name.c_str());
}

void UniTestExec_1uWindow::SetTimerDisplayLabel(IOString label) {

    emit _SetTimerDisplayLabel(label.c_str());
}

void UniTestExec_1uWindow::SetTimerDisplayLabel_Slot(QString value)  {

    ui->TimerDisplayLabel->setText(value);
}

void UniTestExec_1uWindow::on_BreakOnTestGroupCheckbox_clicked(bool checked) {

    unique_ix<AppEngineInterfaces::iAppEngine> app_engine(TheRepository);
    app_engine->Debugger()->SetBreakPoint(checked, new TestExecutionEvent);
}


void UniTestExec_1uWindow::on_BreakOnSystemIOCheckBox_clicked(bool checked) {

    unique_ix<AppEngineInterfaces::iAppEngine> app_engine(TheRepository);
    app_engine->Debugger()->SetBreakPoint(checked, new IOEvent);
}


void UniTestExec_1uWindow::on_BreakOnTestFailCheckBox_clicked(bool checked) {

    unique_ix<AppEngineInterfaces::iAppEngine> app_engine(TheRepository);
    app_engine->Debugger()->SetBreakPoint(checked, new DataCollectionEvent);
}

void UniTestExec_1uWindow::on_BreakOnTestExceptionCheckBox_clicked(bool checked) {

    unique_ix<AppEngineInterfaces::iAppEngine> app_engine(TheRepository);
    app_engine->Debugger()->SetBreakPoint(checked, new ExceptionEvent);
}

