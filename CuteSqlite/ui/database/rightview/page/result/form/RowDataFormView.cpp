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

 * @file   RowDataFormView.cpp
 * @brief  Show form view for cliked the one row data in ResultListPage
 * 
 * @author Xuehan Qin
 * @date   2023-06-16
 *********************************************************************/
#include "stdafx.h"
#include "RowDataFormView.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"


int RowDataFormView::cxChar = 5;
int RowDataFormView::cyChar = 5;
int RowDataFormView::iVscrollPos = 5;
int RowDataFormView::vScrollPages = 5;

void RowDataFormView::setup(ResultListPageAdapter * adapter)
{
	this->adapter = adapter;
}


void RowDataFormView::loadFormData(bool readOnly)
{
	this->readOnly = readOnly;
	if (adapter == nullptr) {
		return ;
	}
	
	Columns columns = adapter->getRuntimeColumns();
	DataList selDatas = adapter->getSelectedDatas();
	if (columns.empty() || selDatas.empty()) {
		return ;
	}
	// show the first selected row
	RowItem rowItem = *(selDatas.begin()); 

	if (columns.size() != rowItem.size()) {
		Q_WARN(L"The columns.size is not equal rowItem.size.");
	}
	showColumnsAndValues(columns, rowItem);
}


void RowDataFormView::clearLabelsAndEdits()
{
	if (!labels.empty()) {
		for (auto label : labels) {
			if (label && label->IsWindow()) {
				label->DestroyWindow();
				delete label;
				label = nullptr;
			} else if (label) {
				delete label;
				label = nullptr;
			}
		}
		labels.clear();
	}

	if (!edits.empty()) {
		for (auto edit : edits) {
			if (edit && edit->IsWindow()) {
				edit->DestroyWindow();
				delete edit;
				edit = nullptr;
			} else if (edit) {
				delete edit;
				edit = nullptr;
			}
		}
		edits.clear();
	}
}


void RowDataFormView::setEditText(int editIndex, std::wstring & text)
{
	if (editIndex < 0 || editIndex >= static_cast<int>(edits.size())) {
		return ;
	}
	
	auto ptr = edits.at(editIndex);
	if (!ptr) {
		return;
	}
	CString str;
	ptr->GetWindowText(str);
	if (text == str.GetString()) {
		return;
	}
	ptr->SetWindowText(text.c_str());
}

/**
 * .
 * 
 * @param columns
 * @param rowItem
 */
void RowDataFormView::showColumnsAndValues(Columns & columns, RowItem & rowItem)
{
	ATLASSERT(!columns.empty() && !rowItem.empty() && columns.size() == rowItem.size());
	clearLabelsAndEdits();

	int n = static_cast<int>(columns.size());	
	int pixel = Lang::fontSize(L"form-text-size");
	std::wstring fontName = Lang::fontName();

	CRect clientRect;
	GetClientRect(clientRect);
	
	CRect labelRect(5, 10, 85, 10 + pixel);
	int perCxPixel = clientRect.Width() - 5 - 80 - 5 - 20;
	nHeightSum = 0;
	for (int i = 0; i < n; i++) {
		std::wstring column = columns.at(i);
		std::wstring value = rowItem.at(i);
		CSize textSize = !value.empty() ?  FontUtil::measureTextSize(value.c_str(), pixel, false, fontName.c_str())
			: CSize((long)perCxPixel, (long)pixel);
		int lines = textSize.cx % perCxPixel > 0 ? (textSize.cx / perCxPixel) + 1 : textSize.cx / perCxPixel;
		CStatic * label = new CStatic();
		QWinCreater::createOrShowLabel(m_hWnd, *label, column.append(L":"), labelRect, clientRect, SS_RIGHT, pixel);
		labels.push_back(label);

		int x = labelRect.right + 5, y = labelRect.top, w = perCxPixel, h = (textSize.cy + 5) * lines;
		CRect editRect(x, y, x + w, y + h);
		CEdit * edit = new CEdit();
		QWinCreater::createOrShowEdit(m_hWnd, *edit, Config::FORMVIEW_EDIT_ID_START + i, value, editRect, clientRect, textFont, ES_MULTILINE | ES_AUTOVSCROLL, readOnly);
		edits.push_back(edit);

		x = labelRect.left, y = editRect.bottom + 10, w = 80, h = pixel;
		labelRect = { x, y, x + w, y + h };

		nHeightSum = editRect.bottom;
	}

	// onSize will trigger init the v-scrollbar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void RowDataFormView::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, topbarBrush);
}

int RowDataFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");

	// Init the scrollbar params 
	HDC hdc  = ::GetDC(m_hWnd);
	::GetTextMetrics(hdc, &tm);
	cxChar = tm.tmAveCharWidth;
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	::ReleaseDC(m_hWnd, hdc);
	return 0;
}


int RowDataFormView::OnDestroy()
{
	QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);

	clearLabelsAndEdits();
	return 0;
}


void RowDataFormView::OnSize(UINT nType, CSize size)
{
	QPage::OnSize(nType, size);
	loadFormData(this->readOnly);
}

void RowDataFormView::initScrollBar(CSize & clientSize)
{
	if (clientSize.cx == 0 || clientSize.cy == 0 || nHeightSum == 0) {
		return ;
	}
	int pageNums = nHeightSum % clientSize.cy ? 
		nHeightSum / clientSize.cy + 1 : nHeightSum / clientSize.cy;
	si.cbSize = sizeof(SCROLLINFO);   // setting the scrollbar

	// change 3 values(SIF_RANGE: si.nMin, si.nMax, si.nPage; SIF_PAGE:si.nPage; SIF_POS: si.nPos)
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 50; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = si.nMax % pageNums ? 
		si.nMax / pageNums + 1 : si.nMax / pageNums;

	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

LRESULT RowDataFormView::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = LOWORD(wParam);
	int nPos = GetScrollPos(SB_VERT);
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	::GetScrollInfo(m_hWnd, SB_VERT, &si);
	iVscrollPos = si.nPos;


	switch (LOWORD(wParam))
	{
	case SB_LINEDOWN:
		si.nPos  += 1;
		break;
	case SB_LINEUP:
		si.nPos -= 1;
		break;
	case SB_PAGEDOWN:
		si.nPos += vScrollPages;
		break;
	case SB_PAGEUP:
		si.nPos -= vScrollPages;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos ;
		break;
	}
	si.nPos = min(50 - vScrollPages, max(0, si.nPos));
	si.fMask = SIF_POS;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	::GetScrollInfo(m_hWnd, SB_VERT, &si);

	if (iVscrollPos != si.nPos) {
		::ScrollWindow(m_hWnd, 0, (nHeightSum / 100) * (iVscrollPos - si.nPos), nullptr, nullptr);
		::UpdateWindow(m_hWnd);
	}
	 return 0;
}


LRESULT RowDataFormView::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	
	if (zDelta == 120) { // 正向
		WPARAM newParam = MAKEWPARAM(SB_LINEUP, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}else if (zDelta == -120) { // 反向
		WPARAM newParam = MAKEWPARAM(SB_LINEDOWN, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}
	return 0;
}

LRESULT RowDataFormView::OnEditKillFocus(UINT uMsg, WPARAM wParam, HWND hwnd, BOOL& bHandled)
{
	int n = static_cast<int>(edits.size());
	int nSelItem = -1;
	CString newStr;
	for (int i = 0; i < n; i++) {
		auto ptr = edits.at(i);
		if (ptr->m_hWnd == hwnd) {
			nSelItem = i;
			ptr->GetWindowText(newStr);
			break;
		}
	}
	if (nSelItem == -1) {
		return 0;
	}
	if (!adapter->getSelectedItemCount()) {
		return 0;
	}
	RowItem rowItem = adapter->getFirstSelectdRowItem();
	SubItemValue subItemVal;
	subItemVal.iItem = adapter->getFirstSelectdIndex();
	subItemVal.iSubItem = nSelItem + 1;
	subItemVal.origVal = rowItem.at(nSelItem);
	subItemVal.newVal = newStr.GetString();
	if (subItemVal.origVal == subItemVal.newVal) {
		return 0;
	}

	// update the listview subitem text and runtimedatas
	adapter->addListViewChangeVal(subItemVal);

	// send 
	::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(subItemVal.iItem), LPARAM(subItemVal.iSubItem));
	return 0;
}

HBRUSH RowDataFormView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush;
}

HBRUSH RowDataFormView::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	if (readOnly) {
		::SetBkColor(hdc, topbarColor);
	} else {
		::SetBkColor(hdc, bkgColor);
	}
	
	::SelectObject(hdc, textFont);
	return topbarBrush;
}


