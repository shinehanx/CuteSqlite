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

 * @file   RowDataFormView.h
 * @brief  Show form view for cliked the one row data in ResultListPage
 * 
 * @author Xuehan Qin
 * @date   2023-06-16
 *********************************************************************/
#pragma once
#include <atlgdi.h>
#include "ui/common/page/QPage.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"

static int cxChar, cyChar, iVscrollPos, y=5;
class RowDataFormView : public QPage
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(RowDataFormView)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(ResultListPageAdapter * adapter);
	void loadFormData();
private:
	ResultListPageAdapter * adapter = nullptr;

	HFONT textFont = nullptr;

	std::vector<CStatic *> labels;
	std::vector<CEdit *> edits;

	TEXTMETRIC tm;
	SCROLLINFO si;
	int nHeightSum = 0;

	void clearLabelsAndEdits();
	void showColumnsAndValues(Columns & columns, RowItem & rowItem);

	virtual void paintItem(CDC & dc, CRect & paintRect);
	void initScrollBar(CSize & clientSize);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void OnSize(UINT nType, CSize size);

	

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	
};
