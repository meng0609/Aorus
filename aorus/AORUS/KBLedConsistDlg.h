#pragma once

#include "resource.h"
#include "inc/vga/Cfg.h"

// CVgaLedDlg dialog

class CKBLedConsistDlg : public CUiDlg
{ 
	typedef enum {
		STC_KBLEDS1_BRIGHTNESS,
		STC_KBLEDS1_SPEED,
		STC_KBLEDS1_COLORBG,
		STC_KBLEDS1_NUM
	};

	typedef enum {
		BTN_KBLEDS1_COLOR_1,
		BTN_KBLEDS1_COLOR_2,
		BTN_KBLEDS1_COLOR_3,
		BTN_KBLEDS1_COLOR_4,
		BTN_KBLEDS1_COLOR_5,
		BTN_KBLEDS1_COLOR_6,
		BTN_KBLEDS1_COLOR_7,
		BTN_KBLEDS1_COLOR_8,
		BTN_KBLEDS1_COLOR_9,
		BTN_KBLEDS1_NUM
	};

	typedef enum {
		EDIT_KBLEDS1_R,
		EDIT_KBLEDS1_G,
		EDIT_KBLEDS1_B,
		EDIT_KBLEDS1_NUM,
	};

	typedef enum {
		SLIDER_KBLEDS1_SPEED,
		SLIDER_KBLEDS1_BRGIHTNESS,
		SLIDER_KBLEDS1_RGB,
		SLIDER_KBLEDS1_NUM,
	};

	DECLARE_DYNAMIC(CKBLedConsistDlg)

public:
	CKBLedConsistDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKBLedConsistDlg();


// Dialog Data
	enum { IDD = IDD_VGA_LED_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnUiButtonClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiSliderMoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiSliderMoving(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiEditChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	void Ui();
	void UiMain();
	void OnOK() { } 
	void EditRGBChange();

	void OnColor(COLORREF clr);

private:
    CUiStatic m_stc[STC_KBLEDS1_NUM];
	CUiButton m_btn[BTN_KBLEDS1_NUM];
    CUiEdit	m_edit[EDIT_KBLEDS1_NUM];
	CUiSliderCtrl m_slider[SLIDER_KBLEDS1_NUM];

	static int s_nWords;
	static LANG_WORD s_Words[48];
};