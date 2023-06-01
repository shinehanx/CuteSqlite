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

 * @file   LeftTreeView.h
 * @brief  Left tree view for splitter
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "ui/common/button/QImageButton.h"
#include "ui/database/leftview/adapter/LeftTreeViewAdapter.h"

class LeftTreeView : public CWindowImpl<LeftTreeView> 
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftTreeView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)

		COMMAND_HANDLER_EX(Config::DATABASE_CREATE_BUTTON_ID, BN_CLICKED, OnClickCreateDbButton)
		COMMAND_HANDLER_EX(Config::DATABASE_OPEN_BUTTON_ID, BN_CLICKED, OnClickOpenDbButton)
		COMMAND_HANDLER_EX(Config::DATABASE_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshDbButton)
		COMMAND_HANDLER_EX(Config::DATABASE_DELETE_BUTTON_ID, BN_CLICKED, OnClickDeleteDbButton)
		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, TVN_SELCHANGED, OnChangeTreeViewItem)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;
	COLORREF topbarColor = RGB(238, 238, 238);
	HBRUSH topbarBrush = nullptr;
	HFONT comboFont = nullptr;

	QImageButton createDbButton;
	QImageButton openDbButton;
	QImageButton refreshDbButton;
	QImageButton deleteDbButton;
	CComboBox seletedDbComboBox;

	CTreeViewCtrlEx treeView;
	
	// singleton pointer
	LeftTreeViewAdapter * treeViewAdapter = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();

	CRect getTopRect(CRect & clientRect);
	CRect getTreeRect(CRect & clientRect);

	void createOrShowUI();
	void createOrShowCreateDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowOpenDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowRefreshDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowDeleteDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowTreeView(CTreeViewCtrlEx & win, CRect & clientRect);
	void createOrShowSelectedDbComboBox(CComboBox & win, CRect & clientRect);

	void loadWindow();
	void loadComboBox();
	void selectComboBox(uint64_t userDbId);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	LRESULT OnClickCreateDbButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickOpenDbButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickRefreshDbButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickDeleteDbButton(UINT uNotifyCode, int nID, HWND hwnd);
	// change selected treeview item .
	LRESULT OnChangeTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);
};
