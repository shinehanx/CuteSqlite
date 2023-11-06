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

 * @file   ImportAsCsvAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-04
 *********************************************************************/
#pragma once
#include "ImportDatabaseAdapter.h"
#include "ui/database/dialog/supplier/ImportFromCsvSupplier.h"

class ImportFromCsvAdapter : public ImportDatabaseAdapter
{
public:
	ImportFromCsvAdapter(HWND parentHwnd, ImportFromCsvSupplier * supplier);
	~ImportFromCsvAdapter() {};

	//FOR IMPORT AS CSV
	UserTableStrings getTables(uint64_t userDbid);
	void loadHeaderForColumnListView(QListViewCtrl & columnListView);
	void loadHeaderForDataListView(const Columns & columns, QListViewCtrl & columnListView);
	int loadCsvFileToColumnListView(const std::wstring &importPath, QListViewCtrl & columnListView);
	int loadCsvFileToDataListView(const std::wstring & importPath, QListViewCtrl & dataListView);
	void fillSubItemForColumnListView(NMLVDISPINFO* plvdi, QListViewCtrl & columnListView);
	void fillSubItemForDataListView(NMLVDISPINFO* plvdi, QListViewCtrl & dataListView);
	void clickListViewSubItem(NMITEMACTIVATE * clickItem, QListViewCtrl & columnListView);

	std::wstring getPreviewSql();

	std::list<std::wstring> getRuntimeSqlList();

	// import action
	bool importFromRuntimeSqlList();
private:
	ImportFromCsvSupplier * supplier = nullptr;
	bool getIsChecked(QListViewCtrl * listView, int iItem);
	std::wstring readFirstLineFromCvsFile(const std::wstring & importPath);
	std::wstring readLineFromCsvFileStream(std::wifstream &ifs);
	std::vector<std::wstring> splitLineAndConvertStringByCsvSettings(std::wstring line);
};
