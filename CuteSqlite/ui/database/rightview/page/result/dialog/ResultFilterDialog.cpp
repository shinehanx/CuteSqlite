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

#define RESULT_FILTER_DLG_WIDTH 445
#define RESULT_FILTER_ELEM_WIDTH 100
#define RESULT_FILTER_ELEM_HEIGHT 20
#define RESULT_FILTER_ELEM2_WIDTH 20
#define RESULT_FILTER_ELEM3_WIDTH 50
#define RESULT_FILTER_ELEM_SPLIT 10
ResultFilterDialog::ResultFilterDialog(HWND parentHwnd, ResultListPageAdapter * adapter, CRect & btnRect)
{
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->btnRect = btnRect;
	this->caption = S(L"filter");
}


void ResultFilterDialog::initWindowRect(int rows)
{
	if (rows == 0) {
		auto filters = adapter->getRuntimeFilters();
		rows = filters.size() ? static_cast<int>(filters.size()) : 1;
	}
	
	int x = btnRect.right - RESULT_FILTER_DLG_WIDTH, y = btnRect.bottom + 5,
		w = RESULT_FILTER_DLG_WIDTH, 
		h = (rows + 1) * (RESULT_FILTER_ELEM_HEIGHT + RESULT_FILTER_ELEM_SPLIT) + QDIALOG_BUTTON_HEIGHT + 20 + 60;

	::SetWindowPos(m_hWnd, GetParent().m_hWnd,  x, y, w, h, SWP_SHOWWINDOW);

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowYesButton(yesButton, clientRect);
	createOrShowNoButton(noButton, clientRect);
}

void ResultFilterDialog::createOrShowUI()
{
	initWindowRect();
	QDialog::createOrShowUI();

	createOrShowElems();
}


void ResultFilterDialog::clearElems()
{
	for (auto item : headerLabels) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	headerLabels.clear();

	for (auto item : connectComboBoxes) {
		if (item) {
			if (item->IsWindow()) item->DestroyWindow();
			delete item;
			item = nullptr;
		}
	}
	connectComboBoxes.clear();

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
	int x = RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM3_WIDTH + RESULT_FILTER_ELEM_SPLIT, 
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
	std::vector<std::wstring> connects = {L"AND", L"OR"};

	// and or combobox
	int x = RESULT_FILTER_ELEM_SPLIT, 
		y = (nIndex + 1) * (RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM_HEIGHT) , 
		w = RESULT_FILTER_ELEM3_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	CComboBox * connectComboBox = nullptr;
	if (nIndex > 0) {
		connectComboBox = new CComboBox();
		createOrShowFormComboBox(*connectComboBox, 0, L"", rect, clientRect);
		loadComboBox(connectComboBox, connects, std::get<0>(tuple));
	}
	connectComboBoxes.push_back(connectComboBox);

	// column
	CComboBox * columnComboBox = new CComboBox();
	x = rect.right + RESULT_FILTER_ELEM_SPLIT, 
		y = (nIndex + 1) * (RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM_HEIGHT) , 
		w = RESULT_FILTER_ELEM_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	rect = { x, y, x + w, y + h };
	createOrShowFormComboBox(*columnComboBox, 0, L"", rect, clientRect);
	loadComboBox(columnComboBox, columns, std::get<1>(tuple));
	columnComboBoxes.push_back(columnComboBox);	

	// operator
	CComboBox * operatorComboBox = new CComboBox();
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	createOrShowFormComboBox(*operatorComboBox, 0, L"", rect, clientRect);
	loadComboBox(operatorComboBox, operators, std::get<2>(tuple));
	operatorComboBoxes.push_back(operatorComboBox);

	// value
	CEdit * valueEdit = new CEdit();
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	createOrShowFormEdit(*valueEdit, 0, std::get<3>(tuple), L"", rect, clientRect, 0, false);
	valueEdits.push_back(valueEdit);

	// +/- button
	CButton * opButton = new CButton();
	x = rect.right +  RESULT_FILTER_ELEM_SPLIT;
	w = RESULT_FILTER_ELEM_HEIGHT;
	rect = { x, y, x + w, y + h };
	createOrShowFormButton(*opButton, Config::FILTER_OP_BUTTON_ID_START + nIndex, L"+", rect, clientRect);
	opButtons.push_back(opButton);

	// change opButton text with +/-
	updateOpButtonsText();
}

void ResultFilterDialog::resizeElems()
{
	
	int n = static_cast<int>(opButtons.size());
	initWindowRect(n);

	CRect clientRect;
	GetClientRect(clientRect);
	for (int i = 0; i < n; i++) {
		resizeRowElems(i, clientRect);
	}
}

void ResultFilterDialog::resizeRowElems(int nIndex, CRect & clientRect)
{	
	// and or combobox
	int x = RESULT_FILTER_ELEM_SPLIT, 
		y = (nIndex + 1) * (RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM_HEIGHT) , 
		w = RESULT_FILTER_ELEM3_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	CComboBox * connectComboBox = connectComboBoxes.at(nIndex);
	if (nIndex == 0) {
		if (connectComboBox) {
			if (connectComboBox->IsWindow())
				connectComboBox->DestroyWindow();

			delete connectComboBox;
			connectComboBox = nullptr;
			connectComboBoxes[nIndex] = nullptr;
		}	
	} else {
		if (connectComboBox && connectComboBox->IsWindow()) {
			connectComboBox->MoveWindow(rect);
		}
	}

	// column
	CComboBox * columnComboBox = columnComboBoxes.at(nIndex);
	x = rect.right + RESULT_FILTER_ELEM_SPLIT, 
		y = (nIndex + 1) * (RESULT_FILTER_ELEM_SPLIT + RESULT_FILTER_ELEM_HEIGHT) , 
		w = RESULT_FILTER_ELEM_WIDTH, h = RESULT_FILTER_ELEM_HEIGHT;
	rect = { x, y, x + w, y + h };
	columnComboBox->MoveWindow(rect);

	// operator
	CComboBox * operatorComboBox = operatorComboBoxes.at(nIndex);
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	operatorComboBox->MoveWindow(rect);

	// value
	CEdit * valueEdit = valueEdits.at(nIndex);
	rect.OffsetRect(RESULT_FILTER_ELEM_WIDTH + RESULT_FILTER_ELEM_SPLIT, 0);
	valueEdit->MoveWindow(rect);

	// +/- button
	CButton * opButton = opButtons.at(nIndex);
	x = rect.right +  RESULT_FILTER_ELEM_SPLIT;
	w = RESULT_FILTER_ELEM_HEIGHT;
	rect = { x, y, x + w, y + h };
	opButton->MoveWindow(rect);
}

/**
 * change opButton text with +/-.
 * 
 */
void ResultFilterDialog::updateOpButtonsText()
{
	int n = static_cast<int>(opButtons.size());
	for (int i = 0; i < n; i++) {
		auto btnPtr = opButtons.at(i);
		if (btnPtr->IsWindow() && i < n - 1) {
			btnPtr->SetWindowText(L"-");
		}
		else {
			btnPtr->SetWindowText(L"+");
		}
	}
}

void ResultFilterDialog::removeRowElems(int nIndex)
{
	removeElem<CComboBox>(connectComboBoxes, nIndex);
	removeElem<CComboBox>(columnComboBoxes, nIndex);
	removeElem<CComboBox>(operatorComboBoxes, nIndex);
	removeElem<CEdit>(valueEdits, nIndex);
	removeElem<CButton>(opButtons, nIndex);

	// change opButton text with +/-
	updateOpButtonsText();

	// resize all elements
	resizeElems();
	return;
}

template<typename E>
void ResultFilterDialog::removeElem(std::vector<E *> & ptrs, int nIndex)
{
	int n = static_cast<int>(ptrs.size());
	if (nIndex < 0 || nIndex >= n) {
		return;
	}
	auto item = ptrs.at(nIndex);
	if (item) {
		if (item->IsWindow()) {
			item->DestroyWindow();
		}
		delete item;
		item = nullptr; 
	}

	auto iter = ptrs.begin();
	for (int i = 0; i < nIndex; i++) {
		iter++;
	}
	ptrs.erase(iter);
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

void ResultFilterDialog::OnClickOpButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int n = static_cast<int>(opButtons.size());
	int nIndex = -1;
	for (int i = 0; i < n; i++) {
		CButton * opButton = opButtons.at(i);
		if (opButton && opButton->m_hWnd == hwnd) {
			nIndex = i;
			break;
		}
	}
	if (nIndex == -1) {
		return;
	}
	CButton * opButton = opButtons.at(nIndex);
	CString opText;
	opButton->GetWindowText(opText);
	if (opText == L"+") {
		initWindowRect(n);
		CRect clientRect;
		GetClientRect(clientRect);
		createOrShowRowElems(n, FilterTuple(), clientRect);
	} else {
		removeRowElems(nIndex);
	}

}

