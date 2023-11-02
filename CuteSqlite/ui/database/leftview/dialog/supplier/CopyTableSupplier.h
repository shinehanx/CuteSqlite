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

 * @file   CopyTableSupplier.h
 * @brief  The runtime data shared for CopyTableDialog/ShardingTableExpressDialog
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#pragma once
#include "core/common/supplier/QSupplier.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "core/entity/Entity.h"

// Compare key by pair, this class use for declare SubItemComboBoxMap/SubItemCheckBoxMap class
struct CompareKeyByUint16 {
	bool operator()(const uint16_t & key1, const uint16_t & key2) const 
	{  
		if (key1 < key2) {
			return true;
		}
		return false;
	} 
};
typedef std::map<uint16_t, std::wstring, CompareKeyByUint16> ShardingTableMap;

class CopyTableSupplier : public QSupplier {
public:
	CopyTableSupplier(DatabaseSupplier * databaseSupplier);
	CopyTableSupplier();
	~CopyTableSupplier();

	
	StructAndDataSetting getStructAndDataSetting() { return structAndDataSetting; }
	void setStructAndDataSetting(const StructAndDataSetting & val) { structAndDataSetting = val; }

	uint64_t getTargetUserDbId() const { return targetUserDbId; }
	void setTargetUserDbId(uint64_t val) { targetUserDbId = val; }

	std::wstring & getTargetTable() { return targetTable; }
	void setTargetTable(const std::wstring & val) { targetTable = val; }

	bool getEnableTableSharding() const { return enableTableSharding; }
	void setEnableTableSharding(bool val) { enableTableSharding = val; }

	uint16_t getTblSuffixBegin() const { return tblSuffixBegin; }
	void setTblSuffixBegin(uint16_t val) { tblSuffixBegin = val; }

	uint16_t getTblSuffixEnd() const { return tblSuffixEnd; }
	void setTblSuffixEnd(uint16_t val) { tblSuffixEnd = val; }

	bool getEnableShardingStrategy() const { return enableShardingStrategy; }
	void setEnableShardingStrategy(bool val) { enableShardingStrategy = val; }

	std::wstring & getShardingStrategyExpress() { return shardingStrategyExpress; }
	void setShardingStrategyExpress(const std::wstring & val) { shardingStrategyExpress = val; }

	ShardingTableMap getShardingTables();
	
private:
	StructAndDataSetting structAndDataSetting = UNKOWN;
	uint64_t targetUserDbId = 0;
	std::wstring targetTable;

	bool enableTableSharding = false;
	uint16_t tblSuffixBegin = 0;
	uint16_t tblSuffixEnd = 0;

	bool enableShardingStrategy = false;
	std::wstring shardingStrategyExpress;
};
