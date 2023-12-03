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

 * @file   SqlLogPage.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/common/button/QImageButton.h"
#include "ui/common/edit/QSearchEdit.h"
#include "ui/database/rightview/page/editor/list/SqlLogListBox.h"
#include "core/service/sqllog/SqlLogService.h"
#include "core/service/db/DatabaseService.h"

class SqlLogPage : public QTabPage<SqlLogPage> {
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(SqlLogPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLORBTN(OnCtlBtnColor)

		COMMAND_HANDLER_EX(Config::FIRST_PAGE_BUTTON_ID, BN_CLICKED, OnClickFirstPageButton)
		COMMAND_HANDLER_EX(Config::PREV_PAGE_BUTTON_ID, BN_CLICKED, OnClickPrevPageButton)
		COMMAND_HANDLER_EX(Config::NEXT_PAGE_BUTTON_ID, BN_CLICKED, OnClickNextPageButton)
		COMMAND_HANDLER_EX(Config::LAST_PAGE_BUTTON_ID, BN_CLICKED, OnClickLastPageButton)
		
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SQL_ID, OnClickAnalysisButton)
		MESSAGE_HANDLER_EX(Config::MSG_TOP_SQL_LOG_ID, OnClickTopButton)
		MESSAGE_HANDLER_EX(Config::MSG_DEL_SQL_LOG_ID, OnClickDeleteButton)
		MESSAGE_HANDLER_EX(Config::MSG_SEARCH_BUTTON_ID, OnClickSearchButton)
		MESSAGE_HANDLER_EX(Config::MSG_QUERY_PAGE_NEXT_PAGE_SQL_LOG_ID, OnLoadNextPageSqlLog)

		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
private:
	AnalysisPageType pageType = SQL_LOG_PAGE;
	bool isNeedReload = true;

	COLORREF textColor = RGB(8, 8, 8);
	COLORREF bkgColor = RGB(219, 219, 219);
	CBrush bkgBrush;
	HFONT textFont = nullptr;

	QSearchEdit searchEdit;
	SqlLogListBox sqlLogListBox;

	CStatic pageLabel;
	CButton firstPageButton;
	CButton prevPageButton;
	CButton nextPageButton;
	CButton lastPageButton;

	int curPage = 1;
	int maxPage = 1;
	int perPage = 10;
	std::wstring keyword;

	QueryPageSupplier queryPageSupplier;
	DatabaseService * databaseService = DatabaseService::getInstance();
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	void createOrShowUI();
	void createOrShowCloseButton(CRect & clientRect);
	void createOrShowSearchEdit(QSearchEdit & win, CRect & clientRect);
	void createOrShowSqlLogListBox(SqlLogListBox & win, CRect & clientRect);
	void createOrShowPageElems(CRect & clientRect);

	void loadWindow();
	void loadSqlLogListBox(int page);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlBtnColor(HDC hdc, HWND hwnd);
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnClickFirstPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickPrevPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickNextPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickLastPageButton(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnClickAnalysisButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickTopButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickDeleteButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickSearchButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLoadNextPageSqlLog(UINT uMsg, WPARAM wParam, LPARAM lParam);

	std::wstring formatDateForDisplay(const std::wstring & date);
	std::wstring getPageText(int curPage, int perPage, uint64_t total);
};
