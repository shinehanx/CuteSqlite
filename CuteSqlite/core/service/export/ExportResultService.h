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

 * @file   ExportResultService.h
 * @brief  Export the query result datas to file
 * 
 * @author Xuehan Qin
 * @date   2023-06-10
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/db/UserDbRepository.h"
#include "core/entity/Entity.h"

class ExportResultService : public BaseService<ExportResultService, UserDbRepository>
{
public:
	ExportResultService() {};
	~ExportResultService() {};

	// do export operation
	int exportToCsv(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas, ExportCsvParams & csvParams);
	int exportToJson2(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas);
	int exportToJson(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas);
	int exportToHtml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas);
	int exportToXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas);
	int exportToExcelXml(std::wstring & exportPath, Columns & columns, ExportSelectedColumns & selColumns, DataList & datas,ExportExcelParams & excelParams);
	int exportToSql(std::wstring & exportPath, 
		UserTable & tbl, 
		Columns & columns, 
		ExportSelectedColumns & selColumns, 
		DataList & datas,
		ExportSqlParams & sqlarams);

	std::wstring & changeExportPathExt(std::wstring & exportPath, const wchar_t * ext);
	
private:
	int getColumnIndex(Columns & columns, std::wstring & columnName);
	std::wstring escapeLineTerminaled(std::wstring & lineTernimal);
	std::wstring readQueryResultHtmlTemplate();
	std::wstring readQueryResultXmlTemplate();
	std::wstring readQueryResultSqlTemplate();
	
};
