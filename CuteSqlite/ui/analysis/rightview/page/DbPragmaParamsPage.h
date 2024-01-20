/*****************************************************************//**
 * Copyright 2023 Xuehan Qin 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and

 * limitations under the License.

 * @file   DbPragmaParamsPage.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#pragma once
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/common/button/QImageButton.h"
#include "common/Config.h"
#include "core/entity/Entity.h"
#include "ui/common/edit/QSqlEdit.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/param/QParamElem.h"
#include "adapter/DbPragmaParamPageAdapter.h"
#include "core/service/db/DatabaseService.h"
#include "supplier/DbPragmaParamSupplier.h"


class DbPragmaParamsPage : public QTabPage<DbPragmaParamsPage>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(DbPragmaParamsPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		COMMAND_HANDLER_EX(Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshButton)
		COMMAND_HANDLER_EX(Config::ANALYSIS_DB_PRAGMAS_COMBOBOX_ID, CBN_SELENDOK, OnChangePragmaComboBox)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORBTN(OnCtlColorBtn)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	DbPragmaParamsPage(uint64_t sqlLogId);
	uint64_t getUserDbId();
private:
	bool isNeedReload = true;
	
	COLORREF textColor = RGB(64, 64, 64);
	COLORREF sectionColor = RGB(8, 8, 8);
	COLORREF btnBkgColor = RGB(210, 210, 210);

	COLORREF lineColor = RGB(127, 225, 244);
	COLORREF headerBkgColor = RGB(81, 81, 81);
	COLORREF headerTextColor = RGB(255, 255, 255);

	HFONT titleFont = nullptr;
	HFONT textFont = nullptr;
	HFONT sectionFont = nullptr;
	HFONT comboFont = nullptr;

	CPen linePen;
	CBrush headerBrush;

	QStaticImage titleImage;
	CEdit titleEdit;
	CStatic databaseLabel;
	CEdit databaseEdit;
	CStatic pragmaLabel;
	CComboBox pragmaComboBox;
	QImageButton refreshButton;

	std::vector<QParamElem *> paramElemPtrs;

	DbPragmaParamPageAdapter * adapter = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();
	DbPragmaParamSupplier supplier;

	// scroll bar
	TEXTMETRIC tm;
	SCROLLINFO si;
	int nHeightSum = 0;
	int cxChar = 5, cyChar = 5, iVscrollPos = 5, vScrollPages = 5;
	void initScrollBar(CSize & clientSize);

	virtual void createOrShowUI();
	void createOrShowTitleElems(CRect & clientRect);
	void createOrShowPragmaParamElems(CRect & clientRect);
	
	void createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowPragmaParamElem(QParamElem &win, CRect & rect, CRect clientRect);

	virtual void loadWindow();
	void loadPragmaComboBox();
	
	void clearParamElemPtrs();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnChangePragmaComboBox(UINT uNotifyCode, int nID, HWND hwnd);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorBtn(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);

};
