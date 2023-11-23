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

 * @file   SqlLogDialog.h
 * @brief  This dialog for show the sql log
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#pragma once
#include <atlcrack.h>
#include "resource.h"
#include "core/service/db/DatabaseService.h"
#include "ui/database/rightview/page/editor/adapter/QueryPageEditorAdapter.h"
#include "ui/common/button/QImageButton.h"
#include "ui/common/edit/QSearchEdit.h"
#include "ui/database/rightview/page/editor/list/SqlLogListBox.h"
#include "core/service/sqllog/SqlLogService.h"

#define  SQL_LOG_DIALOG_WIDTH 500
#define  SQL_LOG_DIALOG_HEIGHT 600
class SqlLogDialog : public CDialogImpl<SqlLogDialog> {
public:
	enum { IDD = IDD_QFORM_DIALOG };

	BEGIN_MSG_MAP_EX(SqlLogDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		
		COMMAND_HANDLER_EX(Config::CLOSE_IMAGE_BUTTON_ID, BN_CLICKED, OnClickCloseButton)
		COMMAND_HANDLER_EX(Config::FIRST_PAGE_BUTTON_ID, BN_CLICKED, OnClickFirstPageButton)
		COMMAND_HANDLER_EX(Config::PREV_PAGE_BUTTON_ID, BN_CLICKED, OnClickPrevPageButton)
		COMMAND_HANDLER_EX(Config::NEXT_PAGE_BUTTON_ID, BN_CLICKED, OnClickNextPageButton)
		COMMAND_HANDLER_EX(Config::LAST_PAGE_BUTTON_ID, BN_CLICKED, OnClickLastPageButton)
		
		MESSAGE_HANDLER_EX(Config::MSG_QUERY_PAGE_USE_SQL_ID, OnClickUseButton)
		MESSAGE_HANDLER_EX(Config::MSG_QUERY_PAGE_TOP_SQL_LOG_ID, OnClickTopButton)
		MESSAGE_HANDLER_EX(Config::MSG_QUERY_PAGE_DEL_SQL_LOG_ID, OnClickDeleteButton)
		MESSAGE_HANDLER_EX(Config::MSG_SEARCH_BUTTON_ID, OnClickSearchButton)
		MESSAGE_HANDLER_EX(Config::MSG_QUERY_PAGE_NEXT_PAGE_SQL_LOG_ID, OnLoadNextPageSqlLog)

		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLORBTN(OnCtlBtnColor)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void setup(HWND parentHwnd, QueryPageEditorAdapter * adapter, CRect parentWinRect);
private:
	bool isNeedReload = true;
	CRect winRect;
	HWND parentHwnd = 0;

	COLORREF textColor = RGB(8, 8, 8);
	COLORREF bkgColor = RGB(219, 219, 219);
	CBrush bkgBrush;
	HFONT textFont = nullptr;

	QImageButton closeButton;
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

	QueryPageEditorAdapter * adapter = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	void createOrShowUI();
	void createOrShowCloseButton(CRect & clientRect);
	void createOrShowSearchEdit(QSearchEdit & win, CRect & clientRect);
	void createOrShowSqlLogListBox(SqlLogListBox & win, CRect & clientRect);
	void createOrShowPageElems(CRect & clientRect);

	void loadWindow();
	void loadSqlLogListBox(int page);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnClickCloseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickFirstPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickPrevPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickNextPageButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickLastPageButton(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnClickUseButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickTopButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickDeleteButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClickSearchButton(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLoadNextPageSqlLog(UINT uMsg, WPARAM wParam, LPARAM lParam);


	HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlBtnColor(HDC hdc, HWND hwnd);

	std::wstring formatDateForDisplay(const std::wstring & date);
	std::wstring getPageText(int curPage, int perPage, uint64_t total);
};