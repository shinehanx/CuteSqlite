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
		COMMAND_RANGE_CODE_HANDLER_EX(Config::FILTER_OP_BUTTON_ID_START, Config::FILTER_OP_BUTTON_ID_END, BN_CLICKED, OnClickOpButton)
		COMMAND_HANDLER_EX(Config::QDIALOG_CLEAR_BUTTON_ID, BN_CLICKED, OnClickClearButton)
		CHAIN_MSG_MAP(QDialog<ResultFilterDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ResultFilterDialog(HWND parentHwnd, ResultListPageAdapter * adapter, CRect & btnRect);
private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ResultListPageAdapter * adapter = nullptr;
	CRect btnRect;

	CButton clearButton;
	// static elements for header
	std::vector<CStatic *>  headerLabels;

	// dynamic elements pointers, the size of elements will be equal
	std::vector<CComboBox *>  connectComboBoxes; // connect by and/or
	std::vector<CComboBox *>  columnComboBoxes;
	std::vector<CComboBox *>  operatorComboBoxes;
	std::vector<CEdit *> valueEdits;
	std::vector<CButton *> opButtons;

	void initWindowRect(int rows = 0);
	virtual void createOrShowUI();

	void clearElems();
	void createOrShowElems();

	void resizeElems();
	void resizeRowElems(int nIndex, CRect & clientRect);

	void createOrShowClearButton(CButton & win, CRect &clientRect);
	void createOrShowHeaderLabels(CRect & clientRect);
	void createOrShowRowElems(int nIndex, FilterTuple & tuple, CRect & clientRect);

	void updateOpButtonsText();

	void removeRowElems(int nIndex);

	template<typename E>
	void removeElem(std::vector<E *> & ptrs, int nIndex);

	void loadComboBox(CComboBox * ptr, std::vector<std::wstring> & columns, std::wstring & defVal);	

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnClickOpButton(UINT uNotifyCode, int nID, HWND hwnd);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickClearButton(UINT uNotifyCode, int nID, HWND hwnd);
};