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

 * @file   CopyTableDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "CopyTableDialog.h"
#include "utils/FontUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"
#include "utils/SqlUtil.h"

CopyTableDialog::CopyTableDialog(HWND parentHwnd, bool isSharding)
{
	setFormSize(COPY_DIALOG_WIDTH, COPY_DIALOG_HEIGHT);
	
	caption = isSharding ? S(L"table-sharding-as") : S(L"table-copy-as");	
	this->isSharding = isSharding;
	this->parentHwnd = parentHwnd;
}

void CopyTableDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	
	createOrShowCopyImage(copyImage, clientRect);
	createOrShowCopyTextLabel(copyTextLabel, clientRect);
	createFromElems(clientRect);
	createToElems(clientRect);

	createOrShowGroupBoxes(clientRect);
	createOrShowStructureAndDataSettingsElems(clientRect);
	createOrShowShardingTableElems(clientRect);
	createOrShowAdvancedOptionsElems(clientRect);
	// process bar 
	createOrShowProcessBar(processBar, clientRect);
	createOrShowPreviewSqlButton(clientRect);
}


void CopyTableDialog::createOrShowCopyImage(QStaticImage &win, CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"database\\dialog\\copy-table.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void CopyTableDialog::createOrShowCopyTextLabel(CStatic &win, CRect & clientRect)
{
	std::wstring text = S(L"copy-table-text-description");
	std::wstring fmt = Lang::fontName();
	int fsize = Lang::fontSize(L"form-text-size");
	CSize size = FontUtil::measureTextSize(text.c_str(), fsize, false,  fmt.c_str());
	int x = 20 + 32 + 20, y = 25, w = clientRect.Width() - x - 20, h = size.cy + 2;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(win, text, rect, clientRect, SS_LEFT, elemFont);
}

void CopyTableDialog::createFromElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 20, w = 170, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(fromDbLabel, S(L"from-db-name"), rect, clientRect, SS_LEFT); 

	UserDb userDb = databaseService->getUserDb(databaseSupplier->getSelectedUserDbId());
	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(fromDbEdit, Config::COPYTABLE_FROM_DB_EDIT_ID, userDb.name, L"", rect, clientRect, ES_LEFT, true);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(fromTblLabel, S(L"from-tbl-name"), rect, clientRect, SS_LEFT);  

	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(fromTblEdit, Config::COPYTABLE_FROM_TBL_EDIT_ID, databaseSupplier->selectedTable, L"", rect, clientRect, ES_LEFT, true);
}


void CopyTableDialog::createToElems(CRect & clientRect)
{
	int x = clientRect.right - 30 - 170, y = 15 + 32 + 20, w = 170, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(toDbLabel, S(L"to-db-name"), rect, clientRect, SS_LEFT); 

	UserDb userDb = databaseService->getUserDb(databaseSupplier->getSelectedUserDbId());
	rect.OffsetRect(0, h + 3);
	createOrShowFormComboBox(toDbComboBox, Config::COPYTABLE_TO_DB_COMBOBOX_ID, userDb.name, rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(toTblLabel, S(L"to-tbl-name"), rect, clientRect, SS_LEFT); 

	std::wstring toTblName = databaseSupplier->selectedTable + L"_1";
	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(toTblEdit, Config::COPYTABLE_TO_TBL_EDIT_ID, toTblName, L"", rect, clientRect, ES_LEFT, false);
}


void CopyTableDialog::createOrShowGroupBoxes(CRect & clientRect)
{
	int x = 20, y = 175, w = 170, h = 130;

	CRect rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(structAndDataGroupBox, Config::EXPORT_AS_SQL_STRUCTURE_AND_DATA_GROUPBOX_ID, S(L"structure-and-data-settings"), rect, clientRect);

	x = clientRect.right - 30 - 170;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(tableShardingGroupBox, 0, S(L"table-sharding-setttings"), rect, clientRect);

	x = 20, y = rect.bottom + 10, w = clientRect.right - 50, h = 80;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(advancedOptionsGroupBox, 0, S(L"sharding-table-advanced-settings"), rect, clientRect);
}

void CopyTableDialog::createOrShowStructureAndDataSettingsElems(CRect & clientRect)
{	
	int x = 30, y = 195, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormRadio(structureOnlyRadio, Config::STRUCTURE_ONLY_RADIO_ID, S(L"structure-only"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(dataOnlyRadio, Config::DATA_ONLY_RADIO_ID, S(L"data-only"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(structureAndDataRadio, Config::STRUCTURE_DATA_RADIO_ID, S(L"structure-and-data"), rect, clientRect);
}


void CopyTableDialog::createOrShowShardingTableElems(CRect & clientRect)
{
	int x = clientRect.right - 30 - 160, y = 195, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormCheckBox(shardingEnableCheckBox, Config::COPYTABLE_SHARDING_ENABLE_CHECKBOX_ID, S(L"enable-table-sharding"), rect, clientRect);

	CRect rect1 = rect;
	rect1.OffsetRect(10, h + 5);
	rect1 = { rect1.left, rect1.top, rect1.left + 80, rect1.bottom };
	std::wstring tblSuffixBegin = S(L"table-suffix-begin").append(L":");
	tblSuffixBegin = StringUtil::replace(tblSuffixBegin, L"{suffix}", L"n");
	createOrShowFormLabel(shardingBeginLabel, tblSuffixBegin, rect1, clientRect);

	CRect rect2 = rect1;
	rect2.OffsetRect(80 + 5, 0);	
	rect2.right = rect2.left + 60;
	createOrShowFormEdit(shardingBeginEdit, Config::COPYTABLE_SHARDING_BEGIN_EDIT_ID, L"1", L"", rect2, clientRect, ES_LEFT | ES_NUMBER, false);
	
	rect1.OffsetRect(0, h + 5);
	std::wstring tblSuffixEnd = S(L"table-suffix-end").append(L":");
	tblSuffixEnd = StringUtil::replace(tblSuffixEnd, L"{suffix}", L"n");
	createOrShowFormLabel(shardingEndLabel, tblSuffixEnd, rect1, clientRect);

	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(shardingEndEdit, Config::COPYTABLE_SHARDING_END_EDIT_ID, L"64", L"", rect2, clientRect, ES_LEFT | ES_NUMBER, false);

	std::wstring tblNameRange = databaseSupplier->selectedTable + L"_1 - " + databaseSupplier->selectedTable + L"_64";
	rect.OffsetRect(0, 80);
	createOrShowFormEdit(shardingRangeEdit, Config::COPYTABLE_SHARDING_RANGE_EDIT_ID, tblNameRange, L"", rect, clientRect, ES_LEFT |ES_AUTOHSCROLL, true);
	shardingRangeEdit.SetLimitText(1024);
}


void CopyTableDialog::createOrShowAdvancedOptionsElems(CRect & clientRect)
{
	CRect rc;
	advancedOptionsGroupBox.GetWindowRect(rc);
	ScreenToClient(&rc);

	int x = 30, y = rc.top + 20, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormCheckBox(dataStrategyEnableCheckBox, Config::COPYTABLE_DATA_STRATEGY_ENABLE_CHECKBOX_ID, S(L"enable-copy-data-with-strategy"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	rect.right = rect.left + 150;
	createOrShowFormLabel(suffixNumberExpressLabel, S(L"suffix-number-express-text"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(rect.Width() + 2, 0);
	createOrShowFormEdit(suffixNumberExpressEdit, Config::COPYTABLE_SUFFIX_NUMBER_EXPRESS_EDIT_ID, L"", L"such as : id % 64 + 1", rect, clientRect, ES_LEFT |ES_AUTOHSCROLL, false);

	rect.OffsetRect(rect.Width() + 5, 0);
	rect.right = rect.left + 20;
	createOrShowFormButton(suffixNumberExpressButton, Config::COPYTABLE_SUFFIX_NUMBER_EXPRESS_BUTTON_ID, L"...", rect, clientRect);

	enableAdvancedSettingsElems();

}

void CopyTableDialog::createOrShowPreviewSqlButton(CRect & clientRect)
{
	CRect rcBtn = GdiPlusUtil::GetWindowRelativeRect(yesButton.m_hWnd);
	int x = 20, y = rcBtn.top, w = rcBtn.Width(), h = rcBtn.Height();
	CRect rect(x, y, x + w, y + h);
	createOrShowFormButton(previewSqlButton, Config::COPYTABLE_PREVIEW_SQL_BUTTON_ID, S(L"preview-sql"), rect, clientRect);
}

void CopyTableDialog::createOrShowProcessBar(QProcessBar &win, CRect & clientRect)
{
	CRect formRect = getFormRect(clientRect);
	int x = formRect.left + 20, y = formRect.bottom - 80, w = formRect.Width() - 40, h = 20;
	CRect rect(x, y, x + w, y + h);

	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		UINT nID = 0;
		win.Create(m_hWnd, rect, L"", dwStyle , 0,  nID);
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void CopyTableDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	
	loadToDbComboBox();
	loadOnlyForIsSharding(); // only for isSharding == true
	loadShardingTableElems();

	enableShardingTableElems();
	enableAdvancedSettingsElems();

	toTblEdit.SetSel(0, -1);
	toTblEdit.SetFocus();
}


void CopyTableDialog::loadToDbComboBox()
{
	// db list
	UserDbList dbs = adapter->getDbs();
	toDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = toDbComboBox.AddString(item.name.c_str());
		toDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
		}
	}
	toDbComboBox.SetCurSel(nSelItem);
}

/**
 * Only load elem settings for isSharding is true.
 * 
 */
void CopyTableDialog::loadOnlyForIsSharding()
{
	if (!isSharding) {
		return;
	}
	// 1. if isSharding==true, checked structureAndDataRadio(checkState = true)
	structureAndDataRadio.SetCheck(1);

	// 2. if isSharding==true, checked shardingEnableCheckBox(checkState = true)
	shardingEnableCheckBox.SetCheck(1);

	// 3. if isSharding==true, checked dataStrategyEnableCheckBox(checkState = true)
	dataStrategyEnableCheckBox.SetCheck(1);

	// 4. if isSharding==true, set suffixNumberExpressEdit.defaultExpress = ([tbl.column : PrimaryKey] % n + 1)
	CString suffixBeginText, suffixEndText;	
	shardingBeginEdit.GetWindowText(suffixBeginText);
	shardingEndEdit.GetWindowText(suffixEndText);
	int suffixBegin = suffixBeginText.IsEmpty() ? 1 : std::stoi(suffixBeginText.GetString());
	int suffixEnd = suffixEndText.IsEmpty() ? 64 : std::stoi(suffixEndText.GetString());

	std::wstring defaultExpress = adapter->getDefaultShardingStrategyExpress(suffixBegin, suffixEnd);
	suffixNumberExpressEdit.SetWindowText(defaultExpress.c_str());
}

void CopyTableDialog::loadShardingTableElems()
{
	bool newChecked = shardingEnableCheckBox.GetCheck();
	if (newChecked) {
		CString str;
		toTblEdit.GetWindowText(str);
		if (str.IsEmpty()) {
			str = databaseSupplier->selectedTable.c_str();
		}
		std::wstring tblName = str.GetString();
		tblName = SqlUtil::replaceNumOfSuffixInTblName(tblName, L"{n}");
		toTblEdit.SetWindowText(tblName.c_str());
		return;
	}

	CString str;
	toTblEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		str = databaseSupplier->selectedTable.c_str();
		str.Append(L"_1");
	}
	std::wstring tblName = str.GetString();
	tblName = StringUtil::replace(tblName, L"{n}", L"1");
	toTblEdit.SetWindowText(tblName.c_str());
}

void CopyTableDialog::enableShardingTableElems()
{
	bool checked = shardingEnableCheckBox.GetCheck();
	shardingBeginLabel.EnableWindow(checked);
	shardingBeginEdit.EnableWindow(checked);
	shardingEndLabel.EnableWindow(checked);
	shardingEndEdit.EnableWindow(checked);
	shardingRangeEdit.EnableWindow(checked);
}


void CopyTableDialog::enableAdvancedSettingsElems()
{
	bool checked1 = dataOnlyRadio.GetCheck() || structureAndDataRadio.GetCheck();

	bool checked2 = shardingEnableCheckBox.GetCheck();
	dataStrategyEnableCheckBox.EnableWindow(checked1  && checked2);
	advancedOptionsGroupBox.EnableWindow(checked1  && checked2);

	bool checked3 = dataStrategyEnableCheckBox.GetCheck();
	suffixNumberExpressLabel.EnableWindow(checked1 && checked2 && checked3);
	suffixNumberExpressEdit.EnableWindow(checked1 && checked2 && checked3);
	suffixNumberExpressButton.EnableWindow(checked1 && checked2 && checked3);
}

LRESULT CopyTableDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	elemFont = FT(L"elem-size");

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_IMPORT_DB_FROM_SQL_PROCESS_ID);

	structureAndDataRadioPtrs.push_back(&structureOnlyRadio);
	structureAndDataRadioPtrs.push_back(&dataOnlyRadio);
	structureAndDataRadioPtrs.push_back(&structureAndDataRadio);

	supplier = new CopyTableSupplier(databaseSupplier);
	adapter = new CopyTableAdapter(m_hWnd, supplier);
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT CopyTableDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (!linePen.IsNull()) linePen.DeleteObject();
	if (elemFont) ::DeleteObject(elemFont);

	if (copyImage.IsWindow()) copyImage.DestroyWindow();
	if (copyTextLabel.IsWindow()) copyTextLabel.DestroyWindow();

	if (fromDbLabel.IsWindow()) fromDbLabel.DestroyWindow();
	if (fromDbEdit.IsWindow()) fromDbEdit.DestroyWindow();
	if (fromTblLabel.IsWindow()) fromTblLabel.DestroyWindow();
	if (fromTblEdit.IsWindow()) fromTblEdit.DestroyWindow();

	if (toDbLabel.IsWindow()) toDbLabel.DestroyWindow();
	if (toDbComboBox.IsWindow()) toDbComboBox.DestroyWindow();
	if (toTblLabel.IsWindow()) toTblLabel.DestroyWindow();
	if (toTblEdit.IsWindow()) toTblEdit.DestroyWindow();

	if (structAndDataGroupBox.IsWindow()) structAndDataGroupBox.DestroyWindow();
	if (tableShardingGroupBox.IsWindow()) tableShardingGroupBox.DestroyWindow();
	if (advancedOptionsGroupBox.IsWindow()) advancedOptionsGroupBox.DestroyWindow();

	if (structureOnlyRadio.IsWindow()) structureOnlyRadio.DestroyWindow();
	if (dataOnlyRadio.IsWindow()) dataOnlyRadio.DestroyWindow();
	if (structureAndDataRadio.IsWindow()) structureAndDataRadio.DestroyWindow();
	
	if (shardingEnableCheckBox.IsWindow()) shardingEnableCheckBox.DestroyWindow();
	if (shardingBeginLabel.IsWindow()) shardingBeginLabel.DestroyWindow();
	if (shardingBeginEdit.IsWindow()) shardingBeginEdit.DestroyWindow();
	if (shardingEndLabel.IsWindow()) shardingEndLabel.DestroyWindow();
	if (shardingEndEdit.IsWindow()) shardingEndEdit.DestroyWindow();
	if (shardingRangeEdit.IsWindow()) shardingRangeEdit.DestroyWindow();

	if (dataStrategyEnableCheckBox.IsWindow()) dataStrategyEnableCheckBox.DestroyWindow();
	if (suffixNumberExpressLabel.IsWindow()) suffixNumberExpressLabel.DestroyWindow();
	if (suffixNumberExpressEdit.IsWindow()) suffixNumberExpressEdit.DestroyWindow();
	if (suffixNumberExpressButton.IsWindow()) suffixNumberExpressButton.DestroyWindow();

	if (processBar.IsWindow()) processBar.DestroyWindow();
	if (previewSqlButton.IsWindow()) previewSqlButton.DestroyWindow();

	structureAndDataRadioPtrs.clear();
	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}

	if (supplier) {
		delete supplier;
		supplier = nullptr;
	}
	return 0;
}

LRESULT CopyTableDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}


void CopyTableDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}


HBRUSH CopyTableDialog::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	if (hwnd == shardingRangeEdit.m_hWnd) {
		::SetTextColor(hdc, RGB(0, 0, 255)); //文本区域前景色
		::SetBkColor(hdc, bkgColor); //文本区域背景色
		// 字体
		::SelectObject(hdc, textFont);
		return bkgBrush;
	}
	return QDialog::OnCtlColorStatic(hdc, hwnd);
}

void CopyTableDialog::OnClickStructureAndDataRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND selHwnd = nullptr;
	std::for_each(structureAndDataRadioPtrs.begin(), structureAndDataRadioPtrs.end(), [&hwnd, &selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->m_hWnd == hwnd) {
			if (!checked) {
				ptr->SetCheck(!checked);
			}
			selHwnd = hwnd;
		}else {
			ptr->SetCheck(0);
		}
	});
	enableAdvancedSettingsElems();
}


void CopyTableDialog::OnClickShardingEnableCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	bool newChecked = !shardingEnableCheckBox.GetCheck();
	shardingEnableCheckBox.SetCheck(newChecked);

	loadShardingTableElems();

	enableShardingTableElems();
	enableAdvancedSettingsElems();
}


void CopyTableDialog::OnClickDataStrategyEnableCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	bool newChecked = !dataStrategyEnableCheckBox.GetCheck();
	dataStrategyEnableCheckBox.SetCheck(newChecked);
	enableAdvancedSettingsElems();
}

void CopyTableDialog::OnShardingEditChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString toTblText, shardingBeginText, shardingEndText;
	if (!toTblEdit.IsWindow() || !shardingBeginEdit.IsWindow() 
		|| !shardingEndEdit.IsWindow() || !shardingEnableCheckBox.IsWindow()) {
		return ;
	}
	if (!shardingEnableCheckBox.GetCheck()) {
		return ;
	}
	
	toTblEdit.GetWindowText(toTblText);
	shardingBeginEdit.GetWindowText(shardingBeginText);
	shardingEndEdit.GetWindowText(shardingEndText);

	if (toTblText.IsEmpty() || shardingBeginText.IsEmpty() || shardingEndText.IsEmpty()) {
		shardingRangeEdit.Clear();
		return;
	}
	std::wstring toTblName = toTblText.GetString();

	if (toTblName.find(L"{n}") == std::wstring::npos) {
		shardingRangeEdit.Clear();
		return;
	}
	std::wstring shardingBeginName = StringUtil::replace(toTblName, L"{n}", shardingBeginText.GetString());
	std::wstring shardingEndName = StringUtil::replace(toTblName, L"{n}", shardingEndText.GetString());
	std::wstring rangeEditText = shardingBeginName + L" - " + shardingEndName;
	shardingRangeEdit.SetWindowText(rangeEditText.c_str());
}


LRESULT CopyTableDialog::OnProcessCopy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Q_DEBUG(L"recieve MSG_COPY_TABLE_PROCESS_ID, isCompete:{},percent:{}", wParam, lParam);

	if (wParam == 1) { // 1 表示执行完成
		AppContext * appContext = AppContext::getInstance();
		processBar.run(100);
		isRunning = false;
	} else if (wParam == 0) { 
		// 进度条
		int percent = static_cast<int>(lParam);
		processBar.run(percent);
	} else if (wParam == 2) {
		QPopAnimate::error(m_hWnd, S(L"copy-table-error-text"));
		isRunning = false;
	} else if (wParam == 3) {
		
		isRunning = false;
	}
	return 0;
}

void CopyTableDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	fromDbEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(S(L"table-name-not-empty"));
		fromDbEdit.SetFocus();
		return;
	}
	std::wstring & tblName = databaseSupplier->selectedTable;
	std::wstring copyTblName = str.GetString();
	if (tblName == copyTblName) {
		fromDbEdit.SetFocus();
		fromDbEdit.SetSel(0, -1, FALSE);
		return;
	}
	auto tableInfo = tableService->getUserTable(databaseSupplier->getSelectedUserDbId(), copyTblName);
	if (!tableInfo.name.empty()) {
		QPopAnimate::error(S(L"table-name-duplicated"));
		fromDbEdit.SetFocus();
		fromDbEdit.SetSel(0, -1, FALSE);
		return;
	}


	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}


