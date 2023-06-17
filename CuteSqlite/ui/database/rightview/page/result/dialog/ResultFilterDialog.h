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

 * @file   ResultFilterDialog.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-06-17
 *********************************************************************/
#pragma once
#include <vector>
#include "ui/common/dialog/QDialog.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"

class ResultFilterDialog : public QDialog<ResultFilterDialog>
{
public:
	BEGIN_MSG_MAP_EX(ResultFilterDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(QDialog<ResultFilterDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ResultFilterDialog(HWND parentHwnd, ResultListPageAdapter * adapter, CRect & btnRect);
private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ResultListPageAdapter * adapter = nullptr;
	CRect btnRect;
	// static elements for header
	std::vector<CStatic *>  headerLabels;

	// dynamic elements pointers, the size of elements will be equal
	std::vector<CComboBox *>  columnComboBoxes;
	std::vector<CComboBox *>  operatorComboBoxes;
	std::vector<CEdit *> valueEdits;
	std::vector<CButton *> opButtons;

	void initWindowRect();
	virtual void createOrShowUI();

	void clearElems();
	void createOrShowElems();
	void createOrShowHeaderLabels(CRect & clientRect);
	void createOrShowRowElems(int nIndex, FilterTuple & tuple, CRect & clientRect);

	void loadComboBox(CComboBox * ptr, std::vector<std::wstring> & columns, std::wstring & defVal);
	

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};