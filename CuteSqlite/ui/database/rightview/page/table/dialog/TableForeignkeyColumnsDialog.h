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

 * @file   TableForeignkeyColumnsDialog.h
 * @brief  The dialog for select columns for index in the indexes ListView
 * 
 * @author Xuehan Qin
 * @date   2023-10-16
 *********************************************************************/
#pragma once
#include "AbstractTableColumnsDialog.h"
#include "core/service/db/TableService.h"
#include "ui/database/rightview/page/table/adapter/TableColumnsPageAdapter.h"
#include "ui/database/rightview/page/table/adapter/TableForeignkeysPageAdapter.h"

class TableForeignkeyColumnsDialog : public AbstractTableColumnsDialog
{
public:
	BEGIN_MSG_MAP_EX(TableForeignkeyColumnsDialog)
		CHAIN_MSG_MAP(AbstractTableColumnsDialog)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	TableForeignkeyColumnsDialog(HWND parentHwnd, 
		TableColumnsPageAdapter * columnPageAdapter,
		TableForeignkeysPageAdapter * foreignkeyPageAdapter, 
		CRect btnRect, int iItem, int iSubItem);
private:
	TableService * tableService = TableService::getInstance();
	TableColumnsPageAdapter * columnPageAdapter = nullptr;
	TableForeignkeysPageAdapter * foreignkeyPageAdapter = nullptr;
	
	virtual void loadLeftListBox();
	virtual void loadRightListBox();
	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};

