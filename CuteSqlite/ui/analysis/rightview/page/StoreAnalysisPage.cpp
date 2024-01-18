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

 * @file   StoreAnalysisPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-03
 *********************************************************************/
#include "stdafx.h"
#include "StoreAnalysisPage.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "common/AppContext.h"
#include "utils/ColumnsUtil.h"
#include "utils/SqlUtil.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"


BOOL StoreAnalysisPage::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

StoreAnalysisPage::StoreAnalysisPage(uint64_t userDbId)
{
	supplier.setRuntimeUserDbId(userDbId);
	supplier.setUserDb(databaseService->getUserDb(userDbId));
}


uint64_t StoreAnalysisPage::getUserDbId()
{
	return supplier.getRuntimeUserDbId();
}


void StoreAnalysisPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	nHeightSum = 0;

	createOrShowTitleElems(clientRect);
	createOrShowStoreAnalysisElems(clientRect);

	// onSize will trigger init the v-scrollbar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void StoreAnalysisPage::createOrShowTitleElems(CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	createOrShowImage(titleImage, rect, clientRect);

	rect.OffsetRect(w + 10, 5);
	rect.right = rect.left + 300;
	
	std::wstring title = S(L"store-analysis-text");
	createOrShowEdit(titleEdit, 0, title, rect, clientRect, 0);

	rect.OffsetRect(300 + 10, 0);
	rect.right = rect.left + 60;
	QWinCreater::createOrShowLabel(m_hWnd, databaseLabel, S(L"database").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(60 + 5, 0);
	rect.right = rect.left + 120;
	title = supplier.getUserDb().name;
	createOrShowEdit(databaseEdit, 0, title, rect, clientRect, 0);

	rect.OffsetRect(120 + 5, 0);
	rect.right = rect.left + 60;
	QWinCreater::createOrShowLabel(m_hWnd, tableLabel, S(L"table").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(60 + 5, -2);
	rect.right = rect.left + 150;
	QWinCreater::createOrShowComboBox(m_hWnd, tableComboBox, Config::ANALYSIS_DB_STORE_TBL_COMBOBOX_ID, rect, clientRect, SS_RIGHT);

	rect.left = clientRect.Width() - 20 - 60;
	rect.right = rect.left + 60;
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, S(L"refresh-page"), rect, clientRect);
	nHeightSum = rect.bottom;
}


void StoreAnalysisPage::createOrShowStoreAnalysisElems(CRect & clientRect)
{
	createOrShowDatabaseStoreAnalysisElems(clientRect);
	createOrShowTableStoreAnalysisElems(clientRect);

	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(storeAnalysisElemPtrs.back()->m_hWnd);
	nHeightSum = rcLast.bottom + 200;
}


void StoreAnalysisPage::createOrShowTableStoreAnalysisElems(CRect & clientRect)
{
	std::wstring selTable = getSelectTable();
	if (selTable.empty() || selTable == L"All") {
		return;
	}
	std::wstring title;
	int itemsLen = 0;
	CRect rect;
	if (!storeAnalysisElemPtrs.empty()) {
		rect = GdiPlusUtil::GetWindowRelativeRect(storeAnalysisElemPtrs.back()->m_hWnd);
	} else {
		CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(titleEdit.m_hWnd);
		int x = 20, y = rcLast.bottom + 25, w = clientRect.Width() - 20 * 2, h = (20 + 5);
		rect = { x, y, x + w, y + h };
	}
	// 7.Foreach table names to show the table report
	auto & tblNames = storeAnalysisService->getTableNames(supplier.getRuntimeUserDbId());
	for (auto & tblName : tblNames) {
		if (tblName != selTable) {
			continue;
		}
		int n = storeAnalysisService->getSpaceUsedCountByTblName(supplier.getRuntimeUserDbId(), tblName);
		if (n > 0) {
			// Table {tblName} and all its indices
			title = S(L"tbl-and-all-idx-report-title");
			title = StringUtil::replace(title, L"{tblName}", tblName);
			StoreAnalysisElem * ptr7 = getStoreAnalysisElemPtr(title);
			if (!ptr7) {
				StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfTblIdxReport(supplier.getRuntimeUserDbId(), tblName);
				itemsLen = static_cast<int>(storeItems.size()) + 1;
				ptr7 = new StoreAnalysisElem(title, storeItems);
				storeAnalysisElemPtrs.push_back(ptr7);
			}
			else {
				const StoreAnalysisItems & storeItems = ptr7->getStoreAnalysisItems();
				itemsLen = static_cast<int>(storeItems.size()) + 1;
			}
			rect.OffsetRect(0, rect.Height() + 10);
			rect.bottom = rect.top + (20 + 5) * itemsLen;
			createOrShowStoreAnalysisElem(*ptr7, rect, clientRect);

			// Table $name w/o any indices
			title = S(L"tbl-only-report-title");
			title = StringUtil::replace(title, L"{tblName}", tblName);
			StoreAnalysisElem * ptr8 = getStoreAnalysisElemPtr(title);
			if (!ptr8) {
				StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfTblOnlyReport(supplier.getRuntimeUserDbId(), tblName, true);
				itemsLen = static_cast<int>(storeItems.size()) + 1;
				ptr8 = new StoreAnalysisElem(title, storeItems);
				storeAnalysisElemPtrs.push_back(ptr8);
			}
			else {
				const StoreAnalysisItems & storeItems = ptr8->getStoreAnalysisItems();
				itemsLen = static_cast<int>(storeItems.size()) + 1;
			}
			rect.OffsetRect(0, rect.Height() + 10);
			rect.bottom = rect.top + (20 + 5) * itemsLen;
			createOrShowStoreAnalysisElem(*ptr8, rect, clientRect);

			auto idxList = storeAnalysisService->getIndexNamesByTblName(supplier.getRuntimeUserDbId(), tblName);
			if (idxList.size() > 1) {
				// Indices of table $name
				title = S(L"idx-only-report-title");
				title = StringUtil::replace(title, L"{tblName}", tblName);
				StoreAnalysisElem * ptr9 = getStoreAnalysisElemPtr(title);
				if (!ptr9) {
					StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfIdxOnlyReport(supplier.getRuntimeUserDbId(), tblName);
					itemsLen = static_cast<int>(storeItems.size()) + 1;
					ptr9 = new StoreAnalysisElem(title, storeItems);
					storeAnalysisElemPtrs.push_back(ptr9);
				}
				else {
					const StoreAnalysisItems & storeItems = ptr9->getStoreAnalysisItems();
					itemsLen = static_cast<int>(storeItems.size()) + 1;
				}
				rect.OffsetRect(0, rect.Height() + 10);
				rect.bottom = rect.top + (20 + 5) * itemsLen;
				createOrShowStoreAnalysisElem(*ptr9, rect, clientRect);
			}

			for (auto & idxName : idxList) {
				// Index {idxName} of table {tblName}
				title = S(L"idx-report-title");
				title = StringUtil::replace(title, L"{idxName}", idxName);
				title = StringUtil::replace(title, L"{tblName}", tblName);
				StoreAnalysisElem * ptr10 = getStoreAnalysisElemPtr(title);
				if (!ptr10) {
					StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfIdxReport(supplier.getRuntimeUserDbId(), tblName, idxName);
					itemsLen = static_cast<int>(storeItems.size()) + 1;
					ptr10 = new StoreAnalysisElem(title, storeItems);
					storeAnalysisElemPtrs.push_back(ptr10);
				}
				else {
					const StoreAnalysisItems & storeItems = ptr10->getStoreAnalysisItems();
					itemsLen = static_cast<int>(storeItems.size()) + 1;
				}
				rect.OffsetRect(0, rect.Height() + 10);
				rect.bottom = rect.top + (20 + 5) * itemsLen;
				createOrShowStoreAnalysisElem(*ptr10, rect, clientRect);
			}
		} else {
			// 
			title = S(L"tbl-report-title");
			title = StringUtil::replace(title, L"{tblName}", tblName);
			StoreAnalysisElem * ptr11 = getStoreAnalysisElemPtr(title);
			if (!ptr11) {
				StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfTblReport(supplier.getRuntimeUserDbId(), tblName);
				itemsLen = static_cast<int>(storeItems.size()) + 1;
				ptr11 = new StoreAnalysisElem(title, storeItems);
				storeAnalysisElemPtrs.push_back(ptr11);
			}
			else {
				const StoreAnalysisItems & storeItems = ptr11->getStoreAnalysisItems();
				itemsLen = static_cast<int>(storeItems.size()) + 1;
			}
			rect.OffsetRect(0, rect.Height() + 10);
			rect.bottom = rect.top + (20 + 5) * itemsLen;
			createOrShowStoreAnalysisElem(*ptr11, rect, clientRect);
		}
	}
}

void StoreAnalysisPage::createOrShowDatabaseStoreAnalysisElems(CRect &clientRect)
{
	std::wstring selTable = getSelectTable();
	if (!selTable.empty() && selTable != L"All") {
		return;
	}
	
	UserDb & userDb = supplier.getUserDb();

	// 1.Disk-Space Utilization Report For {dbPath}
	std::wstring title = S(L"db-disk-used-title");
	title = StringUtil::replace(title, L"{dbPath}", userDb.path);
	StoreAnalysisElem * ptr0 = getStoreAnalysisElemPtr(title);
	int itemsLen = 0;
	if (!ptr0) {
		StoreAnalysisItems dbStoreAnalysisItems = adapter->getStoreAnalysisItemsOfDbDiskUsed();
		itemsLen = static_cast<int>(dbStoreAnalysisItems.size()) + 1;
		ptr0 = new StoreAnalysisElem(title, dbStoreAnalysisItems);
		storeAnalysisElemPtrs.push_back(ptr0);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr0->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}

	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(titleEdit.m_hWnd);
	int x = 20, y = rcLast.bottom + 50, w = clientRect.Width() - 20 * 2, h = (20 + 5) * itemsLen;
	CRect rect(x, y, x + w, y + h);
	createOrShowStoreAnalysisElem(*ptr0, rect, clientRect);

	// 2.Page counts for all tables with their indices
	title = S(L"all-tbl-entrycnt-title");
	StoreAnalysisElem * ptr1 = getStoreAnalysisElemPtr(title);
	if (!ptr1) {
		StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfAllTblEntries();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
		ptr1 = new StoreAnalysisElem(title, storeItems);
		storeAnalysisElemPtrs.push_back(ptr1);
	} else {
		const StoreAnalysisItems & storeItems = ptr1->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr1, rect, clientRect);

	// 2.Page counts for all tables with their indices
	title = S(L"all-tbl-idx-pagecnt-title");
	StoreAnalysisElem * ptr2 = getStoreAnalysisElemPtr(title);
	if (!ptr2) {
		StoreAnalysisItems tblIdxPgCntStoreAnalysisItems = adapter->getStoreAnalysisItemsOfAllTblIdxPageCnt();
		itemsLen = static_cast<int>(tblIdxPgCntStoreAnalysisItems.size()) + 1;
		ptr2 = new StoreAnalysisElem(title, tblIdxPgCntStoreAnalysisItems);
		storeAnalysisElemPtrs.push_back(ptr2);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr2->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr2, rect, clientRect);

	// 3.Page counts for all tables and indices separately
	title = S(L"sperate-tbl-idx-pagecnt-title");
	StoreAnalysisElem * ptr3 = getStoreAnalysisElemPtr(title);
	if (!ptr3) {
		StoreAnalysisItems seperateStoreItems = adapter->getStoreAnalysisItemsOfSeperateTblIdxPageCnt();
		itemsLen = static_cast<int>(seperateStoreItems.size()) + 1;
		ptr3 = new StoreAnalysisElem(title, seperateStoreItems);
		storeAnalysisElemPtrs.push_back(ptr3);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr3->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr3, rect, clientRect);

	// 4.All tables and indices
	title = S(L"all-tbl-idx-report-title");
	StoreAnalysisElem * ptr4 = getStoreAnalysisElemPtr(title);
	if (!ptr4) {
		StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfAllTblIdxReport();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
		ptr4 = new StoreAnalysisElem(title, storeItems);
		storeAnalysisElemPtrs.push_back(ptr4);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr4->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr4, rect, clientRect);

	// 5.All tables report
	title = S(L"all-table-report-title");
	StoreAnalysisElem * ptr5 = getStoreAnalysisElemPtr(title);
	if (!ptr5) {
		StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfAllTblReport();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
		ptr5 = new StoreAnalysisElem(title, storeItems);
		storeAnalysisElemPtrs.push_back(ptr5);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr5->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr5, rect, clientRect);

	// 6.All indices report
	title = S(L"all-indices-report-title");
	StoreAnalysisElem * ptr6 = getStoreAnalysisElemPtr(title);
	if (!ptr6) {
		StoreAnalysisItems storeItems = adapter->getStoreAnalysisItemsOfAllIdxReport();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
		ptr6 = new StoreAnalysisElem(title, storeItems);
		storeAnalysisElemPtrs.push_back(ptr6);
	}
	else {
		const StoreAnalysisItems & storeItems = ptr6->getStoreAnalysisItems();
		itemsLen = static_cast<int>(storeItems.size()) + 1;
	}
	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + (20 + 5) * itemsLen;
	createOrShowStoreAnalysisElem(*ptr6, rect, clientRect);
}

void StoreAnalysisPage::createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"analysis\\page\\store-analysis.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void StoreAnalysisPage::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetReadOnly(TRUE);
		win.SetWindowText(text.c_str());

		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		win.SetWindowText(text.c_str());
	}
}


void StoreAnalysisPage::createOrShowStoreAnalysisElem(StoreAnalysisElem &win, CRect & rect, CRect clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		win.Create(m_hWnd, rect);
		return;
	} else if (::IsWindow(m_hWnd) && win.IsWindow() && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void StoreAnalysisPage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;
	loadTableComboBox();
}


void StoreAnalysisPage::loadTableComboBox()
{
	tableComboBox.ResetContent();
	tableComboBox.AddString(L"All");

	uint64_t userDbId = supplier.getRuntimeUserDbId();
	UserTableStrings tableStrs ;
	try {
		tableStrs = tableService->getUserTableStrings(userDbId);
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"failed, code:{},msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
	
	for (auto tbl : tableStrs) {
		tableComboBox.AddString(tbl.c_str());
	}
	tableComboBox.SetCurSel(0);
}

int StoreAnalysisPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");
	titleFont = FTB(L"properites-title-font", true);
	sectionFont = FTB(L"properites-section-font", true);
	comboFont = FT(L"combobox-size");

	linePen.CreatePen(PS_SOLID, 1, lineColor);
	headerBrush.CreateSolidBrush(headerBkgColor);

	adapter = new StoreAnalysisPageAdapter(m_hWnd, this, &supplier);

	
	return ret;
}

int StoreAnalysisPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (!titleFont) ::DeleteObject(titleFont);
	if (!textFont) ::DeleteObject(textFont);
	if (!sectionFont) ::DeleteObject(sectionFont); 
	if (!comboFont) ::DeleteObject(comboFont); 

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (!headerBrush.IsNull()) headerBrush.DeleteObject();
	

	if (titleImage.IsWindow()) titleImage.DestroyWindow();
	if (titleEdit.IsWindow()) titleEdit.DestroyWindow();
	if (databaseLabel.IsWindow()) databaseLabel.DestroyWindow();
	if (databaseEdit.IsWindow()) databaseEdit.DestroyWindow();
	if (tableLabel.IsWindow()) tableLabel.DestroyWindow();
	if (tableComboBox.IsWindow()) tableComboBox.DestroyWindow();
	if (refreshButton.IsWindow()) refreshButton.DestroyWindow();
	
	void clearStoreAnalysisElemPtrs();

	if (adapter) delete adapter;
	return ret;
}


void StoreAnalysisPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);
	int nPos = GetScrollPos(SB_VERT);
	int x = 20, y = 15 + 32 + 10 - nPos * cyChar, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);	
}

void StoreAnalysisPage::initScrollBar(CSize & clientSize)
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
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = si.nMax % pageNums ? 
		si.nMax / pageNums + 1 : si.nMax / pageNums;

	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

LRESULT StoreAnalysisPage::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	si.nPos = min(100 - vScrollPages, max(0, si.nPos));
	si.fMask = SIF_POS;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	::GetScrollInfo(m_hWnd, SB_VERT, &si);

	if (iVscrollPos != si.nPos) {
		::ScrollWindow(m_hWnd, 0, (nHeightSum / 100) * (iVscrollPos - si.nPos), nullptr, nullptr);
		Invalidate(true);

		// Invalidate the all of QHorizontalBar window in the all StoreAnalysisElem
		updateSubWindow();
	}
	 return 0;
}


LRESULT StoreAnalysisPage::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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



LRESULT StoreAnalysisPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	nHeightSum = 0;

	CRect clientRect;
	GetClientRect(clientRect);
	
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = 1;
	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

	createOrShowUI();
	Invalidate(true);
	return 0;
}


LRESULT StoreAnalysisPage::OnChangeTableComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	clearStoreAnalysisElemPtrs();
	createOrShowUI();
	return 0;
}

void StoreAnalysisPage::clearStoreAnalysisElemPtrs()
{
	for (auto ptr : storeAnalysisElemPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	storeAnalysisElemPtrs.clear();
}

HBRUSH StoreAnalysisPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);

	return bkgBrush.m_hBrush;
}

HBRUSH StoreAnalysisPage::OnCtlColorBtn(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

HBRUSH StoreAnalysisPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, textColor); // Text area foreground color
	::SetBkColor(hdc, bkgColor); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

/**
 * verified title exists in storeAnalysisElemPtrs.
 * 
 * @param title[in] - title
 * @param ptr[out] - output pointer of StoreAnalysisElem
 * @return 
 */
StoreAnalysisElem * StoreAnalysisPage::getStoreAnalysisElemPtr(const std::wstring & title)
{
	if (title.empty()) {
		return nullptr;
	}

	size_t n = storeAnalysisElemPtrs.size();	
	for (size_t i = 0; i < n; i++) {
		if (storeAnalysisElemPtrs.at(i)->getTitle() == title) {
			return storeAnalysisElemPtrs.at(i);
		}
	}

	return nullptr;
}

/**
 * Invalidate the all of QHorizontalBar window in the all StoreAnalysisElem.
 * 
 */
void StoreAnalysisPage::updateSubWindow()
{
	for (auto ptr : storeAnalysisElemPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->refreshHorizBars();
		}		
	}
}

std::wstring StoreAnalysisPage::getSelectTable()
{
	int nSelItem = tableComboBox.GetCurSel();
	if (nSelItem == -1) {
		return L"All";
	}
	wchar_t cch[1024] = { 0 };
	tableComboBox.GetLBText(nSelItem, cch);
	std::wstring text(cch);
	return text;
}
