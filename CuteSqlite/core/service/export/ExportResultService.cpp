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

int ExportResultService::exportToCsv(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportCsvParams & csvParams)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty());
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
			auto & selColumn = selColumns.at(i);
			if (i > 0) {
				ofs << csvParams.csvFieldTerminatedBy;
			}
			ofs << selColumn;
		}
		//ofs << csvParams.csvLineTerminatedBy;
		ofs << endl;
	}

	// 3.write the data to file
	int n = 0;
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
		n++;
	}
	ofs.flush();
	ofs.close();

	return n;
}

int ExportResultService::exportToJson(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
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

	// 3.write the data to file
	ofs << L'[' << endl;
	int n = 0;
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		if (n > 0) {
			ofs  << L',' << endl;
		}
		ofs << L'\t' << L'{' << endl;
		int i = 0;
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			if (i > 0) {
				ofs  << L',' << endl;
			}
			std::wstring val = StringUtil::escape(vals.at(nItem));
			ofs << L"\t\t\"" << selColumn << L"\":\"" << val << "\"";
			i++;
		}
		ofs << endl;
		ofs << L'\t' << L'}';
		n++;
	}
	ofs << endl << L']';
	ofs.flush();
	ofs.close();

	return n;
}



int ExportResultService::exportToHtml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty() && !datas.empty());
	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	std::wstring tplHtml = readQueryResultHtmlTemplate();

	// 1. create and open the ouput fstream	
	std::wofstream ofs;
	auto codecvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codecvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	// 2.write the columns on the top
	
	int n = static_cast<int>(selColumns.size());
	std::wostringstream colHtml, dataHtml;
	colHtml << L"<tr>" << endl;
	for (int i = 0; i < n; i++) {
		auto selColumn = selColumns.at(i);
		colHtml << L"\t" << L"<td bgcolor=silver class='medium'>" << selColumn << L"</td>" << endl;
	}
	//ofs << csvParams.csvLineTerminatedBy;
	colHtml << L"<tr>" << endl;
	std::wstring result = StringUtil::replace(tplHtml, L"{<!--columns-->}", colHtml.str());
	

	// 3.write the data to file
	n = 0;
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		dataHtml << L"<tr>" << endl;
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			dataHtml << L"\t" << L"<td class='normal' valign='top'>" << vals.at(nItem) << L"</td>" << endl;
			i++;
		}
		dataHtml << L"<tr>" << endl;
		n++;
	}
	result = StringUtil::replace(result, L"{<!--datas-->}", dataHtml.str());
	ofs << result << endl;
	ofs.flush();
	ofs.close();
	return n;
}

int ExportResultService::exportToXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty() && !datas.empty());
	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	// 1. create and open the ouput fstream	
	std::wofstream ofs;
	auto codecvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codecvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	// 3.write the data to file
	ofs << L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	ofs << "<data>" << endl;
	int n = 0;
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		
		ofs << L"\t<row>" << endl;
		int i = 0;
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			
			std::wstring val = StringUtil::escapeXml(vals.at(nItem));
			ofs << L"\t\t<" << selColumn << L">" << val  << L"</" << selColumn << L">" << endl;
			i++;
		}
		ofs << L"\t</row>" << endl;
		n++;
	}
	ofs << "</data>" << endl;
	ofs.flush();
	ofs.close();

	return n;
}

int ExportResultService::exportToExcelXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportExcelParams & excelParams)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty() && !datas.empty());
	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	std::wstring tplHtml = readQueryResultXmlTemplate();

	// 1. create and open the ouput fstream	
	std::wofstream ofs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	// 2.write the columns on the top	
	int n = static_cast<int>(selColumns.size());
	std::wostringstream colXml, dataXml;
	colXml << L"\t<ss:Row>" << endl;
	for (int i = 0; i < n; i++) {
		auto selColumn = selColumns.at(i);
		colXml << L"\t\t" << L"<ss:Cell  ss:StyleID=\"s27\"><Data ss:Type=\"String\">" << selColumn << L"</Data></ss:Cell>" << endl;
	}
	//ofs << csvParams.csvLineTerminatedBy;
	colXml << L"\t</ss:Row>" << endl;
	std::wstring result = StringUtil::replace(tplHtml, L"{<!--columns-->}", colXml.str());
	

	// 3.write the data to file
	n = 0;
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		dataXml << L"\t<ss:Row>" << endl;
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			std::wstring val = StringUtil::escapeXml(vals.at(nItem));
			dataXml << L"\t\t" << L"<ss:Cell><Data ss:Type=\"String\">" << val << L"</Data></ss:Cell>" << endl;
			i++;
		}
		dataXml << L"\t</ss:Row>" << endl;
		n++;
	}
	result = StringUtil::replace(result, L"{<!--datas-->}", dataXml.str());
	ofs << result << endl;
	ofs.flush();
	ofs.close();
	return n;
}

/**
 * Export the query result data to an SQL file.
 * 
 * @param exportPath - Export path
 * @param tbl - Query table
 * @param columns - The columns of query data
 * @param selColumns - Select export columns 
 * @param datas - The query result data
 * @param sqlarams - The Sql Settings params 
 * @return Export rows count 
 */
int ExportResultService::exportToSql(std::wstring & exportPath, 
		UserTable & tbl, 
		Columns & columns, 
		ExportSelectedColumns & selColumns, 
		DataList & datas, 
		ExportSqlParams & sqlarams)
{
	ATLASSERT(!exportPath.empty() && !columns.empty() && !selColumns.empty());
	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	// 1. create and open the ouput fstream	
	std::wofstream ofs;
	auto codecvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codecvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	std::wstring tplSql = readQueryResultSqlTemplate();
	ofs << tplSql << endl;

	// 2.write the structure sql	
	int n = static_cast<int>(selColumns.size());
	if (sqlarams.sqlSetting == L"structure-only" || sqlarams.sqlSetting == L"structure-and-data") {
		ofs << tbl.sql << L";" << endl;
	}
	
	if (sqlarams.sqlSetting == L"structure-only") {
		ofs.flush();
		ofs.close();
		return 0;
	}
	
	// 3.write the data to file
	n = 0;
	for (auto vals : datas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		std::wostringstream dataSql, columnStmt, valuesStmt;
		dataSql << L"INSERT INTO \"" << tbl.name << L"\" ";

		columnStmt << L"(";
		valuesStmt << L" VALUES (";
		// write the selected column data value
		for (auto selColumn : selColumns) {
			int nItem = getColumnIndex(columns, selColumn);
			std::wstring val = StringUtil::escapeSql(vals.at(nItem));

			if (i > 0) {
				columnStmt << L", ";
				valuesStmt <<  L", ";
			}
			columnStmt << L"\"" << selColumn << "\"";
			valuesStmt << L"'" << val <<  L"'";
			i++;
		}
		columnStmt <<  L')';
		valuesStmt << L')';
		dataSql << columnStmt.str() << valuesStmt.str() << L';';
		ofs << dataSql.str() << endl;
		n++;
	}
	
	ofs.flush();
	ofs.close();
	return n;
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

/**
 * Change the file extend name of export path.
 * 
 * @param exportPath the file path of export file
 * @param ext the extend name
 * @return new export path
 */
std::wstring & ExportResultService::changeExportPathExt(std::wstring & exportPath, const wchar_t * ext)
{
	if (ext == nullptr) {
		return exportPath;
	}
	std::wstring dir = FileUtil::getFileDir(exportPath);
	std::wstring fname = FileUtil::getFileName(exportPath, false);
	if (dir.empty() || fname.empty()) {
		return exportPath;
	}
	exportPath = dir;
	exportPath.append(fname).append(L".").append(ext);
	return exportPath;
}

std::wstring ExportResultService::readQueryResultHtmlTemplate()
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\query-result-html.tpl";

	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(tplPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}

	wchar_t content[1024];
	memset(content, 0, sizeof(wchar_t) * 1024);
	ifs.read(content, 1024);
	std::wstring result(content);

	ifs.close();
	return result;	
}

std::wstring ExportResultService::readQueryResultXmlTemplate()
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\query-result-excel-xml.tpl";

	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(tplPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}

	wchar_t content[1024];
	memset(content, 0, sizeof(wchar_t) * 1024);
	ifs.read(content, 1024);
	std::wstring result(content);

	ifs.close();
	return result;	
}

std::wstring ExportResultService::readQueryResultSqlTemplate()
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\query-result-sql.tpl";

	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(tplPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}

	wchar_t content[1024];
	memset(content, 0, sizeof(wchar_t) * 1024);
	ifs.read(content, 1024);
	std::wstring result(content);

	ifs.close();
	return result;
}
