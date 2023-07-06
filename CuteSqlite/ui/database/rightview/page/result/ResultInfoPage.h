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

 * @file   ResultInfoPage.h
 * @brief  Execute sql statement and show the info of query result
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#pragma once
#include "ui/common/page/QPage.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"

class ResultInfoPage : public QPage {
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(ResultInfoPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID, OnExecSqlResultMessage)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	HFONT textFont = nullptr;

	CEdit infoEdit;
	ResultListPageAdapter * adapter = nullptr;

	virtual void createOrShowUI();
	virtual void loadWindow();

	void createOrShowInfoEditor(CRect & clientRect);
	virtual void paintItem(CDC & dc, CRect & paintRect);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	LRESULT OnExecSqlResultMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
};
