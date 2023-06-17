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

 * @file   ResultFilterDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-06-17
 *********************************************************************/
#include "stdafx.h"
#include "ResultFilterDialog.h"
#include "ui/common/QWinCreater.h"

#define RESULT_FILTER_DLG_WIDTH 385
#define RESULT_FILTER_ELEM_WIDTH 100
#define RESULT_FILTER_ELEM_HEIGHT 20
#define RESULT_FILTER_ELEM_SPLIT 10
ResultFilterDialog::ResultFilterDialog(HWND parentHwnd, ResultListPageAdapter * adapter, CRect & btnRect)
{
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->btnRect = btnRect;
	this->caption = S(L"filter");
}


void ResultFilterDialog::initWindowRect()
{
	auto filters = adapter->getRuntimeFilters();
	int rows = filters.size() ? static_cast<int>(filters.size()) : 1;
	int x = btnRect.right - RESULT_FILTER_DLG_WIDTH, y = btnRect.bottom + 5,
		w = RESULT_FILTER_DLG_WIDTH, 
		h = (rows + 1) * (RESULT_FILTER_ELEM_HEIGHT + RESULT_FILTER_ELEM_SPLIT) + QDIALOG_BUTTON_HEIGHT + 20 + 40;

	::SetWindowPos(m_hWnd, GetParent().m_hWnd,  x, y, w, h, SWP_SHOWWINDOW);
}

void ResultFilterDialog::createOrShowUI()
{
	initWindowRect();
	QDialog::createOrShowUI();

	createOrShowElems();
}


void ResultFilterDialog::clearElems()
{
	for (auto item : columnComboBoxes) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	columnComboBoxes.clear();

	for (auto item : operatorComboBoxes) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	operatorComboBoxes.clear();

	for (auto item : valueEdits) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	valueEdits.clear();

	for (auto item : opButtons) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	opButtons.clear();
}

void ResultFilterDialog::createOrShowElems()
{
	clearElems();
	CRect clientRect;
	GetClientRect(clientRect);
	// header labels
	createOrShowHeaderLabels(clientRect);

	//condition row elements
	DataFilters filters = adapter->getRuntimeFilters();
	if (filters.empty()) {
		createOrShowRowElems(0, FilterTuple(), clientRect);
		return ;
	}
	int n = static_cast<int>(filters.size());
	for (int i = 0; i < n; i++) {
		FilterTuple item = filters.at(i);
		createOrShowRowElems(i, item, clientRect);
	}
}


void ResultFilterDialog::createOrShowHeaderLabels(CRect & clientRect)
{
	int x = RESULT_FILTER_ELEM_SPLIT, 
		y =  RESULT_FILTER_ELEM_SPLIT, 
		w = RESULT_FILTER_ELEM_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	CStatic * columnLabel = new CStatic();
	createOrShowFormLabel(*columnLabel, S(L"column"), rect, clientRect, SS_LEFT);
	headerLabels.push_back(columnLabel);

	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	CStatic * conditionLabel = new CStatic();
	createOrShowFormLabel(*conditionLabel, S(L"condition"), rect, clientRect, SS_LEFT);
	headerLabels.push_back(conditionLabel);

	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	CStatic * valueLabel = new CStatic();
	createOrShowFormLabel(*valueLabel, S(L"value"), rect, clientRect, SS_LEFT);
	headerLabels.push_back(valueLabel);
}


void ResultFilterDialog::createOrShowRowElems(int nIndex, FilterTuple & tuple, CRect & clientRect)
{	
	Columns columns = adapter->getRuntimeColumns();
	std::vector<std::wstring> operators = {L"=", L"<>", L">", L"<", L">=", L"<=", L"like"};

	// column
	CComboBox * columnComboBox = new CComboBox();
	int x = RESULT_FILTER_ELEM_SPLIT, 
		y = (nIndex + 1) * (RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM_HEIGHT) , 
		w = RESULT_FILTER_ELEM_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormComboBox(*columnComboBox, 0, L"", rect, clientRect);
	loadComboBox(columnComboBox, columns, std::get<0>(tuple));
	columnComboBoxes.push_back(columnComboBox);	

	CComboBox * operatorComboBox = new CComboBox();
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	createOrShowFormComboBox(*operatorComboBox, 0, L"", rect, clientRect);
	loadComboBox(operatorComboBox, operators, std::get<1>(tuple));
	operatorComboBoxes.push_back(operatorComboBox);

	CEdit * valueEdit = new CEdit();
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	createOrShowFormEdit(*valueEdit, 0, std::get<2>(tuple), L"", rect, clientRect, 0, false);
	valueEdits.push_back(valueEdit);

	CButton * opButton = new CButton();
	x = rect.right +  RESULT_FILTER_ELEM_SPLIT;
	w = RESULT_FILTER_ELEM_HEIGHT;
	rect = { x, y, x + w, y + h };
	createOrShowFormButton(*opButton, 0, L"+", rect, clientRect);
	opButtons.push_back(opButton);
}


void ResultFilterDialog::loadComboBox(CComboBox * ptr, std::vector<std::wstring> & strs, std::wstring & defVal)
{
	ptr->ResetContent();
	for (auto str : strs) {
		ptr->AddString(str.c_str());
	}
	if (!defVal.empty()) {
		ptr->SelectString(-1, defVal.c_str());
	}
}

LRESULT ResultFilterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ResultFilterDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled); 

	clearElems();
	return 0;
}

