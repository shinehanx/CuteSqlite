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

 * @file   TableStructureSupplier.h
 * @brief  This class supply runtime data for TableStructurePage,TableTabView,
 * ,TableColumnsPage,TableIndexesPage objects and their's Adapter objects
 * 
 * @author Xuehan Qin
 * @date   2023-10-21
 *********************************************************************/
#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "core/entity/Entity.h"

class TableStructureSupplier {
public:
	// TableColumnsPage list view header columns settings
	const static Columns colsHeadColumns;
	const static std::vector<int> colsHeadSizes;
	const static std::vector<int> colsHeadFormats;
	const static std::vector<std::wstring> colsDataTypeList;

	// TableIndexesPage list view header columns settings
	const static Columns idxHeadColumns;
	const static std::vector<int> idxHeadSizes;
	const static std::vector<int> idxHeadFormats;
	const static std::vector<std::wstring> idxTypeList;

	// Getter or setter
	TblOperateType getOperateType() const { return operateType; }
	void setOperateType(TblOperateType val) { operateType = val; }

	uint64_t getRuntimeUserDbId() const { return runtimeUserDbId; }
	void setRuntimeUserDbId(uint64_t val) { runtimeUserDbId = val; }

	std::wstring getRuntimeTblName() const { return runtimeTblName; }
	void setRuntimeTblName(const std::wstring & val) { runtimeTblName = val; }

	std::wstring getOrigTblName() const { return origTblName; }
	void setOrigTblName(std::wstring val) { origTblName = val; }

	std::wstring getRuntimeSchema() const { return runtimeSchema; }
	void setRuntimeSchema(const std::wstring & val) { runtimeSchema = val; }	
	
	ColumnInfoList & getColsRuntimeDatas() { return colsRuntimeDatas; }
	void setColsRuntimeDatas(ColumnInfoList & val) { colsRuntimeDatas = val; }
	void getColsRuntimeData(int nSelItem);
	void eraseColsRuntimeData(int nSelItem);

	ColumnInfoList & getColsOrigDatas() { return colsOrigDatas; }
	void setColsOrigDatas(ColumnInfoList val) { colsOrigDatas = val; }
	void eraseColsOrigData(int nSelItem);

	IndexInfoList & getIdxRuntimeDatas() { return idxRuntimeDatas; }
	void setIdxRuntimeDatas(IndexInfoList & val) { idxRuntimeDatas = val; }
	void eraseIdxRuntimeData(int nSelItem);

	IndexInfoList & getIdxOrigDatas() { return idxOrigDatas; }
	void setIdxOrigDatas(IndexInfoList val) { idxOrigDatas = val; }
	void eraseIdxOrigData(int nSelItem);

	void updateRelatedColumnsIfDeleteIndex(const IndexInfo &indexInfo);
private:
	TblOperateType operateType; // new table - NEW_TABLE, alter table - MOD_TABLE

	// Runtime variables, for create or alter table
	uint64_t runtimeUserDbId = 0;
	std::wstring runtimeSchema;
	std::wstring runtimeTblName;
	// store the runtime data of the table column info(s)
	ColumnInfoList colsRuntimeDatas;
	// store the runtime data of the column(s) settings
	IndexInfoList idxRuntimeDatas;
	
	// Original variables, for alter table
	// store the original table name, for alter table comparing with runtimeTblName
	std::wstring origTblName;
	// store the original table column infos, for alter table comparing with colsRuntimeDatas
	ColumnInfoList colsOrigDatas; 
	// store the original table index infos, for alter table comparing with idxRuntimeDatas
	IndexInfoList idxOrigDatas;
};
