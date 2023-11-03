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

 * @file   ImportFromCsvSupplier.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-03
 *********************************************************************/
#pragma once
#include "core/common/supplier/QSupplier.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "core/entity/Entity.h"

class ImportFromCsvSupplier :public QSupplier 
{
public:
	ImportFromCsvSupplier(DatabaseSupplier * databaseSupplier);

	std::wstring importPath;
	int csvColumnNameOnTop = 0;
	std::wstring csvFieldTerminateBy;
	std::wstring csvFieldEnclosedBy;
	std::wstring csvFieldEscapedBy;
	std::wstring csvLineTerminatedBy;
	std::wstring csvNullAsKeyword;
	std::wstring csvCharset;

	Columns & getCsvRuntimeColumns() { return csvRuntimeColumns; }
	void setCsvRuntimeColumns(const Columns & val) { csvRuntimeColumns = val; }


	Columns & getTblRuntimeColumns() { return tblRuntimeColumns; }
	void setTblRuntimeColumns(const Columns & val) { tblRuntimeColumns = val; }
private:
	Columns csvRuntimeColumns;
	Columns tblRuntimeColumns;
};
