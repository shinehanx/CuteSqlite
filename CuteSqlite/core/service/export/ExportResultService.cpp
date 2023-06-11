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

 * @file   ExportResultService.cpp
 * @brief  Export the query result datas to file
 * 
 * @author Xuehan Qin
 * @date   2023-06-10
 *********************************************************************/
#include "stdafx.h"
#include "ExportResultService.h"
#include <fstream>
#include "utils/FileUtil.h"

void ExportResultService::exportToCsv(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportCsvParams & csvParams)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty() && !datas.empty());
	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	// 1. create and open the ouput fstream
	
	std::wofstream ofs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	// 2.write the columns on the top
	if (csvParams.hasColumnOnTop) {
		int n = static_cast<int>(selColumns.size());
		for (int i = 0; i < n; i++) {
			auto selColumn = selColumns.at(i);
			if (i > 0) {
				ofs << csvParams.csvFieldTerminatedBy;
			}
			ofs << selColumn;
		}
		//ofs << csvParams.csvLineTerminatedBy;
		ofs << endl;
	}

	// 3.write the data to file
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			if (i > 0) {
				ofs << csvParams.csvFieldTerminatedBy;
			}
			ofs << csvParams.csvFieldEnclosedBy << vals.at(nItem) << csvParams.csvFieldEnclosedBy;
			i++;
		}
		ofs << endl;
	}

	ofs.close();
}

void ExportResultService::exportToJson(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
{

}

void ExportResultService::exportToHtml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
{

}

void ExportResultService::exportToXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
{

}

void ExportResultService::exportToExcelXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportExcelParams & excelParams)
{

}

void ExportResultService::exportToSql(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportSqlParams & sqlarams)
{

}

/**
 * Get the column index in the vector of Columns.
 * 
 * @param columns The vector of Columns
 * @param columnName Column name
 * @return index 
 */
int ExportResultService::getColumnIndex(Columns & columns, std::wstring & columnName)
{
	ATLASSERT(!columns.empty() && !columnName.empty());
	int n = static_cast<int>(columns.size());
	for (int i = 0; i < n; i++) {
		auto col = columns.at(i);
		if (col == columnName) {
			return i;
		}
	}
	return -1;
}
