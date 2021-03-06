#pragma once

#include "KBLedStyleDlg.h"

// CKBLedDlg dialog

class CKBLedDlg : public CUiDlg
{
	typedef enum 
	{
	//	BTN_KBLED_PROADD = 0,
		BTN_KBLED_PROEDIT,
	//	BTN_KBLED_PRODEL,
		BTN_KBLED_APPLY,
		BTN_KBLED_DEFAULT,
		BTN_KBLED_SYNCDEVICE,
		BTN_KBLED_NUM
	};

	typedef enum
	{
		COMBOX_KBLED_PROFILE = 0,
		COMBOX_KBLED_SELKEYS,
		COMBOX_KBLED_NUM,
	};

	typedef enum
	{
		EDIT_KBLED_PROFILENAME,
	};
	typedef enum
	{
		STC_KBLED_SYNCDEVICE,
		STC_KBLED_NUM,
	};

	DECLARE_DYNAMIC(CKBLedDlg)

public:
	CKBLedDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKBLedDlg();
	void InitProfile();
	void InitState();
	void UpdateUI();
	void SetApplyBtnEnable(bool bEnable);	// Peter Lee 2017/6/18
	void SetCancelBtnEnable(bool bEnable);	// Peter Lee 2017/6/18
	static DWORD ApplyThreadProc(LPVOID lpParameter);
	static UINT SwitchKeyThread(LPVOID lpParam);
	static UINT DefaultThread(LPVOID lpParam);
// Dialog Data
	enum { IDD = IDD_VGA_LED_DIALOG };

	CKBLedStyleDlg m_LedStyleDlg;
	int m_nProfileSel;
	void ChangeCurProfile(int iProfile) {m_nProfileSel = iProfile;}
	void ApplySetting() {OnApply(); }

	bool bSyncFlag;//統一控燈標誌位



protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnUiButtonClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiComboBoxSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
	void Ui();
	void UiMain();

	bool IsAllKeysSelected();
	void SwitchKeysType(int nKeys);
	void SwitchKeyStatus(int iIndex);
	void SetKeysSelected(int keysSelIndex[], int nKeys);
	void SetItemStatus(int count, int arr[]);
	void OnApply();	
	bool isAllKey();
	void SwitchLedNav(int nIndex);
	CString GetProfilePath(int nProfile);
	void SetProfile(int nProfile);
	void OnDefault();

	void SetSyncLed(bool bSyncLed);
	bool IsSyncLed();
	void SyncLedSetting(LED_SETTING& led, bool bSave); // sync led setting between different devices.
	void ApplyLedSetting(int nCard, LED_SETTING& led, bool bLedSync, bool bSave /*true: save, false: only store in memory temperialy*/);
	
	
private:
	int m_nKeys; // 104/105/106/107...
	CUiStatic m_stc[STC_KBLED_NUM];
	CUiKeyboardCtrl m_KB;
	CUiButton m_btn[BTN_KBLED_NUM];
	CUiComboBox m_combobox[COMBOX_KBLED_NUM];
	CString         m_sCurFile;               //当前所选择项目
	int             m_iAllType;               //上次全选时的类型

	CUiEdit m_edit;

	static int s_nWords;
	static LANG_WORD s_Words[48];

	int m_nProSel;
	int m_nDevice;
	LED_SETTING m_kbProfile[GVDISP_CARD_MAXNUM][LED_PROFILE_CONT];
};
