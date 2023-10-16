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

 * @file   TableIndexColumnsDialog.h
 * @brief  The dialog for select columns for index in the indexes ListView
 * 
 * @author Xuehan Qin
 * @date   2023-10-16
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "ui/database/rightview/page/table/adapter/TableColumnsPageAdapter.h"
#include "ui/database/rightview/page/table/adapter/TableIndexesPageAdapter.h"

class TableIndexColumnsDialog : public QDialog<TableIndexColumnsDialog>
{
public:
	BEGIN_MSG_MAP_EX(TableIndexColumnsDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER_EX(Config::TABLE_INDEX_MOVE_RIGHT_BUTTON_ID, BN_CLICKED, OnClickMoveRightButton)
		COMMAND_HANDLER_EX(Config::TABLE_INDEX_MOVE_LEFT_BUTTON_ID, BN_CLICKED, OnClickMoveLeftButton)
		COMMAND_HANDLER_EX(Config::TABLE_INDEX_MOVE_UP_BUTTON_ID, BN_CLICKED, OnClickMoveUpButton)
		COMMAND_HANDLER_EX(Config::TABLE_INDEX_MOVE_DOWN_BUTTON_ID, BN_CLICKED, OnClickMoveDownButton)
		CHAIN_MSG_MAP(QDialog<TableIndexColumnsDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	TableIndexColumnsDialog(HWND parentHwnd, TableColumnsPageAdapter * columnAdapter, 
		TableIndexesPageAdapter * adapter, CRect btnRect, int iItem, int iSubItem);
private:
	bool isNeedReload = true;
	HWND parentHwnd;
	TableColumnsPageAdapter * columnPageAdapter = nullptr;
	TableIndexesPageAdapter * indexPageAdapter = nullptr;
	int iItem = 0;
	int iSubItem = 0;
	CRect btnRect;

	CStatic leftLabel;
	CStatic rightLabel;
	CListBox leftListBox; // colums list
	CListBox rightListBox; // selected list
	CButton moveRightButton;
	CButton moveLeftButton;
	CButton moveUpButton;
	CButton moveDownButton;

	// create elements when initting dialog
	virtual void createOrShowUI();
	void initWindowRect();
	void createOrShowLeftListBox(CRect & clientRect);
	void createOrShowRightListBox(CRect & clientRect);
	void createOrShowButtons(CRect & clientRect);

	// load data when showing the window visible
	void loadWindow();
	void loadLeftListBox();
	void loadRightListBox();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnClickMoveRightButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickMoveLeftButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickMoveUpButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickMoveDownButton(UINT uNotifyCode, int nID, HWND hwnd);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};

