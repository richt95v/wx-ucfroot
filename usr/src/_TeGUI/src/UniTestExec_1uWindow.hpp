#ifndef UNITESTEXEC_1UWINDOW_HPP
#define UNITESTEXEC_1UWINDOW_HPP

#include <QMainWindow>
#include <QTableWidgetItem>

#include "iUniConsole.hpp"
#include <iAppEngine.hpp>
#include "iTestExecGui-1u.hpp"
#include "iTestExecGuiMgr.hpp"
#include "QTableObject.hpp"
#include "IxMgr.hpp"

namespace Ui {

    class UniTestExec_1uWindow;
}

using imx::IOString;

class UniTestExec_1uWindow : public QMainWindow, public iTestExecGui_1u {

    Q_OBJECT

    public:

    explicit UniTestExec_1uWindow(QWidget *parent = nullptr);

    void SetMgmtIx(iUniConsole::Console *ptr) override { TheMgtIx=ptr; }

	void SetWindowTitle(IOString name) override;

    void AddTest(IOString name,IOString description) override;

    unsigned AppendRow(unsigned width) override;
    unsigned AppendColumn(IOString name,unsigned width) override;

    void EnableSessionControls(bool enable) override;

    void SetTimerDisplayLabel(IOString label) override;
    void InitTestStatus() override;
    void ClearCurrentTestStatus() override;

    void UpdateTestStatus(unsigned test_slot, int test_id,int status) override;
	/*  update the gui with the current test result
	    1.	test_id is the 0 based index into the test list.
	    2.	status is the pass/fail status (0=fail, 1=suspect, 2=pass
	    */

    void Initialize(iIXmgr *ix) override;

    void SetUutSn(unsigned test_slot, IOString msg) override;
    void MarkUutAsTested(unsigned test_slot, int id) override;

    void SetProgressBarPosition(unsigned position) override;

    void ShowDialog(bool error, IOString msg) override;
    bool GetUutID(IOString &sn, IOString &extended_sn) override;

    bool GetUserInput(IOString prompt, IOString &result) override;

    bool GetLoopEnable() override;

    void SetFocus() override;

    ~UniTestExec_1uWindow() override;

    signals:
    void _GetTextInput(QString prompt,QString default_value);
    void _UpdateTestStatus(int test_slot, int test_id,int status);

    void _InitTestStatus();
    void _ClearCurrentTestStatus();

    void _MarkUutAsTested(unsigned slot_id, int id);
	void _AddTest(QString name, QString description);
	void _ShowDialog(bool error, QString msg);
    void _SetUutSn(unsigned test_slot,QString msg);
    void _EnableSessionControls(int value);
    void _SetTimerDisplayLabel(QString value);

    public slots:

    void SetTimerDisplayLabel_Slot(QString value);
    void ShowDialog_Slot(bool error, QString msg);

    void EnableSessionControls_Slot(int value);
    void AddTest_Slot(QString name,QString description);

    void MarkUutAsTested_Slot(unsigned slot_id, int id);

    void SetUutSn_Slot(unsigned slot_id, QString msg);
    bool UpdateTestStatus_Slot(int test_slot, int test_id,int status);

    void InitTestStatus_Slot();
    void ClearCurrentTestStatus_Slot();

    void on_StartButton_clicked();
    void on_HelpButton_clicked();
    void on_StopButton_clicked();
    void on_GonogoCheckbox_clicked(bool checked);
    void on_TestListTable_itemClicked(QTableWidgetItem *item);
    void GetTextInput_Slot(QString prompt,QString default_value);

    private slots:
    void on_BreakOnTestExceptionCheckBox_clicked(bool checked);
    void on_BreakOnTestFailCheckBox_clicked(bool checked);
    void on_BreakOnSystemIOCheckBox_clicked(bool checked);
    void on_BreakOnTestGroupCheckbox_clicked(bool checked);

private:
    AppEngineInterfaces::iAppEngine *TheAppEngine {nullptr};

	bool StopOnFirstFail {false};

    iUniConsole::Console *TheUniConsole {nullptr};
	unsigned TestCount {0};
    unsigned SlotCount {0};

	std::string LastTextInput;
	bool LastTextInputValid {false};
	bool PendingTextInput 	{false};
	bool PendingUserSync 	{false};

    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    Ui::UniTestExec_1uWindow *ui;
	iIXmgr *TheRepository {nullptr};

	QTableObject TheTable;
    iUniConsole::Console *TheMgtIx {nullptr};
    iDbg *TheDebugger {nullptr};
};

#endif // UNITESTEXEC_1UWINDOW_HPP
