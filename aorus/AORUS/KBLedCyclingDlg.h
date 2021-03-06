#pragma once

#include "resource.h"
#include "inc/vga/Cfg.h"

// CVgaLedDlg dialog

class CKBLedCyclingDlg : public CUiDlg
{ 
	typedef enum {
		STC_KBLEDS5_BRIGHTNESS,
		STC_KBLEDS5_SPEED,
		STC_KBLEDS5_COLORBG,
		STC_KBLEDS5_NUM
	};

	typedef enum {
		BTN_KBLEDS5_COLOR_1,
		BTN_KBLEDS5_COLOR_2,
		BTN_KBLEDS5_COLOR_3,
		BTN_KBLEDS5_COLOR_4,
		BTN_KBLEDS5_COLOR_5,
		BTN_KBLEDS5_COLOR_6,
		BTN_KBLEDS5_COLOR_7,
		BTN_KBLEDS5_COLOR_8,
		BTN_KBLEDS5_COLOR_9,
		BTN_KBLEDS5_NUM
	};

	typedef enum {
		EDIT_KBLEDS5_R,
		EDIT_KBLEDS5_G,
		EDIT_KBLEDS5_B,
		EDIT_KBLEDS5_NUM,
	};

	typedef enum {
		SLIDER_KBLEDS5_SPEED,
		SLIDER_KBLEDS5_BRGIHTNESS,
		SLIDER_KBLEDS5_RGB,
		SLIDER_KBLEDS5_NUM,
	};

	DECLARE_DYNAMIC(CKBLedCyclingDlg)

public:
	CKBLedCyclingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKBLedCyclingDlg();


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
    CUiStatic m_stc[STC_KBLEDS5_NUM];
	CUiButton m_btn[BTN_KBLEDS5_NUM];
    CUiEdit	m_edit[EDIT_KBLEDS5_NUM];
	CUiSliderCtrl m_slider[SLIDER_KBLEDS5_NUM];

	static int s_nWords;
	static LANG_WORD s_Words[48];
};