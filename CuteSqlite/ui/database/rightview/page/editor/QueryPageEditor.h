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

 * @file   QueryPageEditor.h
 * @brief  Editor for QueryPage class, extended the toolbar buttons and menus
 * 
 * @author Xuehan Qin
 * @date   2023-11-13
 *********************************************************************/
#pragma once

#include "ui/common/edit/QHelpEdit.h"
#include "ui/common/button/QDropButton.h"
#include "ui/database/rightview/page/editor/adapter/QueryPageEditorAdapter.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"

class QueryPageEditor : public QHelpEdit
{
public:
	BEGIN_MSG_MAP_EX(QueryPageEditor)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(Config::EDITOR_TEMPLATES_BUTTON_ID, BN_CLICKED, OnClickTemplatesButton)
		COMMAND_HANDLER_EX(Config::EDITOR_PRAGMAS_BUTTON_ID, BN_CLICKED, OnClickPragmasButton)
		COMMAND_HANDLER_EX(Config::EDITOR_CLEAR_ALL_BUTTON_ID, BN_CLICKED, OnClickClearAllButton)
		COMMAND_HANDLER_EX(Config::EDITOR_SQL_LOG_BUTTON_ID, BN_CLICKED, OnClickSqlLogButton)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::TEMPLATES_SELECT_STMT_MEMU_ID, Config::TEMPLATES_WITH_STMT_MEMU_ID, BN_CLICKED, OnClickTemplatesMenu)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::PRAGMAS_MENU_ID_START, Config::PRAGMAS_MENU_ID_END, BN_CLICKED, OnClickPragmasMenu)
		MESSAGE_HANDLER_EX(Config::MSG_USE_SQL_ID, OnHandleUseSql)
		CHAIN_MSG_MAP(QHelpEdit)
		// REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void setup(QueryPageSupplier * supplier);
private:
	COLORREF buttonColor = RGB(238, 238, 238);

	QDropButton templatesButton;
	QDropButton pragmasButton;
	QDropButton clearAllButton;
	QDropButton sqlLogButton;
	
	QueryPageSupplier * supplier = nullptr;

	virtual void createOrShowUI();
	virtual void createOrShowToolBarElems(CRect & clientRect);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	void OnClickTemplatesButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickPragmasButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickClearAllButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd);

	void OnClickTemplatesMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickPragmasMenu(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnHandleUseSql(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
