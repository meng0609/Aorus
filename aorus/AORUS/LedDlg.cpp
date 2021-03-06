// VgaLedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AORUS.h"
#include "LedDlg.h"
#include "AORUSDlg.h"

// Timer for LED light effects

//#define TIMER_SEV_SETTING				1
#define TIMER_CIRCLING_ID				2  
#define TIMER_CIRCLING_CLK				100
#define TIMER_BREAHING_ID				3
#define TIMER_BREAHING_CLK				100
#define TIMER_FLASHING_ID				4
#define TIMER_FLASHING_CLK				1000
#define TIMER_DUALFLASHING_ID			5
#define TIMER_DUALFLASHING_CLK			100
#define TIMER_BREATH_MUTILCLR_ID		6
#define TIMER_BREATH_MUTILCLR_CLK		100
#define TIMER_FLASHING_MUTILCLR_ID		7
#define TIMER_FLASHING_MUTILCLR_CLK		1000
#define TIMER_DUALFLASHING_MUTILCLR_ID	8
#define TIMER_DUALFLASHING_MUTILCLR_CLK	100
#define TIMER_LEDMONITORING_ID			9
#define TIMER_LEDMONITORING_CLK			1000


// mask for enable/disable UI control
#define NONE_MASK			0
#define STYLE_MASK		(0x1<<0)
#define VARIATION_MASK	(0x1<<1)
#define SPEED_MASK		(0x1<<2)
#define BRIGHTNESS_MASK (0x1<<3)
#define COLOR_MASK		(0x1<<4)
#define RANDOM_MASK		(0x1<<5)
#define ALL_MASK		 0xffff


IMPLEMENT_DYNAMIC(CLedDlg, CUiDlg)

CLedDlg::CLedDlg(CWnd* pParent /*=NULL*/)
	: CUiDlg(CLedDlg::IDD, pParent)
{
	m_nDevice = DEVICE_VGA; m_bHasProfile = false; m_nShiftTop = 0;
	m_nProfiles = 1;
	m_nProSel = 0;

	m_nCircleClr = 0;
	m_nBreathBright = 10;
	m_bBreathDown = true;
	m_bFlashOff = false;
	m_nDualFlashCnt = 0;
	m_nBreathClrBright = 10;
	m_bBreathClrDown = true;
	m_nBreathClrIdx = 0;
	m_nFlashClrClr = 0;
	m_bFlashClrOff = false;
	m_nDualFlashClrCnt = 0;
	m_nDualFlashClrClr = 0;
}

CLedDlg::~CLedDlg()
{
	//m_Styles.RemoveAll(); m_Monitors.RemoveAll();
}

void CLedDlg::DoDataExchange(CDataExchange* pDX)
{
	CUiDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLedDlg, CUiDlg)
	ON_MESSAGE(WM_UI_BUTTON_CLICKED, OnUiButtonClicked)
	ON_MESSAGE(WM_UI_SLIDER_MOVED, OnUiSliderMoved)
	ON_MESSAGE(WM_UI_SLIDER_MOVING, OnUiSliderMoving)
	ON_MESSAGE(WM_UI_MULITSLIDER_MOVED, OnUiSliderMoved)
	ON_MESSAGE(WM_UI_MULITSLIDER_MOVING, OnUiSliderMoving)
	ON_MESSAGE(WM_UI_COMBOBOX_SELCHANGED, OnUiComboBoxSelChanged)
	ON_MESSAGE(WM_UI_EDIT_CHANGE, OnUiEditChanged)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()

END_MESSAGE_MAP()


// CLedDlg message handlers
BOOL CLedDlg::OnInitDialog()
{
	CUiDlg::OnInitDialog();
	Ui();

	return true;
}

void CLedDlg::Ui()
{
	UiMain();
}

void CLedDlg::UiMain()
{
	SetBkBitmap(m_bHasProfile ? IDB_LED_BG_PROFILE : IDB_LED_BG, 0xff, CLR_TRPT);
	HDC hdc = ::GetDC(m_hWnd);
	SetBkColor(hdc, 0xffff00);

	m_stc[STC_LED_STYLE_COMBINE].CreateStatic(UI_STATIC(this, STC_LED_STYLE_COMBINE,  CRect(11, 83 + m_nShiftTop, 231, 105 + m_nShiftTop), _T(""), NULL, CLR_NONE));
	m_stc[STC_LED_SPEED_COMBINE].CreateStatic(UI_STATIC(this, STC_LED_SPEED_COMBINE,  CRect(9, 120 + m_nShiftTop, 237, 143 + m_nShiftTop), _T(""), NULL, CLR_NONE));
	m_stc[STC_LED_VARIATION_COMBINE].CreateStatic(UI_STATIC(this, STC_LED_VARIATION_COMBINE,  CRect(9, 120 + m_nShiftTop, 237, 143 + m_nShiftTop), _T(""), NULL, CLR_TRPT));
	m_stc[STC_LED_VARIATION_COMBINE].SetBkColor(CLR_BLACK);

	UI_STATIC stc[] =
	{
		UI_STATIC(this, STC_LED_LEDCAPTION,		CRect(39, 20 + m_nShiftTop, 135, 33 + m_nShiftTop), CLangX::Get(LANG_VGALED_LEDCAPTION), NULL, CLR_NONE),
		UI_STATIC(this, STC_LED_LEDEFFECTS,		CRect(12, 52 + m_nShiftTop, 155, 67 + m_nShiftTop),CLangX::Get(LANG_VGALED_LEDEFFECTS), NULL, CLR_NONE),
		UI_STATIC(&m_stc[STC_LED_STYLE_COMBINE], STC_LED_STYLE,		CRect(2, 5, 80, 19), CLangX::Get(LANG_VGALED_STYLE), NULL, CLR_NONE),
		UI_STATIC(&m_stc[STC_LED_SPEED_COMBINE], STC_LED_SPEED,	CRect(3, 4, 85, 19),CLangX::Get(LANG_VGALED_SPEED) , NULL, CLR_NONE),
		UI_STATIC(&m_stc[STC_LED_VARIATION_COMBINE], STC_LED_VARIATION,	CRect(3, 4, 85, 19), CLangX::Get(LANG_VGALED_VARIATION), NULL, CLR_NONE),
		UI_STATIC(this, STC_LED_BRIGHTNESS,		CRect(12, 158 + m_nShiftTop, 93, 173 + m_nShiftTop), CLangX::Get(LANG_VGALED_BRIGHTNESS), NULL, CLR_NONE),
		UI_STATIC(this, STC_LED_SYNCDEVICE,		CRect(31, 292 + m_nShiftTop, 300, 306 + m_nShiftTop), CLangX::Get(LANG_VGALED_SYNCLEDDEV), NULL, CLR_NONE),
		UI_STATIC(this, STC_LED_AORUSLOGO,		CRect(336, 7 + m_nShiftTop, 531, 199 + m_nShiftTop), _T(""), IDB_VGALED_AORUSLOGO, CLR_TRPT),
	};
	for (int i = STC_LED_LEDCAPTION;i <= STC_LED_AORUSLOGO; i++)
	{
		m_stc[i].CreateStatic(stc[i - (int)STC_LED_LEDCAPTION]);
		m_stc[i].ModifyCtrlStyle(ST_VCENTER,ST_LEFT);
		m_stc[i].SetFont(Font(11), CLR_MAIN);
	}

	m_stc[STC_LED_HIDE_LEDCOLR].CreateStatic(UI_STATIC(this, STC_LED_HIDE_LEDCOLR, CRect(10,180 + m_nShiftTop,260,282 + m_nShiftTop), _T(""), NULL, CLR_NONE));
    m_stc[STC_LED_HIDE_LEDCOLR].SetBkColor(CLR_BLACK);
	m_stc[STC_LED_HIDE_LEDCOLR].ShowWindow(SW_HIDE);
	m_stc[STC_LED_SYNCDEVICE].SetFont(Font(11), CLR_GRAY);

    m_stc[STC_LED_AORUSLOGO].SetBkColor(CLR_WHITE);
	UI_BTN btn[] = 
	{
        UI_BTN(this, BTN_LED_EFFECTS,   CRect(173,  52 + m_nShiftTop, 227,  65 + m_nShiftTop), &UIBTN_IMAGE(IDB_VGALED_EFFECTS_OFF, IDB_VGALED_EFFECTS_OFF, IDB_VGALED_EFFECTS_OFF, 0, IDB_VGALED_EFFECTS_ON, IDB_VGALED_EFFECTS_ON, IDB_VGALED_EFFECTS_ON, 0, CLR_TRPT), NULL, BTN_CHECK),
		UI_BTN(this, BTN_LED_COLOR_1,   CRect( 13, 194 + m_nShiftTop,  30, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_2,   CRect( 37, 194 + m_nShiftTop,  55, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_3,   CRect( 62, 194 + m_nShiftTop,  80, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_4,   CRect( 87, 194 + m_nShiftTop, 104, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_5,   CRect(112, 194 + m_nShiftTop, 129, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_6,   CRect(137, 194 + m_nShiftTop, 154, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_7,   CRect(161, 194 + m_nShiftTop, 179, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_8,   CRect(186, 194 + m_nShiftTop, 203, 211 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_9,   CRect(212, 195 + m_nShiftTop, 227, 210 + m_nShiftTop), &UIBTN_IMAGE(0,	0,	0,	0)),
		UI_BTN(this, BTN_LED_COLOR_R,   CRect(236, 194 + m_nShiftTop, 253, 211 + m_nShiftTop), &UIBTN_IMAGE(IDB_LED_BTN_LCRR, IDB_LED_BTN_LCRR, IDB_LED_BTN_LCRR, 0, IDB_LED_BTN_LCRR, IDB_LED_BTN_LCRR, IDB_LED_BTN_LCRR, 0, CLR_TRPT), NULL, BTN_CHECK),
		UI_BTN(this, BTN_LED_SYNCDEVICE,CRect( 13, 295 + m_nShiftTop,  25, 307 + m_nShiftTop), &UIBTN_IMAGE(IDB_CHECKBOX_UNCHECK, IDB_CHECKBOX_UNCHECK, IDB_CHECKBOX_UNCHECK, IDB_CHECKBOX_UNCHECK, IDB_CHECKBOX_CHECKED, IDB_CHECKBOX_CHECKED, IDB_CHECKBOX_CHECKED, 0, CLR_TRPT), NULL, BTN_CHECK),
	};

	for(int i=BTN_LED_EFFECTS; i <= BTN_LED_SYNCDEVICE; i++)
	{
		m_btn[i].CreateButton(btn[i - (int)BTN_LED_EFFECTS]);
		m_btn[i].SetCursor(IDC_CUR_HAND);
	}

//	m_btn[BTN_LED_ADDPROFILE].CreateButton(UI_BTN(this, BTN_LED_ADDPROFILE,  CRect(145, 10, 0, 0), &UIBTN_IMAGE(IDB_VGAPRO_ADDFILE, IDB_VGAPRO_ADDFILE, IDB_VGAPRO_ADDFILE, IDB_VGAPRO_ADDFILEOFF)));
	m_btn[BTN_LED_EDITPROFILE].CreateButton(UI_BTN(this, BTN_LED_EDITPROFILE, CRect(165-10, 10, 0, 0), &UIBTN_IMAGE(IDB_VGAPRO_EDITON, IDB_VGAPRO_EDITON, IDB_VGAPRO_EDITON, IDB_VGAPRO_EDITOFF, IDB_VGAPRO_SAVEFILE, IDB_VGAPRO_SAVEFILE, IDB_VGAPRO_SAVEFILE, IDB_VGAPRO_SAVEFILEOFF, CLR_TRPT), NULL, BTN_CHECK));
//	m_btn[BTN_LED_DELPROFILE].CreateButton(UI_BTN(this, BTN_LED_DELPROFILE,  CRect(185, 10, 0, 0), &UIBTN_IMAGE(IDB_VGAPRO_DELETEFILE, IDB_VGAPRO_DELETEFILE,	IDB_VGAPRO_DELETEFILE,	IDB_VGAPRO_DELETEFILEOFF)));
//	m_btn[BTN_LED_ADDPROFILE].SetCursor(IDC_CUR_HAND);
	m_btn[BTN_LED_EDITPROFILE].SetCursor(IDC_CUR_HAND);
//	m_btn[BTN_LED_DELPROFILE].SetCursor(IDC_CUR_HAND);

	m_btn[BTN_LED_COLOR_9].SetBkColor(CLR_WHITE);
	m_btn[BTN_LED_DEFAULT].CreateButton(UI_BTN(this, BTN_LED_DEFAULT, CRect(425, 223 + m_nShiftTop, 0, 0), &UIBTN_IMAGE(IDB_BTN_ON, IDB_BTN_ON, IDB_BTN_ON,   0,  0, 0, 0, 0, CLR_TRPT),&UIBTN_STRING(CLangX::Get(LANG_VGALED_DEFAULT),	CLR_WHITE, CLR_WHITE,CLR_WHITE,0,Font(11))));
	m_btn[BTN_LED_DEFAULT].SetCursor(IDC_CUR_HAND);

	m_btn[BTN_LED_APPLY].CreateButton(UI_BTN(this, BTN_LED_APPLY, CRect(306, 223 + m_nShiftTop, 0, 0), &UIBTN_IMAGE(IDB_BTN_ON, IDB_BTN_ON, IDB_BTN_ON,   0,  0, 0, 0, 0, CLR_TRPT),&UIBTN_STRING(CLangX::Get(LANG_VGALED_APPLY),	CLR_WHITE, CLR_WHITE,CLR_WHITE,0,Font(11))));
	m_btn[BTN_LED_APPLY].SetCursor(IDC_CUR_HAND);

	// EDIT controls for user to key in User Define color

	m_edit[EDIT_LED_R].Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(28, 259 + m_nShiftTop, 57,  271 + m_nShiftTop), this, EDIT_LED_R);
	m_edit[EDIT_LED_R].SetFont(Font(9), CLR_WHITE);
	m_edit[EDIT_LED_R].SetBkColor(CLR_DARK);
	m_edit[EDIT_LED_R].SetAsIntNumberEdit(3);

	m_edit[EDIT_LED_G].Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(80,  259 + m_nShiftTop, 109,  271 + m_nShiftTop), this, EDIT_LED_G);
	m_edit[EDIT_LED_G].SetFont(Font(9), CLR_WHITE);
	m_edit[EDIT_LED_G].SetBkColor(CLR_DARK);
	m_edit[EDIT_LED_G].SetAsIntNumberEdit(3);

	m_edit[EDIT_LED_B].Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(132,  259 + m_nShiftTop, 161,  271 + m_nShiftTop), this, EDIT_LED_B);
	m_edit[EDIT_LED_B].SetFont(Font(9), CLR_WHITE);
	m_edit[EDIT_LED_B].SetBkColor(CLR_DARK);
	m_edit[EDIT_LED_B].SetAsIntNumberEdit(3);

	m_edit[EDIT_LED_CODE].Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(183,  259 + m_nShiftTop, 226,  271 + m_nShiftTop), this, EDIT_LED_CODE);
	m_edit[EDIT_LED_CODE].SetFont(Font(9), CLR_WHITE);
	m_edit[EDIT_LED_CODE].SetBkColor(CLR_DARK);
	m_edit[EDIT_LED_CODE].SetAsHexNumberEdit(6);

	m_edit[EDIT_LED_PROFILE].Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(5,  12, 135,  24), this, EDIT_LED_PROFILE);
	m_edit[EDIT_LED_PROFILE].SetFont(Font(9), CLR_WHITE);
	m_edit[EDIT_LED_PROFILE].SetBkColor(CLR_DARK);

	m_combobox[COMBOX_LED_PROFILE].Create(CRect(5, 12, 0, 0), this, COMBOX_LED_PROFILE);
	m_combobox[COMBOX_LED_PROFILE].SetComboBoxBitmap(IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, 0, CLR_NONE);
	m_combobox[COMBOX_LED_PROFILE].SetFont(Font(11), CLR_WHITE);
	m_combobox[COMBOX_LED_PROFILE].SetCursor(IDC_CUR_HAND);
	m_combobox[COMBOX_LED_PROFILE].SetArrowWidth(20);
	m_combobox[COMBOX_LED_PROFILE].GetDropList()->SetFont(Font(11),CLR_WHITE);
	m_combobox[COMBOX_LED_PROFILE].GetDropList()->SetBkColor(RGB(0, 0, 0));
	m_combobox[COMBOX_LED_PROFILE].GetDropList()->SetHighlightBkColor(CLR_MAIN);
	m_combobox[COMBOX_LED_PROFILE].ModifyCtrlStyle(ST_RIGHT, ST_LEFT);

	for (int i = 0; i < 5; i++)
	{
		CString sValue;
		//sValue.Format(CLangX::Get(LANG_PROFILE) + _T("%d"), i+1);
		sValue.Format(_T("PROFILE %d"), i+1);
		m_combobox[COMBOX_LED_PROFILE].AddString(sValue, 0, CLR_MAIN);
	}
	m_combobox[COMBOX_LED_PROFILE].SetCurSel(0);

	// LED light effect styles:

	m_combobox[COMBOX_LED_STYLE].Create(CRect(99, 87 + m_nShiftTop, 0, 0), this, COMBOX_LED_STYLE);
	m_combobox[COMBOX_LED_STYLE].SetComboBoxBitmap(IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, 0, CLR_NONE);
	m_combobox[COMBOX_LED_STYLE].SetFont(Font(11), CLR_WHITE);
	m_combobox[COMBOX_LED_STYLE].SetCursor(IDC_CUR_HAND);
	m_combobox[COMBOX_LED_STYLE].SetArrowWidth(20);
	m_combobox[COMBOX_LED_STYLE].GetDropList()->SetFont(Font(11),CLR_WHITE);
	m_combobox[COMBOX_LED_STYLE].GetDropList()->SetBkColor(RGB(0, 0, 0));
	m_combobox[COMBOX_LED_STYLE].GetDropList()->SetHighlightBkColor(CLR_MAIN);
	m_combobox[COMBOX_LED_STYLE].ModifyCtrlStyle(ST_RIGHT,ST_LEFT);

	// Items for monitor style, such as: light effect for CPU temperature monitor

	m_combobox[COMBOX_LED_VARIATION].Create(CRect(90, 5 + m_nShiftTop, 0, 0), &m_stc[STC_LED_VARIATION_COMBINE], COMBOX_LED_VARIATION);
	m_combobox[COMBOX_LED_VARIATION].SetComboBoxBitmap(IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, IDB_VGALED_COMB_NORMAL, 0, CLR_NONE);
	m_combobox[COMBOX_LED_VARIATION].SetFont(Font(11), CLR_WHITE);
	m_combobox[COMBOX_LED_VARIATION].SetCursor(IDC_CUR_HAND);
	m_combobox[COMBOX_LED_VARIATION].SetArrowWidth(20);
	m_combobox[COMBOX_LED_VARIATION].GetDropList()->SetFont(Font(11),CLR_WHITE);
	m_combobox[COMBOX_LED_VARIATION].GetDropList()->SetBkColor(RGB(0, 0, 0));
	m_combobox[COMBOX_LED_VARIATION].GetDropList()->SetHighlightBkColor(CLR_MAIN);
	m_combobox[COMBOX_LED_VARIATION].ModifyCtrlStyle(ST_RIGHT,ST_LEFT);

	// Slider: speed / brightness / color

	m_slider[SLIDER_LED_SPEED].CreateSliderCtrl(UI_SLIDERCTRL(&m_stc[STC_LED_SPEED_COMBINE], SLIDER_LED_SPEED,	CRect(88,  6,  0,  0), IDB_SLIDERBAR_ON, IDB_SLIDERBAR_ON, IDB_SLIDERBAR_OFF, IDB_SLIDERBG_OFF, IDB_SLIDERBG_ON, IDB_SLIDERBG_OFF, CLR_TRPT));
	m_slider[SLIDER_LED_BRGIHTNESS].CreateSliderCtrl(UI_SLIDERCTRL(this, SLIDER_LED_BRGIHTNESS,	CRect(98,  161 + m_nShiftTop,  0,  0), IDB_SLIDERBAR_ON, IDB_SLIDERBAR_ON, IDB_SLIDERBAR_OFF, IDB_SLIDERBG_OFF, IDB_SLIDERBG_ON, IDB_SLIDERBG_OFF, CLR_TRPT));
	m_slider[SLIDER_LED_RGB].CreateSliderCtrl(UI_SLIDERCTRL(this, SLIDER_LED_RGB,	CRect(7, 222 + m_nShiftTop, 233, 245 + m_nShiftTop),	IDB_SLIDER_RGBBAR, IDB_SLIDER_RGBBAR, 0, 0, 0, 0, CLR_TRPT));
	m_Brightness.CreateSliderCtrl(UI_MULITSLIDERCTRL(this, SLIDER_LED_MulitBRGIHTNESS,	CRect(98,  161 + m_nShiftTop,  0,  0), IDB_SLIDERBAR_ON, IDB_SLIDERBAR_ON, IDB_SLIDERBAR_OFF, IDB_SLIDERBG_OFF, IDB_SLIDERBG_ON, IDB_SLIDERBG_OFF, CLR_TRPT));
	m_Brightness.SetRange(0, 100);
	m_Brightness.AddPos(1, 100);
	m_Brightness.ClickAddEnable(false);
	m_Brightness.ShowWindow(SW_HIDE);
	m_slider[SLIDER_LED_RGB].SetRange(0, 359);
	m_slider[SLIDER_LED_SPEED].SetRange(0, 10);
	m_slider[SLIDER_LED_BRGIHTNESS].SetRange(0 ,100);

	if (!m_bHasProfile)
	{
	//	m_btn[BTN_LED_ADDPROFILE].ShowWindow(SW_HIDE);
		m_btn[BTN_LED_EDITPROFILE].ShowWindow(SW_HIDE);
	//	m_btn[BTN_LED_DELPROFILE].ShowWindow(SW_HIDE);
		m_combobox[COMBOX_LED_PROFILE].ShowWindow(SW_HIDE);
		m_edit[EDIT_LED_PROFILE].ShowWindow(SW_HIDE);
	}
	else 
	{
		m_btn[BTN_LED_EDITPROFILE].SetCheck(false);
		m_edit[EDIT_LED_PROFILE].ShowWindow(SW_HIDE);
	}
}


int CLedDlg::GetCardSel() 
{ 
	return (DEVICE_VGA == m_nDevice) ? Aorus()->GetVGASel() : 0;
}

void CLedDlg::SyncAorusLedBtnClr(COLORREF clr)
{
	if(Aorus()->GetDeviceSel() == m_nDevice) Aorus()->SyncLedBtnClr(clr); 
}

void CLedDlg::SyncLedSetting(LED_SETTING& led, bool bSave)
{
	ShowLoadingWindow(AfxGetMainWnd(), SW_SHOW);			
	CVGA* pVGA = (CVGA*)Aorus()->GetDevice(DEVICE_VGA);
	int nGPUs = pVGA->Ctrl()->GetGpuCount();

	// Update profile and save to ini

	for (int i = 0; i < DEVICE_COUNT; i++)
	{
		CDevice* pDevice = Aorus()->GetDevice(s_DevIds[i]);
		if(!pDevice->IsConnected())
			continue;

		int nDevices = 1;
		if (pDevice->GetDeviceId() == DEVICE_VGA) nDevices = nGPUs;
		for (int m = 0; m < nDevices; m++)
		{
			int nProSel = 0;
			CArray<LED_SETTING> profiles;
			Aorus()->GetCfg()->GetLedProfiles(pDevice->GetDeviceId(), profiles, nProSel, m);
			// always save
			CArray<LED_SETTING> prosNew;
			for (int k = 0; k < (int)profiles.GetSize(); k++)
			{
				if (k == nProSel)
				{
					prosNew.Add(led); continue;
				}
 				LED_SETTING ledTmp = profiles.GetAt(k);
				prosNew.Add(ledTmp);
			}
			
			Aorus()->GetCfg()->SetLedProfiles(pDevice->GetDeviceId(), prosNew, nProSel, m);
			// pDevice->SetLedSetting(m, nProSel, led, bSave); 
		}
	}

	Aorus()->ReInitAllDevicesLedState();   // refresh all devices' led dialog

	// Notify all devices to sync led.

	SYNCLED_CONFIG cfg;
	cfg.nType = -1;
	cfg.nBright = led.nRangeMax;
	cfg.nSpeed = led.nSpeed;
	for (int i = 0; i < 7; i++) 
	{
		cfg.clrArray[i] = s_clr[i];
	}
	
	switch(led.dwStyle)
	{
	case LED_STYLE_CONSISTENT:	
		cfg.nType = GVORDER_LED_CONSISTENT; 
		cfg.nClrCount = 1;
		cfg.clrArray[0] = led.clrLed;
		break;
	case LED_STYLE_BREATHING:	
		cfg.nType = led.bMutilColor ? GVORDER_LED_CYCLING : GVORDER_LED_BREATHING; 
		cfg.nClrCount = led.bMutilColor ? 7 : 1;
		if (!led.bMutilColor) cfg.clrArray[0] = led.clrLed;
		break;
	case LED_STYLE_FLASHING:	
		cfg.nType = GVORDER_LED_SINGLETWINKLE; 
		cfg.nClrCount = led.bMutilColor ? 7 : 1;
		if (!led.bMutilColor) cfg.clrArray[0] = led.clrLed;
		break;
	case LED_STYLE_DUALFLASHING:
		cfg.nType = GVORDER_LED_TWICETWINKLE; 
		cfg.nClrCount = led.bMutilColor ? 7 : 1;
		if (!led.bMutilColor) cfg.clrArray[0] = led.clrLed;
		break;
	case LED_STYLE_CIRCLING:	
		cfg.nType = GVORDER_LED_CYCLING; 
		cfg.nClrCount = 7;
		break;
	default: 
		cfg.nType = GVORDER_LED_CONSISTENT; 
		cfg.nClrCount = 1;
		cfg.clrArray[0] = led.clrLed;
		break;  // others: unsupported
	}

	for (int i = cfg.nClrCount; i < CLR_MAX_COUNT; i++)
	{
		cfg.clrArray[i] = RGB(0, 0, 0);
	}
	if (!led.bOn) { cfg.nType = GVORDER_LED_CONSISTENT; cfg.nBright = 0;}
	s_SyncLedConfig = cfg;
	CSyncLed::Set(s_SyncLedConfig, true);
	
	ShowLoadingWindow(AfxGetMainWnd(), SW_HIDE);
	Aorus()->PostMessage(WM_SYN_MOBILESTATE, 0, 0);
}

void CLedDlg::ApplyLedSetting(int nCard, LED_SETTING& led, bool bSyncLed, bool bSave /*false: store in memory*/)
{
	if (bSyncLed)
	{
		return SyncLedSetting(led, true);
	}

	if (IsSyncLed())
	{
		return; // Control by GvLedService.exe now. DO NOTHING.
	}

	CDevice* pCurDevice = NULL;
	for(int i = 0; i < DEVICE_COUNT; i++)
	{
		CDevice* pDevice = Aorus()->GetDevice(s_DevIds[i]);
		if (pDevice->GetDeviceId() != m_nDevice)
			continue;
		pCurDevice = pDevice; break;
	}
	ASSERT(NULL != pCurDevice);
	return pCurDevice->SetLedSetting(nCard, m_nProSel, led, bSave);   

	Aorus()->PostMessage(WM_SYN_MOBILESTATE, 0, 0);
}

bool CLedDlg::IsSyncLed()
{
	VGA_PARAM_BASESETTING base;
	Aorus()->GetCfg()->GetBaseSetting(base);
	return base.bSyncLed ? true : false;
}

// When sync led effects in different devices, 
// only some styles are allowed to use. Make sure it.
void CLedDlg::TranToSyncStyle(LED_SETTING& led)
{
	//led.bMutilColor = false;  
	//led.dwStyle = LED_STYLE_CONSISTENT; 
}

void CLedDlg::SetSyncLed(bool bSyncLed)
{
	VGA_PARAM_BASESETTING base;
	Aorus()->GetCfg()->GetBaseSetting(base);
	base.bSyncLed = bSyncLed;
	Aorus()->GetCfg()->SetBaseSetting(base);
}

// Support LED sync or not.
bool CLedDlg::IsSyncLedSupported()
{
// 	CVGA* pVGA = (CVGA*)Aorus()->GetDevice(DEVICE_VGA);
// 	if (!pVGA->IsSupportLed())
// 		return false;

	int nLedDeivces = 0;
	for (int i = 0; i < DEVICE_COUNT; i++)
	{
		CDevice* pDevice = Aorus()->GetDevice(s_DevIds[i]);
		if (!pDevice->IsConnected())
			continue;
		if (pDevice->IsSupportLedSync()) nLedDeivces++;
	}

	return (nLedDeivces >= 2) ? true : false;
}

void CLedDlg::Initialize(int nDevice, bool bHasProfile) 
{ 
	m_nDevice = nDevice; 
	m_bHasProfile = bHasProfile; 
	m_nShiftTop = bHasProfile ? 33 : 0;  // size diff of IDB_LED_BG_PROFILE & IDB_LED_BG, pixels
}

void CLedDlg::InitState()
{

	// check sync suupported

 	bool bSupported = IsSyncLedSupported();
	m_btn[BTN_LED_SYNCDEVICE].ShowWindow(bSupported ? SW_SHOW : SW_HIDE);
	m_stc[STC_LED_SYNCDEVICE].ShowWindow(bSupported ? SW_SHOW : SW_HIDE);

	bool bSyncLed = IsSyncLed();
	if (bSyncLed && !bSupported) 
	{
		bSyncLed = false; //SetSyncLed(bSyncLed); // devices change: set it back.
	}

	m_btn[BTN_LED_SYNCDEVICE].SetCheck(bSyncLed ? true : false);

	// Check styles supported.
	CDevice* pDevice = Aorus()->GetDevice(m_nDevice);
	pDevice->GetLedMonitorItems(m_Monitors);
	pDevice->GetLedStylesSupported(m_Styles, bSyncLed);



	// Get led setting profiles

	int nDevices = 1; 
	if (DEVICE_VGA == m_nDevice)
	{
		CVGA* pVGA = (CVGA*)Aorus()->GetDevice(DEVICE_VGA);
		nDevices = pVGA->Ctrl()->GetGpuCount();
	}

	LED_SETTING ledCur;
	for (int i = 0; i < nDevices; i++)
	{
		int nProSel = 0;
		CArray<LED_SETTING> profiles;
		Aorus()->GetCfg()->GetLedProfiles(m_nDevice, profiles, nProSel, i);
		if ((int)profiles.GetSize() <= 0)
		{
			m_nProSel = 0;
			LED_SETTING ledNew;
			ledNew.sProfileName = "Profile1";
			profiles.Add(ledNew);
		} 
		for(int k = 0; k < (int)profiles.GetSize(); k++)
		{
			LED_SETTING ledP = profiles.GetAt(k);

			// double check
			int nStyleIdx = 0;
			for (int mm = 0; mm < (int)m_Styles.GetSize(); mm++)
			{
				if (m_Styles.GetAt(mm).nId == (int)ledP.dwStyle) { nStyleIdx = mm; break; }
			}
			ledP.dwStyle = (DWORD)m_Styles.GetAt(nStyleIdx).nId;

			m_ledProfiles[i][k] = ledP;
		}

		LED_SETTING led = m_ledProfiles[i][nProSel];
		ApplyLedSetting(i, led, false, false);   // only apply itself

	if (GetCardSel() == i)
		{
			m_nProSel = nProSel; m_nProfiles = (int)profiles.GetSize(); ledCur = m_ledProfiles[i][nProSel];
		}
	}

// 	if (m_nProfiles <= 1)
// 	{
// 		m_btn[BTN_LED_ADDPROFILE].Enable(true);
// 		m_btn[BTN_LED_DELPROFILE].Enable(false);
// 	}
// 	else 
// 	{
// 		m_btn[BTN_LED_ADDPROFILE].Enable(false);
// 		m_btn[BTN_LED_DELPROFILE].Enable(true);
// 	}

	m_combobox[COMBOX_LED_PROFILE].ResetContent();
	for(int i = 0; i < m_nProfiles; i++)
	{
		m_combobox[COMBOX_LED_PROFILE].AddString(m_ledProfiles[GetCardSel()][i].sProfileName, 0, CLR_WHITE);
	}

//	m_combobox[COMBOX_LED_PROFILE].SetCurSel(m_nProSel);

	OnColor(ledCur.clrLed);
	InitializeUI();

	bSyncLed ? CSyncLed::Begin() : CSyncLed::End(); // make sure GvLedService.exe open/close
 }

void CLedDlg::OnDefault()
{
	ShowLoadingWindow(AfxGetMainWnd(), SW_SHOW);

	CArray<LED_SETTING> profiles;
	for (int i = 0; i < m_nProfiles; i++)
	{
		profiles.Add(m_ledProfiles[GetCardSel()][i]);
	}

	LED_SETTING led; // default
	led.sProfileName = m_ledProfiles[GetCardSel()][m_nProSel].sProfileName;
	bool bSyncLed = m_btn[BTN_LED_SYNCDEVICE].IsCheck();
	if (!bSyncLed)
	{
		if (m_nDevice == DEVICE_VGA) led.bMutilColor = true;
	}
	led.dwStyle = LED_STYLE_CIRCLING;// Terry 2017/07/05
	m_ledProfiles[GetCardSel()][m_nProSel] = led;

	InitializeUI();
	UpdateWindow();
	ApplyLedSetting(GetCardSel(), led, bSyncLed, false);
	
	ShowLoadingWindow(AfxGetMainWnd(), SW_HIDE);
 }

void CLedDlg::OnApply()
{      
	ShowLoadingWindow(AfxGetMainWnd(), SW_SHOW);

	Sleep(200);
	CArray<LED_SETTING> profiles;
	for (int i = 0; i < m_nProfiles; i++)
	{
		Sleep(200);
		profiles.Add(m_ledProfiles[GetCardSel()][i]);
	}
	Aorus()->GetCfg()->SetLedProfiles(m_nDevice, profiles, m_nProSel, GetCardSel());
	
	LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
	bool bSyncLed = IsSyncLed();
	if (bSyncLed)
	{
		TranToSyncStyle(led);
		m_ledProfiles[GetCardSel()][m_nProSel] = led;
	}

	InitializeUI();
	UpdateWindow();
	ApplyLedSetting(GetCardSel(), led, bSyncLed, true);
	ShowLoadingWindow(AfxGetMainWnd(), SW_HIDE);
}

// Demo different led light effects in UI
void CLedDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
	case TIMER_CIRCLING_ID: {	// show circling mode 
			m_nCircleClr = (m_nCircleClr >= 360) ? 0 : m_nCircleClr;
			COLORREF clr = HSBToRGB((float)m_nCircleClr);
			double r = GetRValue(clr);
			double g = GetGValue(clr);
			double b = GetBValue(clr);
			if (r == 255 && g == 0 && b == 255) { b -= 1; clr = RGB(r, g, b); }
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			SyncAorusLedBtnClr(clr);
			m_nCircleClr++;
			break;
		} break;

	case TIMER_BREAHING_ID: {	// show breahing mode 
			LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
			COLORREF clr = led.clrLed;
			double r = GetRValue(clr) * 0.1 * m_nBreathBright;
			double g = GetGValue(clr) * 0.1 * m_nBreathBright;
			double b = GetBValue(clr) * 0.1 * m_nBreathBright;
			if (r == 255 && g == 0 && b == 255) { r -= 1; }
			clr = RGB(r, g, b);
			if (m_nBreathBright == 10) m_bBreathDown = true;
			if (m_nBreathBright ==  0) m_bBreathDown = false;
			m_bBreathDown ? m_nBreathBright-- : m_nBreathBright++;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			SyncAorusLedBtnClr(clr);
		} break;

	case TIMER_FLASHING_ID: {	// show flashing mode 
			LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
			COLORREF clr = m_bFlashOff ? led.clrLed : CLR_DARK;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			m_bFlashOff = !m_bFlashOff;
			SyncAorusLedBtnClr(clr);
		} break;

	case TIMER_DUALFLASHING_ID: {	// show dual flashing mode 
			LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
			COLORREF clr = ((m_nDualFlashCnt < 3)|| (m_nDualFlashCnt > 4 && m_nDualFlashCnt < 8)) ? led.clrLed : CLR_DARK;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			SyncAorusLedBtnClr(clr);
			m_nDualFlashCnt = (m_nDualFlashCnt >= 12) ? 0 : m_nDualFlashCnt;
			m_nDualFlashCnt++;
		} break;

	case TIMER_BREATH_MUTILCLR_ID: {	// show breathing(mutil-color) mode 
			double r = GetRValue(s_clr[m_nBreathClrIdx]) * 0.1 * m_nBreathClrBright;
			double g = GetGValue(s_clr[m_nBreathClrIdx]) * 0.1 * m_nBreathClrBright;
			double b = GetBValue(s_clr[m_nBreathClrIdx]) * 0.1 * m_nBreathClrBright;
			if (r == 255 && g==0 && b == 255) { r -= 1; }
			COLORREF clr = RGB(r, g, b);
			if (m_nBreathClrBright == 10) { m_bBreathClrDown = true; }
			if (m_nBreathClrBright ==  0) { m_bBreathClrDown = false; m_nBreathClrIdx++; m_nBreathClrIdx = (m_nBreathClrIdx > 7) ? 0 : m_nBreathClrIdx; }
			m_bBreathClrDown ? m_nBreathClrBright-- : m_nBreathClrBright++;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			SyncAorusLedBtnClr(clr);
		} break;

	case TIMER_FLASHING_MUTILCLR_ID: {	// show flashing(mutil-color) mode 
			m_nFlashClrClr = (m_nFlashClrClr >= 360) ? 0 : m_nFlashClrClr;
			COLORREF clr = HSBToRGB((float)m_nFlashClrClr);
			double r = GetRValue(clr);
			double g = GetGValue(clr);
			double b = GetBValue(clr);
			if (r == 255 && g == 0 && b == 255) { b -= 1; clr = RGB(r, g, b);}
			clr = m_bFlashClrOff ? clr : CLR_DARK;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			SyncAorusLedBtnClr(clr);
			m_bFlashClrOff = !m_bFlashClrOff;
			m_nFlashClrClr = m_nFlashClrClr + 50;
		} break;

	case TIMER_DUALFLASHING_MUTILCLR_ID: {	// show dual flashing(mutil-color) mode 
			m_nDualFlashClrClr = (m_nDualFlashClrClr >= 360) ? 0 : m_nDualFlashClrClr;
			COLORREF clr = HSBToRGB((float)m_nDualFlashClrClr);
			double r = GetRValue(clr);
			double g = GetGValue(clr);
			double b = GetBValue(clr);
			if (r == 255 && g == 0 && b == 255) { b -= 1; clr=RGB(r, g, b); }
			clr = ((m_nDualFlashClrCnt < 3)||(m_nDualFlashClrCnt > 4 && m_nDualFlashClrCnt < 8)) ? clr : CLR_DARK;
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			m_stc[STC_LED_AORUSLOGO].UpdateWindow();
			SyncAorusLedBtnClr(clr);
			m_nDualFlashClrCnt = (m_nDualFlashClrCnt >= 10) ? 0 : m_nDualFlashClrCnt;
			m_nDualFlashClrCnt++;
			m_nDualFlashClrClr++;
		} break;

	case TIMER_LEDMONITORING_ID: {	// show monitor mode 
			LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
			VGA_PERFORMANCE perf;
			COLORREF clr = RGB(255, 102, 0);
			if (led.dwStyle != LED_STYLE_MONITORING) break; // Monitor
			switch(led.dwVariation)
			{
			case LED_MONITOR_GPU_TEMPERATURE:
				Aorus()->GetVGA()->Ctrl()->GetCurrentPerformances(VGA_PERF_TEMPER, Aorus()->GetVGASel(), perf);
				if(perf.nTemperature <= 40) clr = RGB(0, 255, 0);
				else if(perf.nTemperature <= 70) clr = RGB(255, 255, 0);
				break;
			case LED_MONITOR_GPU_USAGE:
				Aorus()->GetVGA()->Ctrl()->GetCurrentPerformances(VGA_PERF_GPUSAGE,Aorus()->GetVGASel(),perf);
				if(perf.nGPUsage <= 40) clr = RGB(0, 255, 0);
				else if(perf.nGPUsage <= 70) clr = RGB(255, 255, 0);
				break;
			case LED_MONITOR_GPU_FANSPEED:
				Aorus()->GetVGA()->Ctrl()->GetCurrentPerformances(VGA_PERF_FANSPD, Aorus()->GetVGASel(), perf);
				if(perf.nFanSpd <= 35) clr = RGB(0, 255, 0);
				else if(perf.nFanSpd <= 50) clr = RGB(255, 255, 0);
				break;
			}
			m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
			SyncAorusLedBtnClr(clr);
		} break;
	default: break;
    }
}


LRESULT CLedDlg::OnUiButtonClicked(WPARAM wParam, LPARAM lParam)
{
	LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];

	switch(wParam)
	{
// 	case BTN_LED_ADDPROFILE: {
// 		if (m_nProfiles >= LED_PROFILE_CONT) 
// 			return 1;
// 		m_nProSel = m_nProfiles;
// 		m_nProfiles++;
// 		LED_SETTING ledNew;
// 		ledNew.sProfileName.Format(_T("Profile%d"), m_nProfiles);
// 		m_ledProfiles[GetCardSel()][m_nProSel] = ledNew;
// 		m_combobox[COMBOX_LED_PROFILE].ResetContent();
// 		for (int i = 0; i < m_nProfiles; i++)
// 		{
// 			m_combobox[COMBOX_LED_PROFILE].AddString(m_ledProfiles[GetCardSel()][i].sProfileName, 0, CLR_WHITE);
// 		}
// 		m_combobox[COMBOX_LED_PROFILE].SetCurSel(m_nProSel);
// 		LED_SETTING ledCur = m_ledProfiles[GetCardSel()][m_nProSel];
// 		OnColor(ledCur.clrLed);
// 		InitializeUI();
// 		m_btn[BTN_LED_DELPROFILE].Enable(TRUE);
// 		if (m_nProfiles >= LED_PROFILE_CONT)
// 		{
// 			m_btn[BTN_LED_ADDPROFILE].Enable(FALSE);
// 		}
// 		ApplyLedSetting(GetCardSel(), ledCur, false, false);  // when add profile, don't sync led.
// 		return 1;
// 		}
	case BTN_LED_EDITPROFILE: {
		bool bCheck = m_btn[BTN_LED_EDITPROFILE].IsCheck();
// 		m_btn[BTN_LED_ADDPROFILE].Enable(!bCheck);
// 		m_btn[BTN_LED_DELPROFILE].Enable(!bCheck);
		m_edit[EDIT_LED_PROFILE].ShowWindow(bCheck ? SW_SHOW : SW_HIDE);
		m_combobox[COMBOX_LED_PROFILE].ShowWindow(bCheck ? SW_HIDE : SW_SHOW);
		if (bCheck)
		{
			m_edit[EDIT_LED_PROFILE].SetWindowText(m_ledProfiles[GetCardSel()][m_nProSel].sProfileName);
		}
		else // ready to save profile name
		{
			CString sName = _T("");
			m_edit[EDIT_LED_PROFILE].GetWindowText(sName);
			sName.Trim();
			if (_T("") == sName)
			{
				sName = m_ledProfiles[GetCardSel()][m_nProSel].sProfileName; // set it back
				m_edit[EDIT_LED_PROFILE].SetWindowText(sName);
			}
			m_ledProfiles[GetCardSel()][m_nProSel].sProfileName = sName;
			m_combobox[COMBOX_LED_PROFILE].ResetContent();
			for (int i = 0; i < m_nProfiles; i++)
			{
				m_combobox[COMBOX_LED_PROFILE].AddString(m_ledProfiles[GetCardSel()][i].sProfileName, 0, CLR_WHITE);
			}
			m_combobox[COMBOX_LED_PROFILE].SetCurSel(m_nProSel);


			CArray<LED_SETTING> profiles;
			for (int i = 0; i < LED_PROFILE_CONT; i++)
			{
				profiles.Add(m_ledProfiles[GetCardSel()][i]);
			}

			Aorus()->GetCfg()->SetLedProfiles(m_nDevice, profiles, m_nProSel, GetCardSel());
		}
		return 1;
		}
// 	case BTN_LED_DELPROFILE: {
// 		if (m_nProfiles <= 1) 
// 			return 1;
// 		// delete profile
// 		CArray<LED_SETTING> profiles;
// 		for (int i = 0; i < m_nProfiles; i++)
// 		{
// 			if (i == m_nProSel) continue;
// 			profiles.Add(m_ledProfiles[GetCardSel()][i]);
// 		}
// 		m_nProfiles--;
// 		m_nProSel = (m_nProSel - 1 < 0) ? 0 : (m_nProSel - 1);
// 		for (int i = 0; i < (int)profiles.GetSize(); i++)
// 		{
// 			m_ledProfiles[GetCardSel()][i] = profiles.GetAt(i);
// 		}
// 		// reflash Ui
// 		m_combobox[COMBOX_LED_PROFILE].ResetContent();
// 		for (int i = 0; i < m_nProfiles; i++)
// 		{
// 			m_combobox[COMBOX_LED_PROFILE].AddString(m_ledProfiles[GetCardSel()][i].sProfileName, 0, CLR_WHITE);
// 		}
// 		m_combobox[COMBOX_LED_PROFILE].SetCurSel(m_nProSel);
// 		LED_SETTING ledCur = m_ledProfiles[GetCardSel()][m_nProSel];
// 		OnColor(ledCur.clrLed);
// 		InitializeUI();
// 		m_btn[BTN_LED_ADDPROFILE].Enable(TRUE);
// 		if (m_nProfiles <= 1)
// 		{
// 			m_btn[BTN_LED_DELPROFILE].Enable(FALSE);
// 		}
// 		ApplyLedSetting(GetCardSel(), ledCur, IsSyncLed(), false);
// 		return 1;
// 		}
	 case BTN_LED_SYNCDEVICE: {
		 if (m_btn[BTN_LED_SYNCDEVICE].IsCheck())
		 {
			 AfxMessageBox(_T("注意：啟用燈光同步控制功能，鍵盤的波浪、反應、漣漪效果將會被暫時禁用。"));
		 }
			SetSyncLed(m_btn[BTN_LED_SYNCDEVICE].IsCheck());
			m_btn[BTN_LED_SYNCDEVICE].UpdateWindow();
			bool bSyncLed = IsSyncLed();		
			ApplyLedSetting(GetCardSel(), led, bSyncLed, true);
			if (!bSyncLed) 
			{
				ShowLoadingWindow(AfxGetMainWnd(), SW_SHOW);
				Aorus()->ReInitAllDevicesLedState(); 
				ShowLoadingWindow(AfxGetMainWnd(), SW_HIDE);
			}
		} return 1;
	 case BTN_LED_DEFAULT: OnDefault(); return 1;  // return directly 
	 case BTN_LED_APPLY: OnApply(); return 1; // return directly
	case BTN_LED_EFFECTS: led.bOn = m_btn[BTN_LED_EFFECTS].IsCheck(); break; 
	case BTN_LED_COLOR_1: case BTN_LED_COLOR_2: 
	case BTN_LED_COLOR_3: case BTN_LED_COLOR_4: 
	case BTN_LED_COLOR_5: case BTN_LED_COLOR_6:
	case BTN_LED_COLOR_7: case BTN_LED_COLOR_8: 
	case BTN_LED_COLOR_9: {
			COLORREF color = s_clr[wParam - BTN_LED_COLOR_1];
			m_btn[BTN_LED_COLOR_R].SetCheck(false); // Off Mutil-Color.
			OnColor(color);
			led.clrLed = color;
			led.bMutilColor = false; 
		} break;
	 case BTN_LED_COLOR_R: {
			led.bMutilColor = true;  // On Mutil-Color.
		} break;
	 }

	bool bSyncLed = IsSyncLed();
	if (bSyncLed)
	{
		TranToSyncStyle(led);
	}
	m_ledProfiles[GetCardSel()][m_nProSel] = led;
	UpdateUI(led);
	UpdateWindow();
	ApplyLedSetting(GetCardSel(), led, bSyncLed, false);
	return 1;
}

LRESULT CLedDlg::OnUiSliderMoved(WPARAM wParam, LPARAM lParam)
 {
	LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];

	int nCurValue = (int)lParam;
	switch(wParam)
	{
	case SLIDER_LED_RGB: { // Change color
			led.bMutilColor = false;
			led.clrLed = HSBToRGB((float)nCurValue);
			OnColor(led.clrLed);
		} break;
	case SLIDER_LED_BRGIHTNESS:  {
			led.nRangeMin = 0;
			led.nRangeMax = nCurValue;
			m_Brightness.SetPos(1, nCurValue > 6 ? nCurValue : 6);
		} break;
	case SLIDER_LED_MulitBRGIHTNESS: {
			led.nRangeMax = m_Brightness.GetPos(1);
			led.nRangeMin = m_Brightness.GetPos(0);
			m_slider[SLIDER_LED_BRGIHTNESS].SetPos(led.nRangeMax);
		} break;
	case SLIDER_LED_SPEED: led.nSpeed = nCurValue; break;
	}

	bool bSyncLed = IsSyncLed();
	if (bSyncLed)
	{
		TranToSyncStyle(led);
	}

	m_ledProfiles[GetCardSel()][m_nProSel] = led;

	UpdateUI(led);
	UpdateWindow();
	ApplyLedSetting(GetCardSel(), led, bSyncLed, false);
	return 1;
 }

LRESULT CLedDlg::OnUiSliderMoving(WPARAM wParam, LPARAM lParam)
{
	int nCurValue = (int)lParam;
	switch (wParam)
	{	
		case SLIDER_LED_RGB: OnColor(HSBToRGB((float)nCurValue)); break;
	}
	return 1;
}

LRESULT CLedDlg::OnUiEditChanged(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case EDIT_LED_R: case EDIT_LED_G: case EDIT_LED_B: {
			CString sR, sG, sB;
			m_edit[wParam].GetWindowText(sR);
			if(_ttoi(sR) < 0) sR.Format(_T("%03d"), 0);
			if(_ttoi(sR) > 255) sR.Format(_T("%03d"), 255);
			m_edit[wParam].SetWindowText(sR);
			m_edit[EDIT_LED_R].GetWindowText(sR);
			m_edit[EDIT_LED_G].GetWindowText(sG);
			m_edit[EDIT_LED_B].GetWindowText(sB);
		} break;
	case EDIT_LED_CODE: { // User key in color directly
			CString sCode = _T("");
			m_edit[wParam].GetWindowText(sCode);
			if(sCode.GetLength() < 6) break;
		} break;
	}
	return 1;
}

LRESULT CLedDlg::OnUiComboBoxSelChanged(WPARAM wParam, LPARAM lParam)
{
	int nCurSel = (int)lParam; 
	LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];
	CDevice* pDevice = Aorus()->GetDevice(m_nDevice);
	bool bSyncLed = IsSyncLed();
	switch(wParam)
	{
	case COMBOX_LED_VARIATION: 
		led.clrLed = RGB(0, 255, 0);  // Initialize the color. see OnTimer() funtion
		led.dwVariation = m_Monitors.GetAt(nCurSel).nId; //pDevice->LedIndex2Monitor(nCurSel); 
		m_ledProfiles[GetCardSel()][m_nProSel] = led;
		UpdateUI(led);
		UpdateWindow();
		ApplyLedSetting(GetCardSel(), led, bSyncLed, false);
		break;
	case COMBOX_LED_STYLE:
		led.dwStyle = m_Styles.GetAt(nCurSel).nId; //pDevice->LedIndex2Style(nCurSel); 
		m_ledProfiles[GetCardSel()][m_nProSel] = led;
		UpdateUI(led);
		UpdateWindow();
		ApplyLedSetting(GetCardSel(), led, bSyncLed, false);
		break;
	case COMBOX_LED_PROFILE:
		m_nProSel = nCurSel;
		led = m_ledProfiles[GetCardSel()][m_nProSel];
		InitializeUI();
		UpdateWindow();
		ApplyLedSetting(GetCardSel(), led, bSyncLed, false);

		CAORUSDlg* pParent = (CAORUSDlg*)AfxGetMainWnd();
		pParent->SwitchM3ButtonProfile(m_nProSel);
	}

	return 1;
}

// Initialize UI: controls/Styles/Monitor items/Slider range etc...
void CLedDlg::InitializeUI()
{
	LED_SETTING led = m_ledProfiles[GetCardSel()][m_nProSel];

	bool bSyncLed = IsSyncLed();

	// Check styles supported.
	CDevice* pDevice = Aorus()->GetDevice(m_nDevice);
	pDevice->GetLedMonitorItems(m_Monitors);
	pDevice->GetLedStylesSupported(m_Styles, bSyncLed);
	if (bSyncLed)
	{
		TranToSyncStyle(led);
		m_ledProfiles[GetCardSel()][m_nProSel] = led;
	}

	// reset styles & monitor items

	m_combobox[COMBOX_LED_STYLE].ResetContent();
	m_combobox[COMBOX_LED_VARIATION].ResetContent();
	for (int i = 0; i < m_Styles.GetSize(); i++)
	{
		m_combobox[COMBOX_LED_STYLE].AddString(m_Styles[i].sName, 0, CLR_WHITE);
	}
	for (int i = 0; i < m_Monitors.GetSize(); i++)
	{
		m_combobox[COMBOX_LED_VARIATION].AddString(m_Monitors[i].sName, 0, CLR_WHITE);
	}

	int nStyleIdx = 0;
	for (int mm = 0; mm < (int)m_Styles.GetSize(); mm++)
	{
		if (m_Styles.GetAt(mm).nId == (int)led.dwStyle) { nStyleIdx = mm; break; }
	}

	int nMonitorIdx = 0;
	for (int mm = 0; mm < (int)m_Monitors.GetSize(); mm++)
	{
		if (m_Monitors.GetAt(mm).nId == (int)led.dwVariation) { nMonitorIdx = mm; break; }
	}

	m_combobox[COMBOX_LED_STYLE].SetCurSel(nStyleIdx);
	m_combobox[COMBOX_LED_VARIATION].SetCurSel(nMonitorIdx);

	m_btn[BTN_LED_EFFECTS].SetCheck(led.bOn);
	m_slider[SLIDER_LED_RGB].SetPos((int)RGBtoHSB(led.clrLed));
	m_slider[SLIDER_LED_SPEED].SetPos(led.nSpeed);
	m_slider[SLIDER_LED_BRGIHTNESS].SetPos(led.nRangeMax);
	m_Brightness.SetPos(0, led.nRangeMin);
	m_Brightness.SetPos(1, led.nRangeMax);
	
	UpdateUI(led);
}

void CLedDlg::UpdateUI(LED_SETTING led)
{
	if (!led.bOn)
	{
		SetLogoLedTimer(0 , 0);
		EnableControls(NONE_MASK, false, false);
		m_stc[STC_LED_AORUSLOGO].SetBkColor(CLR_WHITE); 
		return;
	}

	bool bMBExisted = false;
	if (Aorus()->GetDevice(DEVICE_MB)->IsConnected() || Aorus()->GetDevice(DEVICE_MB)->OnConnect())
	{
		bMBExisted = true;
	}
	// switch style, set timer to demo effect.

	switch (led.dwStyle)
	{
	case LED_STYLE_CONSISTENT: // led.bMutilColor = true: impossible
		//if (led.bMutilColor)
		//{
		//	SetLogoLedTimer(TIMER_CIRCLING_ID, TIMER_CIRCLING_CLK);
		//	EnableControls(STYLE_MASK | BRIGHTNESS_MASK | COLOR_MASK, false, false);
		//}
		//else
		//{
			SetLogoLedTimer(0, 0);			
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | COLOR_MASK, false , false);
			OnColor(led.clrLed);
		//}
		break;
	case LED_STYLE_BREATHING:
		if(led.bMutilColor)
		{
			SetLogoLedTimer(TIMER_BREATH_MUTILCLR_ID, TIMER_BREATH_MUTILCLR_CLK);
		}
		else
		{
			SetLogoLedTimer(TIMER_BREAHING_ID, TIMER_BREAHING_CLK);
		}		
		if (m_nDevice == DEVICE_MB)		// Peter Lee 2017/6/2
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK, false);
		else
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK | RANDOM_MASK, false);
		break;
	case LED_STYLE_FLASHING:
		if(led.bMutilColor)
		{
			SetLogoLedTimer(TIMER_FLASHING_MUTILCLR_ID, TIMER_FLASHING_MUTILCLR_CLK);
		}
		else
		{
			SetLogoLedTimer(TIMER_FLASHING_ID,TIMER_FLASHING_CLK);
		}
		if (m_nDevice == DEVICE_MB)		// Peter Lee 2017/6/2
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK, false);
		else
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK | RANDOM_MASK, false);
		break;
	case LED_STYLE_DUALFLASHING:
		if(led.bMutilColor)
		{
			SetLogoLedTimer(TIMER_DUALFLASHING_MUTILCLR_ID, TIMER_DUALFLASHING_MUTILCLR_CLK);
		}
		else
		{
			SetLogoLedTimer(TIMER_DUALFLASHING_ID, TIMER_DUALFLASHING_CLK);
		}
		if (m_nDevice == DEVICE_MB)		// Peter Lee 2017/6/2
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK, false);
		else
			EnableControls(STYLE_MASK | BRIGHTNESS_MASK | SPEED_MASK | COLOR_MASK | RANDOM_MASK, false);
		break;
	case LED_STYLE_MONITORING:
		SetLogoLedTimer(TIMER_LEDMONITORING_ID, TIMER_LEDMONITORING_CLK);
		EnableControls(STYLE_MASK | VARIATION_MASK | BRIGHTNESS_MASK, true, false);
		OnColor(led.clrLed);
		break;
	case LED_STYLE_AUDIOFLASHING:
		SetLogoLedTimer(0, 0);
		EnableControls(STYLE_MASK | BRIGHTNESS_MASK | COLOR_MASK, false, false);
		OnColor(led.clrLed);
		break;
	case LED_STYLE_CIRCLING:
		SetLogoLedTimer(TIMER_CIRCLING_ID, TIMER_CIRCLING_CLK);
		EnableControls(STYLE_MASK | BRIGHTNESS_MASK, false, false);
		break;
	case LED_STYLE_WAVE:
		SetLogoLedTimer(0, 0);
		EnableControls(STYLE_MASK, false, false);
		OnColor(led.clrLed);
		break;
	}
}

void CLedDlg::OnColor(COLORREF clr)
{
	 double r = GetRValue(clr);
	 double g = GetGValue(clr);
	 double b = GetBValue(clr);
	 m_edit[EDIT_LED_R].SetWindowsValue(r);
	 m_edit[EDIT_LED_G].SetWindowsValue(g);
	 m_edit[EDIT_LED_B].SetWindowsValue(b);
	 m_edit[EDIT_LED_CODE].SetWindowText(RGBToHex(clr));
	 m_slider[SLIDER_LED_RGB].SetPos((int)RGBtoHSB(clr));

	if (r == 255 && g == 0 && b == 255) { b -= 1; clr = RGB(r, g, b); }
	// if mutil-color is choosed, 
	// set the AORUS LOGO BK color at the same time.
	if ((!m_btn[BTN_LED_COLOR_R].IsCheck() && m_btn[BTN_LED_COLOR_R].IsEnable()) || 
		!m_btn[BTN_LED_COLOR_R].IsEnable() || !m_btn[BTN_LED_COLOR_R].IsWindowVisible())
	{
		m_stc[STC_LED_AORUSLOGO].SetBkColor(clr); 
		SyncAorusLedBtnClr(clr);
	}
}

BOOL CLedDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && 
		pMsg->wParam == VK_ESCAPE)   
	{
		return TRUE;  // Ignored ESC key
	}
	if (pMsg->message != WM_KEYDOWN || 
		pMsg->wParam != VK_RETURN)
	{
		return CUiDlg::PreTranslateMessage(pMsg);
	}

	// Allow user to change R/G/B value, and press ENTER key to change the color 
 
	DWORD dwID = GetFocus()->GetDlgCtrlID();
	switch(dwID)
	{
	case EDIT_LED_R: case EDIT_LED_G: case EDIT_LED_B: 
		EditRGBChange(); break;
	case EDIT_LED_CODE: {
		CString sCode;
		m_edit[dwID].GetWindowText(sCode);
		if(sCode.GetLength() < 6) break;

		m_ledProfiles[GetCardSel()][m_nProSel].clrLed = HexToRGB(sCode);
		m_ledProfiles[GetCardSel()][m_nProSel].bMutilColor = false;
		bool bSyncLed = IsSyncLed();
		if(bSyncLed)
		{
			TranToSyncStyle(m_ledProfiles[GetCardSel()][m_nProSel]);
		}
		ApplyLedSetting(GetCardSel(), m_ledProfiles[GetCardSel()][m_nProSel], bSyncLed, false);
		} break;
	}

	return CUiDlg::PreTranslateMessage(pMsg);
}

// Show or hide controls according to different setting.
void CLedDlg::EnableControls(DWORD dwMask, bool bShowVariation, bool bShowMultiBritness)
{
	m_combobox[COMBOX_LED_STYLE].Enable(dwMask & STYLE_MASK ? true : false);
	m_combobox[COMBOX_LED_VARIATION].Enable(dwMask & VARIATION_MASK ? true : false);
	m_stc[STC_LED_SPEED_COMBINE].ShowWindow(dwMask & SPEED_MASK ? SW_SHOW : SW_HIDE);
	m_slider[SLIDER_LED_BRGIHTNESS].ShowWindow(dwMask & BRIGHTNESS_MASK ? SW_SHOW : SW_HIDE);
	m_stc[STC_LED_BRIGHTNESS].ShowWindow(dwMask & BRIGHTNESS_MASK ? SW_SHOW: SW_HIDE);

    for(int i = BTN_LED_COLOR_1; i <= BTN_LED_COLOR_9; i++)
	{
		m_btn[i].Enable(dwMask & COLOR_MASK ? true : false);
	}
	for (int i = EDIT_LED_R; i <= EDIT_LED_CODE; i++)
	{
		m_edit[i].EnableWindow(dwMask & COLOR_MASK ? true : false);
	}
	m_slider[SLIDER_LED_RGB].Enable(dwMask & COLOR_MASK ? true : false);
	m_btn[BTN_LED_COLOR_R].Enable(dwMask & RANDOM_MASK ? true : false);
	m_stc[STC_LED_VARIATION_COMBINE].ShowWindow(bShowVariation ? SW_SHOW : SW_HIDE);
	if (dwMask & COLOR_MASK)
	{
		m_stc[STC_LED_HIDE_LEDCOLR].SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		m_stc[STC_LED_HIDE_LEDCOLR].ShowWindow(SW_HIDE);
		for (int i = EDIT_LED_R; i <= EDIT_LED_CODE; i++)		
		{
			m_edit[i].ShowWindow(SW_SHOW);
		}
		m_edit[EDIT_LED_CODE].ShowWindow(SW_HIDE);
		for (int i = BTN_LED_COLOR_1; i <= BTN_LED_COLOR_9; i++)
		{
			m_btn[i].ShowWindow(SW_SHOW);
		}
		m_slider[SLIDER_LED_RGB].ShowWindow(SW_SHOW);
		UpdateWindow();
	}
	else // No user-defined color
	{
		m_stc[STC_LED_HIDE_LEDCOLR].SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		m_stc[STC_LED_HIDE_LEDCOLR].ShowWindow(SW_SHOW);
		for (int i = EDIT_LED_R; i <= EDIT_LED_CODE; i++)
		{
			m_edit[i].ShowWindow(SW_HIDE);
		}
		for(int i = BTN_LED_COLOR_1;i <= BTN_LED_COLOR_9; i++)
		{
			m_btn[i].ShowWindow(SW_HIDE);
		}
		m_slider[SLIDER_LED_RGB].ShowWindow(SW_HIDE);
	}

	CRect rc;
	if (!m_stc[STC_LED_VARIATION_COMBINE].IsWindowVisible() && !m_stc[STC_LED_SPEED_COMBINE].IsWindowVisible())
	{
		m_slider[SLIDER_LED_BRGIHTNESS].GetWindowRect(rc);
		m_slider[SLIDER_LED_BRGIHTNESS].MoveWindow(98, 127 + m_nShiftTop, rc.Width(), rc.Height());
		m_stc[STC_LED_BRIGHTNESS].MoveWindow(CRect(12, 124 + m_nShiftTop, 93, 140 + m_nShiftTop));
	}
	else
	{
		m_slider[SLIDER_LED_BRGIHTNESS].GetWindowRect(rc);
		m_slider[SLIDER_LED_BRGIHTNESS].MoveWindow(98, 161 + m_nShiftTop, rc.Width(), rc.Height());
	    m_stc[STC_LED_BRIGHTNESS].MoveWindow(CRect(12, 158 + m_nShiftTop, 93, 173 + m_nShiftTop));
	}
}

// Set timer to show led effects on AORUS logo.
void CLedDlg::SetLogoLedTimer(int nTimerId, int nTimerClk)
{
	// nTimerId is 0:  kill all timers

	KillTimer(TIMER_CIRCLING_ID);
	KillTimer(TIMER_BREAHING_ID);
	KillTimer(TIMER_FLASHING_ID);
	KillTimer(TIMER_DUALFLASHING_ID);
	KillTimer(TIMER_BREATH_MUTILCLR_ID);
	KillTimer(TIMER_FLASHING_MUTILCLR_ID);
	KillTimer(TIMER_DUALFLASHING_MUTILCLR_ID);
	KillTimer(TIMER_LEDMONITORING_ID);
	if (nTimerId <= 0)
		return;
	SetTimer(nTimerId, nTimerClk, NULL);
}

void CLedDlg::EditRGBChange()
{
	CString sR, sG, sB;

	m_edit[EDIT_LED_R].GetWindowText(sR);
	if(_ttoi(sR) < 0) sR.Format(_T("%03d"), 0);
	if(_ttoi(sR) > 255) sR.Format(_T("%03d"), 255);
	m_edit[EDIT_LED_R].SetWindowText(sR);

	m_edit[EDIT_LED_G].GetWindowText(sG);
	if(_ttoi(sG) < 0) sG.Format(_T("%03d"), 0);
	if(_ttoi(sG) > 255) sG.Format(_T("%03d"), 255);
	m_edit[EDIT_LED_G].SetWindowText(sG);

	m_edit[EDIT_LED_B].GetWindowText(sB);
	if(_ttoi(sB) < 0) sB.Format(_T("%03d"), 0);
	if(_ttoi(sB) > 255) sB.Format(_T("%03d"), 255);
	m_edit[EDIT_LED_B].SetWindowText(sB);

	m_edit[EDIT_LED_R].GetWindowText(sR);
	m_edit[EDIT_LED_G].GetWindowText(sG);
	m_edit[EDIT_LED_B].GetWindowText(sB);
				 
	m_ledProfiles[GetCardSel()][m_nProSel].bMutilColor = false;
	m_ledProfiles[GetCardSel()][m_nProSel].clrLed = RGB(_ttoi(sR), _ttoi(sG), _ttoi(sB));

	bool bSyncLed = IsSyncLed();
	if (bSyncLed)
	{
		TranToSyncStyle(m_ledProfiles[GetCardSel()][m_nProSel]);
	}

	UpdateUI(m_ledProfiles[GetCardSel()][m_nProSel]);
	UpdateWindow();
	ApplyLedSetting(GetCardSel(), m_ledProfiles[GetCardSel()][m_nProSel], bSyncLed, false);
}

void CLedDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow)
	{
		InitProfile();
		OnUiComboBoxSelChanged(COMBOX_LED_PROFILE, m_nProSel);
		m_combobox[COMBOX_LED_PROFILE].SetCurSel(m_nProSel);
	}
	CWnd::OnShowWindow(bShow, nStatus);
}

void CLedDlg::InitProfile()
{
	int nDevices = 1; 
	if (DEVICE_VGA == m_nDevice)
	{
		CVGA* pVGA = (CVGA*)Aorus()->GetDevice(DEVICE_VGA);
		nDevices = pVGA->Ctrl()->GetGpuCount();
	}

	LED_SETTING ledCur;
	for (int i = 0; i < nDevices; i++)
	{
		int nProSel = 0;
		CArray<LED_SETTING> profiles;
		Aorus()->GetCfg()->GetLedProfiles(m_nDevice, profiles, nProSel, i);
		if ((int)profiles.GetSize() <= 0)
		{
			m_nProSel = 0;
			LED_SETTING ledNew;
			ledNew.sProfileName = "Profile1";
			profiles.Add(ledNew);
		}
		for(int k = 0; k < (int)profiles.GetSize(); k++)
		{
			LED_SETTING ledP = profiles.GetAt(k);

			// double check
			int nStyleIdx = 0;
			for (int mm = 0; mm < (int)m_Styles.GetSize(); mm++)
			{
				if (m_Styles.GetAt(mm).nId == (int)ledP.dwStyle) { nStyleIdx = mm; break; }
			}
			ledP.dwStyle = (DWORD)m_Styles.GetAt(nStyleIdx).nId;

			m_ledProfiles[i][k] = ledP;
		}
	}
	m_combobox[COMBOX_LED_PROFILE].ResetContent();
	for(int i = 0; i < m_nProfiles; i++)
	{
		m_combobox[COMBOX_LED_PROFILE].AddString(m_ledProfiles[GetCardSel()][i].sProfileName, 0, CLR_WHITE);
	}


}