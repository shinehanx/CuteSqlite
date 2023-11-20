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

 * @file   ImportAsCsvAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-04
 *********************************************************************/
#include "stdafx.h"
#include "ImportFromCsvAdapter.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include <strsafe.h>
#include <utils/SavePointUtil.h>

ImportFromCsvAdapter::ImportFromCsvAdapter(HWND parentHwnd, ImportFromCsvSupplier * supplier)
	:ImportDatabaseAdapter(parentHwnd, nullptr)
{
	this->parentHwnd = parentHwnd;
	this->supplier = supplier;
}


UserTableStrings ImportFromCsvAdapter::getTables(uint64_t userDbid)
{
	try {
		return tableService->getUserTableStrings(userDbid);
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
	return UserTableStrings();
}

void ImportFromCsvAdapter::loadHeaderForColumnListView(QListViewCtrl & columnListView)
{
	// clear column
	CHeaderCtrl headerCtrl = columnListView.GetHeader();
	for (int i = headerCtrl.GetItemCount() -1; i >=0 ; i--) {
		columnListView.DeleteColumn(i);
	}

	columnListView.InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	std::wstring headColumns[2] = { L"Source Column", L"Target Column" };
	int headSize[2] = { 170, 170};
	
	for (int i = 0; i < 2; i++) {
		columnListView.InsertColumn(i+1, headColumns[i].c_str(), LVCFMT_LEFT, headSize[i]);
	}
}


void ImportFromCsvAdapter::loadHeaderForDataListView(const Columns & columns, QListViewCtrl & dataListView)
{
	if (columns.empty() || !dataListView.IsWindow()) {
		return;
	}
	// clear column
	CHeaderCtrl headerCtrl = dataListView.GetHeader();
	for (int i = headerCtrl.GetItemCount() -1; i >=0 ; i--) {
		dataListView.DeleteColumn(i);
	}
	
	dataListView.InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);

	int n = static_cast<int>(columns.size());
	for (int i = 0; i < n; i++) {
		dataListView.InsertColumn(i+1, columns[i].c_str(), LVCFMT_LEFT, 100);
	}
}

int ImportFromCsvAdapter::loadCsvFileToColumnListView(const std::wstring &importPath, QListViewCtrl & columnListView)
{
	if (importPath.empty() || _waccess(importPath.c_str(), 0) != 0 
		|| supplier->getRuntimeTblName().empty() || !columnListView.IsWindow()) {
		return 0;
	}
	
	std::wstring firstLine = readFirstLineFromCvsFile(importPath);
	if (firstLine.empty()) {
		QPopAnimate::error(E(L"200002"));
		return 0;
	}
	columnListView.DeleteAllItems();
	auto fields = splitLineAndConvertStringByCsvSettings(firstLine);
	
	supplier->setCsvRuntimeColumns(fields);
	int rowCount = static_cast<int>(fields.size());

	Columns targetColumns = tableService->getUserColumnStrings(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	supplier->setTblOriginalColumns(targetColumns);
	int targetLen = static_cast<int>(targetColumns.size());
	if (targetLen < rowCount) {
		// add blank value to targetColumns
		for (int i = 0; i < (rowCount - targetLen); i++) {
			targetColumns.push_back(L"");
		}
	} else if (targetLen > rowCount) {
		auto iter = targetColumns.begin();
		for (int i = 0; i < rowCount; i++) {
			iter++;
		}
		targetColumns.erase(iter, targetColumns.end());
	}
	supplier->setTblRuntimeColumns(targetColumns);

	columnListView.SetItemCount(rowCount);
	return rowCount;
}

int ImportFromCsvAdapter::loadCsvFileToDataListView(const std::wstring & importPath, QListViewCtrl & dataListView)
{	
	if (importPath.empty() || _waccess(importPath.c_str(), 0) != 0 
		|| supplier->getRuntimeTblName().empty() || !dataListView.IsWindow()) {
		return 0;
	}
	
	std::wifstream ifs;
	if (supplier->csvCharset == L"UTF-16") {
		auto codeccvt = new std::codecvt_utf16<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf16(std::locale("C"), codeccvt);
		ifs.imbue(utf16);
	} else {
		auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf8(std::locale("C"), codeccvt);
		ifs.imbue(utf8);
	}
	
	ifs.open(importPath, std::ios::in);
	if (ifs.bad()) {
		return 0;
	}
	
	std::wstring line = readLineFromCsvFileStream(ifs);
	if (line.empty()) {
		QPopAnimate::error(E(L"200002"));
		ifs.close();
		return 0;
	}
	auto fields = splitLineAndConvertStringByCsvSettings(line);

	dataListView.DeleteAllItems();
	supplier->clearCsvRuntimeDatas();
	int rowCount = 0;
	
	if (supplier->csvColumnNameOnTop) {
		loadHeaderForDataListView(fields, dataListView);
	}else {
		int n = static_cast<int>(fields.size());
		Columns columns;
		for (int i = 0; i < n; i++) {
			columns.push_back(L"Column##" + std::to_wstring(i + 1));
		}
		loadHeaderForDataListView(columns, dataListView);
		supplier->addCsvRuntimeData(fields);
		rowCount++;
	}
		
	while (!ifs.eof()) {
		line = readLineFromCsvFileStream(ifs);
		if (line.empty() || 
			(line.size() <= 2 && (*(line.rbegin()) == L'\r' || *(line.rbegin()) == L'\n'))) {
			continue;
		}
		auto rowItem = splitLineAndConvertStringByCsvSettings(line);
		if (rowItem.size() != supplier->getCsvRuntimeColumns().size()) {
			continue;
		}
		supplier->addCsvRuntimeData(rowItem);
		rowCount++;
	}
	ifs.close();
	dataListView.SetItemCount(rowCount);
	return rowCount;
}

void ImportFromCsvAdapter::fillSubItemForColumnListView(NMLVDISPINFO* pLvdi, QListViewCtrl & columnListView)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return ;

	auto count = static_cast<int>(supplier->getCsvRuntimeColumns().size());
	if (!count || count <= iItem)
		return ;

	// set checked/unchecked image in the first column	
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) {
		pLvdi->item.mask = LVIF_IMAGE;
		pLvdi->item.iIndent = 1;
		if (getIsChecked(&columnListView, pLvdi->item.iItem)) { 
			pLvdi->item.iImage = 1;
		} else {
			pLvdi->item.iImage = 0;
		}
		
		return ;
	} else  if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring val;
		if (supplier->csvColumnNameOnTop) {
			val = supplier->getCsvRuntimeColumns().at(iItem);
		} else {
			val = L"Column##" + std::to_wstring(iItem + 1);
		}
		
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		return ;
	} else  if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		if (iItem < supplier->getTblRuntimeColumns().size()) {
			std::wstring & val = supplier->getTblRuntimeColumns().at(pLvdi->item.iItem);
			StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
			columnListView.createComboBox(iItem, pLvdi->item.iSubItem, val);
		} else {
			columnListView.createComboBox(iItem, pLvdi->item.iSubItem, L"");
		}
		
		return ;
	}
}

void ImportFromCsvAdapter::fillSubItemForDataListView(NMLVDISPINFO* pLvdi, QListViewCtrl & dataListView)
{
	auto iItem = pLvdi->item.iItem;
	auto iSubItem = pLvdi->item.iSubItem;
	if (-1 == iItem)
		return ;

	auto & csvRuntimeDatas = supplier->getCsvRuntimeDatas();
	auto count = static_cast<int>(csvRuntimeDatas.size());
	if (!count || count <= iItem)
		return ;

	auto iter = csvRuntimeDatas.begin();
	for (int i = 0; i < iItem; i++) {
		iter++;
	}
	if (iter == csvRuntimeDatas.end()) {
		return;
	}
	RowItem & rowItem = (*iter);
	int n = static_cast<int>(rowItem.size());
	if (iSubItem > n) {
		return;
	}
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) {
		return ;
	} else if (pLvdi->item.mask & LVIF_TEXT){
		std::wstring & val = rowItem.at(pLvdi->item.iSubItem - 1);
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	}
}


void ImportFromCsvAdapter::clickListViewSubItem(NMITEMACTIVATE * clickItem, QListViewCtrl & columnListView)
{
	if (clickItem->iSubItem == 1) {
		columnListView.SelectItem(clickItem->iItem);
	} else if (clickItem->iSubItem == 2) {
		Columns columns = supplier->getTblOriginalColumns();
		std::wstring blkstr;
		columns.insert(columns.begin(), blkstr);
		columnListView.showComboBox(clickItem->iItem, clickItem->iSubItem, columns);
	}
}

/**
 * if the row of index=iItem is selected.
 * 
 * @param iItem the row index
 * @return 
 */
bool ImportFromCsvAdapter::getIsChecked(QListViewCtrl * listView, int iItem)
{
	if (listView->GetSelectedCount() == 0) {
		return false;
	}
	int nSelItem = listView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); // 向下搜索选中的项 -1表示先找出第一个
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = listView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // 继续往下搜索选中项
	}

	return false;
}

std::wstring ImportFromCsvAdapter::readFirstLineFromCvsFile(const std::wstring & importPath)
{
	if (importPath.empty() || _waccess(importPath.c_str(), 0) != 0) {
		return 0;
	}
	
	std::wifstream ifs;
	if (supplier->csvCharset == L"UTF-16") {
		auto codeccvt = new std::codecvt_utf16<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf16(std::locale("C"), codeccvt);
		ifs.imbue(utf16);
	} else {
		auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf8(std::locale("C"), codeccvt);
		ifs.imbue(utf8);
	}
	
	ifs.open(importPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}
	
	std::wstring line = readLineFromCsvFileStream(ifs);
	if (line.empty()) {
		QPopAnimate::error(E(L"200002"));
		ifs.close();
		return line;
	}

	ifs.close();

	return line;
}

std::wstring ImportFromCsvAdapter::readLineFromCsvFileStream(std::wifstream &ifs)
{
	std::stack<int> stack;
	std::wstring line;
	int i = 0;
	while (!ifs.eof()) {
		wchar_t ch;
		ifs.get(ch); // 读取一个字符

		// 下面一堆判断,是用来确定换行符,是否包含在包含符号中
		if (!supplier->csvFieldEnclosedBy.empty()
				&& ch == supplier->csvFieldEnclosedBy.at(0)){ // 条件1. 获取到的字符是包含字符(开启enclose)				
			if (!supplier->csvFieldEscapedBy.empty() // （已开启escape）
				&& !line.empty()
				&& *(line.rbegin()) == *(supplier->csvFieldEscapedBy.rbegin())) {
				// 条件2.判断line最后一个字符,是否是转义符（已开启escape）,是，则继续
				line.push_back(ch); 
				continue;
			}
			if (!stack.empty()) { // 关闭的包含字符，如引号"",这里是最后的那一个
				stack.pop();
			} else {
				stack.push(i); // 开启新的包含字符，如引号"",这里是开始的那一个
			}			
		}

		bool foundLineSeperator = false; // 是否找到换行符
		if (supplier->csvLineTerminatedBy == L"CR" && ch == L'\r') {
			foundLineSeperator = true;
		} else if (supplier->csvLineTerminatedBy == L"LF" && ch == L'\n') {
			foundLineSeperator = true;
		} else if (supplier->csvLineTerminatedBy == L"CRLF" && ch == L'\n') {
			foundLineSeperator = true;
		}

		// stack 用来标识还有包含符号未关闭
		if (foundLineSeperator && stack.empty()) {
			break;
		}
		line.push_back(ch);
		i++;
	}

	StringUtil::trim(line);
	return line;
}

/**
 * Spit a line, then convert the every field value with CSV settings.
 * 
 * @param line - one row string
 * @return - field value vector
 */
std::vector<std::wstring> ImportFromCsvAdapter::splitLineAndConvertStringByCsvSettings(std::wstring line)
{
	std::wstring::size_type pos;
	std::vector<std::wstring> result;
	line.append(supplier->csvFieldTerminateBy); // append a field separator
	std::wstring pattern = supplier->csvFieldTerminateBy;
	if (pattern == L"TAB") {
		pattern = L"\t";
	}
	int size = static_cast<int>(line.size());
	for (int i = 0; i < size; i++) {
		pos = line.find(pattern, i);
		if (pos < size && pos != std::wstring::npos) {
			std::wstring s = line.substr(i, pos - i);
			if (s.empty()) {
				i = static_cast<int>(pos + pattern.size()) - 1;
				continue;
			}
			
			// Field value enclose by
			if (!supplier->csvFieldEnclosedBy.empty()) {
				for (;;) {
					size_t _begin = s.find(supplier->csvFieldEnclosedBy);
					size_t _end = s.rfind(supplier->csvFieldEnclosedBy);

					if (_begin != std::wstring::npos && _end != std::wstring::npos && _begin != _end) {
						s = s.substr(_begin + 1, _end - _begin - 1);
						break;
					} else if (_begin != std::wstring::npos && (_end == std::wstring::npos || _begin == _end)) {
						// 表示分离字符[pattern]，未处于前后两个包含符[csvFieldEnclosedBy]之中
						// 如 "123456abcd, 只出现前一个包含附双引号，未出现后一个包含附双引号，逗号分隔符还未能处于两个双引号之间
						pos = line.find(pattern, pos + 1);
						if (pos == std::wstring::npos) {
							break;
						}
						s = line.substr(i, pos - i);
					} else {
						break; // not found any csvFieldEnclosedBy
					}
				}				
			}

			if (!supplier->csvFieldEscapedBy.empty() && !supplier->csvFieldEnclosedBy.empty()) {
				std::wstring before = supplier->csvFieldEscapedBy + supplier->csvFieldEnclosedBy;
				std::wstring after = supplier->csvFieldEnclosedBy;
				s = StringUtil::replace(s, before, after);
			}

			if (!supplier->csvNullAsKeyword.empty() && supplier->csvNullAsKeyword == L"YES") {
				if (s == L"NULL" || s == L"null") {
					s = L"< NULL >";
				}
			}


			if (!s.empty()) {
				StringUtil::trim(s);
			}
			result.push_back(s);
			i = static_cast<int>(pos + pattern.size()) - 1;
		}
	}
	return result;
}

std::wstring ImportFromCsvAdapter::getPreviewSql()
{	
	auto sqlList = getRuntimeSqlList();
	std::wstring result;
	int i = 0;
	for (auto & sql : sqlList) {
		if (i++) {
			result.append(L"\n");
		}
		result.append(sql);
	}

	return result;
}

std::list<std::wstring> ImportFromCsvAdapter::getRuntimeSqlList()
{
	std::list<std::wstring> result;
	auto & targetColumns = supplier->getTblRuntimeColumns();
	auto & csvRuntimeDatas = supplier->getCsvRuntimeDatas();
	int colLen = static_cast<int>(targetColumns.size());
	int n = static_cast<int>(csvRuntimeDatas.size());
	std::wstring insertClause = L"INSERT INTO \"";
	insertClause.append(supplier->getRuntimeTblName()).append(L"\" ");
	for (auto & item : csvRuntimeDatas) {
		std::wstring fieldClause;
		std::wstring valuesClause;
		int c = 0; // not empty columns count
		for (int i = 0; i < colLen; i++) {
			auto & columnName = targetColumns.at(i);
			if (columnName.empty()) {
				continue;
			}
			std::wstring val = item.at(i);
			
			if (c++) {
				fieldClause.append(L",");
				valuesClause.append(L",");
			}
			
			fieldClause.append(L"\"").append(columnName).append(L"\"");
			val == L"< NULL >" ?  valuesClause.append(L"NULL") 
				: valuesClause.append(L"'").append(StringUtil::escapeSql(val)).append(L"'");
		}

		if (fieldClause.empty() || valuesClause.empty()) {
			continue;
		}
		// SQL : insert into "tbl_name" ([fieldClause]) VALUES ([valuesClause])
		std::wstring sql = insertClause;
		sql.append(L"(").append(fieldClause).append(L") VALUES (").append(valuesClause).append(L");");
		result.push_back(sql);
	}

	return result;
}

bool ImportFromCsvAdapter::importFromRuntimeSqlList()
{
	// 1.generate runtime sql statements list
	auto sqlList = getRuntimeSqlList();
	if (sqlList.empty()) {
		QPopAnimate::error(E(L"200028"));
		return false;
	}
	int percent = 5;
	::PostMessage(parentHwnd, Config::MSG_IMPORT_PROCESS_ID, 0, percent);

	// 2.Begin a save point
	uint64_t targetUserDbId = supplier->getRuntimeUserDbId();
	std::wstring savePoint = SavePointUtil::create(L"import_from_csv");
	std::wstring sql = L"SAVEPOINT \"" + savePoint + L"\";";
	tableService->execBySql(targetUserDbId, sql);
	
	try {
		// 3. begin execute sql statement	
		int n = static_cast<int>(sqlList.size());
		int avgVal = int(round(95.0 / double(n)));
		for (auto & oneSql : sqlList) {
			sql = oneSql;
			tableService->execBySql(targetUserDbId, sql);
			percent += avgVal;
			::PostMessage(parentHwnd, Config::MSG_IMPORT_PROCESS_ID, 0, percent);
		}
		::PostMessage(parentHwnd, Config::MSG_IMPORT_PROCESS_ID, 1, 100);

		// 4.Release the SAVEPOINT
		sql = L"RELEASE \"" + savePoint + L"\";";
		tableService->execBySql(targetUserDbId, sql);
		return true;
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK TO \"" + savePoint + L"\";";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_IMPORT_PROCESS_ID, 2, NULL);
		return false;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK TO \"" + savePoint + L"\";";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_IMPORT_PROCESS_ID, 2, NULL);
		return false;
	}
	return false;
}
