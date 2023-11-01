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
#include "PreviewSqlDialog.h"

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
	createOrShowSourceElems(clientRect);
	createOrShowTargetElems(clientRect);

	createOrShowGroupBoxes(clientRect);
	createOrShowStructureAndDataSettingsElems(clientRect);
	createOrShowTableShardingElems(clientRect);
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

void CopyTableDialog::createOrShowSourceElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 20, w = 170, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(sourceDbLabel, S(L"from-db-name"), rect, clientRect, SS_LEFT); 

	UserDb userDb = databaseService->getUserDb(databaseSupplier->getSelectedUserDbId());
	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(sourceDbEdit, Config::COPYTABLE_FROM_DB_EDIT_ID, userDb.name, L"", rect, clientRect, ES_LEFT, true);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(sourceTblLabel, S(L"from-tbl-name"), rect, clientRect, SS_LEFT);  

	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(sourceTblEdit, Config::COPYTABLE_FROM_TBL_EDIT_ID, databaseSupplier->selectedTable, L"", rect, clientRect, ES_LEFT, true);
}


void CopyTableDialog::createOrShowTargetElems(CRect & clientRect)
{
	int x = clientRect.right - 30 - 170, y = 15 + 32 + 20, w = 170, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(targetDbLabel, S(L"to-db-name"), rect, clientRect, SS_LEFT); 

	UserDb userDb = databaseService->getUserDb(databaseSupplier->getSelectedUserDbId());
	rect.OffsetRect(0, h + 3);
	createOrShowFormComboBox(targetDbComboBox, Config::COPYTABLE_TARGET_DB_COMBOBOX_ID, userDb.name, rect, clientRect);
	
	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(targetTblLabel, S(L"to-tbl-name"), rect, clientRect, SS_LEFT); 

	std::wstring toTblName = databaseSupplier->selectedTable + L"_1";
	rect.OffsetRect(0, h + 3);
	createOrShowFormEdit(targetTblEdit, Config::COPYTABLE_TARGET_TABLE_EDIT_ID, toTblName, L"", rect, clientRect, ES_LEFT, false);
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


void CopyTableDialog::createOrShowTableShardingElems(CRect & clientRect)
{
	int x = clientRect.right - 30 - 160, y = 195, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormCheckBox(enableTableShardingCheckBox, Config::COPYTABLE_ENABLE_TABLE_SHARDING_CHECKBOX_ID, S(L"enable-table-sharding"), rect, clientRect);

	CRect rect1 = rect;
	rect1.OffsetRect(10, h + 5);
	rect1 = { rect1.left, rect1.top, rect1.left + 80, rect1.bottom };
	std::wstring tblSuffixBegin = S(L"table-suffix-begin").append(L":");
	tblSuffixBegin = StringUtil::replace(tblSuffixBegin, L"{suffix}", L"n");
	createOrShowFormLabel(tblSuffixBeginLabel, tblSuffixBegin, rect1, clientRect);

	CRect rect2 = rect1;
	rect2.OffsetRect(80 + 5, 0);	
	rect2.right = rect2.left + 60;
	createOrShowFormEdit(tblSuffixBeginEdit, Config::COPYTABLE_TBL_SUFFER_BEGIN_EDIT_ID, L"1", L"", rect2, clientRect, ES_LEFT | ES_NUMBER, false);
	
	rect1.OffsetRect(0, h + 5);
	std::wstring tblSuffixEnd = S(L"table-suffix-end").append(L":");
	tblSuffixEnd = StringUtil::replace(tblSuffixEnd, L"{suffix}", L"n");
	createOrShowFormLabel(tblSuffixEndLabel, tblSuffixEnd, rect1, clientRect);

	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(tblSuffixEndEdit, Config::COPYTABLE_TBL_SUFFER_END_EDIT_ID, L"64", L"", rect2, clientRect, ES_LEFT | ES_NUMBER, false);

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
	createOrShowFormCheckBox(enableShardingStrategyCheckBox, Config::COPYTABLE_ENABLE_SHARDING_STRATEGY_CHECKBOX_ID, S(L"enable-copy-data-with-strategy"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	rect.right = rect.left + 150;
	createOrShowFormLabel(shardingStrategyExpressLabel, S(L"sharding-strategy-express-text"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(rect.Width() + 2, 0);
	createOrShowFormEdit(shardingStrategyExpressEdit, Config::COPYTABLE_SHARDING_STRATEGY_EXPRESS_EDIT_ID, L"", L"such as : id % 64 + 1", rect, clientRect, ES_LEFT |ES_AUTOHSCROLL, false);

	rect.OffsetRect(rect.Width() + 5, 0);
	rect.right = rect.left + 20;
	createOrShowFormButton(shardingStrategyExpressButton, Config::COPYTABLE_SHARDING_STRATEGY_EXPRESS_BUTTON_ID, L"...", rect, clientRect);

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
	loadAdvancedOptionsElems();

	enableShardingTableElems();
	enableAdvancedSettingsElems();

	targetTblEdit.SetSel(0, -1);
	targetTblEdit.SetFocus();
}


void CopyTableDialog::loadToDbComboBox()
{
	// db list
	UserDbList dbs = adapter->getDbs();
	targetDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = targetDbComboBox.AddString(item.name.c_str());
		targetDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
		}
	}
	targetDbComboBox.SetCurSel(nSelItem);
	supplier->setTargetUserDbId((uint64_t)targetDbComboBox.GetItemData(nSelItem));
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
	supplier->setStructAndDataSetting(STRUCTURE_AND_DATA);
	
	// 2. if isSharding==true, checked shardingEnableCheckBox(checkState = true)
	enableTableShardingCheckBox.SetCheck(1);
	supplier->setEnableTableSharding(1);

	// 3. if isSharding==true, checked dataStrategyEnableCheckBox(checkState = true)
	enableShardingStrategyCheckBox.SetCheck(1);
	supplier->setEnableShardingStrategy(1);

	// 4. if isSharding==true, set suffixNumberExpressEdit.defaultExpress = ([tbl.column : PrimaryKey] % n + 1)
	CString suffixBeginText, suffixEndText;	
	tblSuffixBeginEdit.GetWindowText(suffixBeginText);
	tblSuffixEndEdit.GetWindowText(suffixEndText);
	int suffixBegin = suffixBeginText.IsEmpty() ? 1 : std::stoi(suffixBeginText.GetString());
	int suffixEnd = suffixEndText.IsEmpty() ? 64 : std::stoi(suffixEndText.GetString());
	supplier->setTblSuffixBegin(suffixBegin);
	supplier->setTblSuffixEnd(suffixEnd);
	
	std::wstring defaultExpress = adapter->getDefaultShardingStrategyExpress(suffixBegin, suffixEnd);
	shardingStrategyExpressEdit.SetWindowText(defaultExpress.c_str());
	supplier->setShardingStrategyExpress(defaultExpress);
}

void CopyTableDialog::loadShardingTableElems()
{
	bool newChecked = enableTableShardingCheckBox.GetCheck();
	if (newChecked) {
		CString str;
		targetTblEdit.GetWindowText(str);
		if (str.IsEmpty()) {
			str = databaseSupplier->selectedTable.c_str();
		}
		std::wstring tblName = str.GetString();
		tblName = SqlUtil::replaceNumOfSuffixInTblName(tblName, L"{n}");
		targetTblEdit.SetWindowText(tblName.c_str());
		supplier->setTargetTable(tblName);
		return;
	}

	CString str;
	targetTblEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		str = databaseSupplier->selectedTable.c_str();
		str.Append(L"_1");
	}
	std::wstring tblName = str.GetString();
	tblName = StringUtil::replace(tblName, L"{n}", L"1");
	targetTblEdit.SetWindowText(tblName.c_str());
	supplier->setTargetTable(tblName);
}


void CopyTableDialog::loadAdvancedOptionsElems()
{
	bool checked1 = structureAndDataRadio.GetCheck() || dataOnlyRadio.GetCheck();
	bool checked2 = enableTableShardingCheckBox.GetCheck();
	bool checked3 = enableShardingStrategyCheckBox.GetCheck();
	if (!checked1 || !checked2 || !checked3) {
		return;
	}

	CString str, suffixBeginText, suffixEndText;
	shardingStrategyExpressEdit.GetWindowText(str);
	tblSuffixBeginEdit.GetWindowText(suffixBeginText);
	tblSuffixEndEdit.GetWindowText(suffixEndText);
	shardingStrategyExpressEdit.GetWindowText(str);
	if (!str.IsEmpty() || suffixBeginText.IsEmpty() || suffixEndText.IsEmpty()) {
		return ;
	}
	uint16_t suffixBegin = std::stoi(suffixBeginText.GetString());
	uint16_t suffixEnd = std::stoi(suffixEndText.GetString());
	std::wstring defaultExpress = adapter->getDefaultShardingStrategyExpress(suffixBegin, suffixEnd);
	shardingStrategyExpressEdit.SetWindowText(defaultExpress.c_str());
	supplier->setShardingStrategyExpress(defaultExpress);
}

void CopyTableDialog::enableShardingTableElems()
{
	bool checked = enableTableShardingCheckBox.GetCheck();
	tblSuffixBeginLabel.EnableWindow(checked);
	tblSuffixBeginEdit.EnableWindow(checked);
	tblSuffixEndLabel.EnableWindow(checked);
	tblSuffixEndEdit.EnableWindow(checked);
	shardingRangeEdit.EnableWindow(checked);
}


void CopyTableDialog::enableAdvancedSettingsElems()
{
	bool checked1 = dataOnlyRadio.GetCheck() || structureAndDataRadio.GetCheck();
	bool checked2 = enableTableShardingCheckBox.GetCheck();

	enableShardingStrategyCheckBox.EnableWindow(checked1  && checked2);
	advancedOptionsGroupBox.EnableWindow(checked1  && checked2);

	bool checked3 = enableShardingStrategyCheckBox.GetCheck();
	shardingStrategyExpressLabel.EnableWindow(checked1 && checked2 && checked3);
	shardingStrategyExpressEdit.EnableWindow(checked1 && checked2 && checked3);
	shardingStrategyExpressButton.EnableWindow(checked1 && checked2 && checked3);
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

	if (sourceDbLabel.IsWindow()) sourceDbLabel.DestroyWindow();
	if (sourceDbEdit.IsWindow()) sourceDbEdit.DestroyWindow();
	if (sourceTblLabel.IsWindow()) sourceTblLabel.DestroyWindow();
	if (sourceTblEdit.IsWindow()) sourceTblEdit.DestroyWindow();

	if (targetDbLabel.IsWindow()) targetDbLabel.DestroyWindow();
	if (targetDbComboBox.IsWindow()) targetDbComboBox.DestroyWindow();
	if (targetTblLabel.IsWindow()) targetTblLabel.DestroyWindow();
	if (targetTblEdit.IsWindow()) targetTblEdit.DestroyWindow();

	if (structAndDataGroupBox.IsWindow()) structAndDataGroupBox.DestroyWindow();
	if (tableShardingGroupBox.IsWindow()) tableShardingGroupBox.DestroyWindow();
	if (advancedOptionsGroupBox.IsWindow()) advancedOptionsGroupBox.DestroyWindow();

	if (structureOnlyRadio.IsWindow()) structureOnlyRadio.DestroyWindow();
	if (dataOnlyRadio.IsWindow()) dataOnlyRadio.DestroyWindow();
	if (structureAndDataRadio.IsWindow()) structureAndDataRadio.DestroyWindow();
	
	if (enableTableShardingCheckBox.IsWindow()) enableTableShardingCheckBox.DestroyWindow();
	if (tblSuffixBeginLabel.IsWindow()) tblSuffixBeginLabel.DestroyWindow();
	if (tblSuffixBeginEdit.IsWindow()) tblSuffixBeginEdit.DestroyWindow();
	if (tblSuffixEndLabel.IsWindow()) tblSuffixEndLabel.DestroyWindow();
	if (tblSuffixEndEdit.IsWindow()) tblSuffixEndEdit.DestroyWindow();
	if (shardingRangeEdit.IsWindow()) shardingRangeEdit.DestroyWindow();

	if (enableShardingStrategyCheckBox.IsWindow()) enableShardingStrategyCheckBox.DestroyWindow();
	if (shardingStrategyExpressLabel.IsWindow()) shardingStrategyExpressLabel.DestroyWindow();
	if (shardingStrategyExpressEdit.IsWindow()) shardingStrategyExpressEdit.DestroyWindow();
	if (shardingStrategyExpressButton.IsWindow()) shardingStrategyExpressButton.DestroyWindow();

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
	// sync to supplier
	if (structureOnlyRadio.GetCheck()) supplier->setStructAndDataSetting(STRUCT_ONLY);
	if (dataOnlyRadio.GetCheck()) supplier->setStructAndDataSetting(DATA_ONLY);
	if (structureAndDataRadio.GetCheck()) supplier->setStructAndDataSetting(STRUCTURE_AND_DATA);

	enableAdvancedSettingsElems();
	loadAdvancedOptionsElems();
}


void CopyTableDialog::OnClickEnableTableShardingCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	bool newChecked = !enableTableShardingCheckBox.GetCheck();
	enableTableShardingCheckBox.SetCheck(newChecked);
	supplier->setEnableTableSharding(newChecked);

	loadShardingTableElems();

	enableShardingTableElems();
	enableAdvancedSettingsElems();

	loadAdvancedOptionsElems();
}


void CopyTableDialog::OnClickEnableShardingStrategyCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	bool newChecked = !enableShardingStrategyCheckBox.GetCheck();
	enableShardingStrategyCheckBox.SetCheck(newChecked);
	supplier->setEnableShardingStrategy(newChecked);
	enableAdvancedSettingsElems();
	loadAdvancedOptionsElems();
}

void CopyTableDialog::OnShardingEditChange(UINT uNotifyCode, int nID, HWND hwnd)
{	
	if (!targetTblEdit.IsWindow() || !tblSuffixBeginEdit.IsWindow() 
		|| !tblSuffixEndEdit.IsWindow() || !enableTableShardingCheckBox.IsWindow()) {
		return ;
	}
	
	CString targetTblText, suffixBeginText, suffixEndText;
	targetTblEdit.GetWindowText(targetTblText);
	tblSuffixBeginEdit.GetWindowText(suffixBeginText);
	tblSuffixEndEdit.GetWindowText(suffixEndText);
	//sync supplier
	supplier->setTargetTable(targetTblText.GetString());
	supplier->setTblSuffixBegin(suffixBeginText.IsEmpty() ? 0 : _wtoi(suffixBeginText.GetString()));
	supplier->setTblSuffixEnd(suffixEndText.IsEmpty() ? 0  : _wtoi(suffixEndText.GetString()));

	if (!enableTableShardingCheckBox.GetCheck()) {
		return ;
	}

	// shardingRange edit
	if (targetTblText.IsEmpty() || suffixBeginText.IsEmpty() || suffixEndText.IsEmpty()) {
		shardingRangeEdit.Clear();
		return;
	}
	std::wstring toTblName = targetTblText.GetString();

	if (toTblName.find(L"{n}") == std::wstring::npos) {
		shardingRangeEdit.Clear();
		return;
	}
	std::wstring shardingBeginName = StringUtil::replace(toTblName, L"{n}", suffixBeginText.GetString());
	std::wstring shardingEndName = StringUtil::replace(toTblName, L"{n}", suffixEndText.GetString());
	std::wstring rangeEditText = shardingBeginName + L" - " + shardingEndName;
	shardingRangeEdit.SetWindowText(rangeEditText.c_str());
	
	// Sharding strategy express
	int newCapcity = _wtoi(suffixEndText) - _wtoi(suffixBeginText);

	CString str;
	shardingStrategyExpressEdit.GetWindowText(str);
	std::wstring express = str.GetString();
	int capacity = -1, offset = -1;
	if (newCapcity <=0 || !enableShardingStrategyCheckBox.GetCheck() 
		|| str.IsEmpty() || !adapter->matchDefaultExpress(express, capacity, offset)) {
		return;
	}

	// make new sharding strategy express 
	std::wstring newExpress = adapter->getDefaultShardingStrategyExpress(_wtoi(suffixBeginText), _wtoi(suffixEndText));
	if (!newExpress.empty()) {
		shardingStrategyExpressEdit.SetWindowText(newExpress.c_str());
		supplier->setShardingStrategyExpress(newExpress);
	}	
}


void CopyTableDialog::OnTargetDbComboBoxChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = targetDbComboBox.GetCurSel();
	supplier->setTargetUserDbId(static_cast<uint64_t>(targetDbComboBox.GetItemData(nSelItem)));
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
		processBar.error(L"Error");
		isRunning = false;
	} else if (wParam == 3) {
		
		isRunning = false;
	}
	return 0;
}


void CopyTableDialog::OnClickPreviewSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	PreviewSqlDialog dialog(m_hWnd, adapter);
	dialog.DoModal(m_hWnd);
	return;
}

void CopyTableDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{	
	if (!verifyParams()) {
		return;
	}

	yesButton.EnableWindow(false);
	if (adapter->startCopy()) {
		QPopAnimate::success(S(L"copy-table-success-text"));
	}
	yesButton.EnableWindow(true);
}

/**
 * Verify the params before start to copy table.
 * 
 * @return 
 */
bool CopyTableDialog::verifyParams()
{
	auto tableInfo = tableService->getUserTable(supplier->getTargetUserDbId(), supplier->getTargetTable());
	if (supplier->getStructAndDataSetting() != DATA_ONLY && !tableInfo.name.empty()) {
		QPopAnimate::error(S(L"table-name-duplicated"));
		targetTblEdit.SetFocus();
		targetTblEdit.SetSel(0, -1, FALSE);
		return false;
	}

	if (supplier->getStructAndDataSetting() == UNKOWN) {
		QPopAnimate::error(S(L"structure-and-data-settings-unknown"));
		structureOnlyRadio.SetFocus();
		return false;
	}

	if (supplier->getEnableTableSharding() && supplier->getTargetTable().find(L"{n}") == std::wstring::npos) {
		QPopAnimate::error(S(L"target-table-name-no-n"));
		targetTblEdit.SetFocus();
		targetTblEdit.SetSel(0, -1, FALSE);
		return false;
}

	if (supplier->getEnableTableSharding() && supplier->getTblSuffixBegin() >= supplier->getTblSuffixEnd()) {
		QPopAnimate::error(S(L"table-suffix-error"));
		tblSuffixEndEdit.SetFocus();
		tblSuffixEndEdit.SetSel(0, -1, FALSE);
		return false;
	}

	if (supplier->getEnableTableSharding() && supplier->getEnableShardingStrategy() && supplier->getStructAndDataSetting() != STRUCT_ONLY) {
		if (supplier->getShardingStrategyExpress().empty()) {
			QPopAnimate::error(S(L"sharding-strategy-express-empty"));
			tblSuffixEndEdit.SetFocus();
			tblSuffixEndEdit.SetSel(0, -1, FALSE);
			return false;
		}
	}

	return true;
}


