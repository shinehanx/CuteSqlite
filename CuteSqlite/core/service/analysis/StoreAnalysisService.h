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

 * @file   StoreAnalysisService.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-16
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/user/DbStatUserRepository.h"
#include "core/repository/user/SqliteSchemaUserRepository.h"

class StoreAnalysisService : public BaseService<StoreAnalysisService, DbStatUserRepository>
{
public:
	StoreAnalysisService() {}
	~StoreAnalysisService() {}

	int getPageSize(uint64_t userDbId);
	uint64_t getPageCount(uint64_t userDbId);
	uint64_t getFreePageCount(uint64_t userDbId);
	DbSpaceUsed & getDbSpaceUsed(uint64_t userDbId);
	TblIdxEntryCntVector getSeperateTblEntryCntList(uint64_t userDbId);
	TblIdxPageCntVector getAllTblIdxPageCntList(uint64_t userDbId);
	TblIdxPageCntVector getSeperateTblIdxPageCntList(uint64_t userDbId);
	TblIdxSpaceUsed getAllTblAndIdxReport(uint64_t userDbId);
	TblIdxSpaceUsed getAllTableReport(uint64_t userDbId);
	TblIdxSpaceUsed getAllIndexReport(uint64_t userDbId);
	TblIdxSpaceUsed getSpecifiedTblIndexReport(uint64_t userDbId, const std::wstring& tblName);
	TblIdxSpaceUsed getSpecifiedTblOnlyReport(uint64_t userDbId, const std::wstring& tblName);
	TblIdxSpaceUsed getSpecifiedTblThenIdxOnlyReport(uint64_t userDbId, const std::wstring & tblName);
	TblIdxSpaceUsed getSpecifiedIdxReport(uint64_t userDbId, const std::wstring & tblName, const std::wstring & idxName);
	TblIdxSpaceUsed getSpecifiedTblReport(uint64_t userDbId, const std::wstring & tblName);

	std::vector<std::wstring> getTableNames(uint64_t userDbId);
	std::vector<std::wstring> getIndexNamesByTblName(uint64_t userDbId, const std::wstring & tblName);
	int getSpaceUsedCountByTblName(uint64_t userDbId, const std::wstring & tblName);
	void clearCache(uint64_t userDbId);	
private:
	// template params: first(uint64_t) - userDbId, second(TblIdxSpaceUsedList) - table and index SpaceUsed List
	std::unordered_map<uint64_t, TblIdxSpaceUsedList> cacheTblIdxSpaceUsedListMap; // The TblIdxSpaceUsedList cache Map of specified user db id
	std::unordered_map<uint64_t, DbSpaceUsed> cacheDbSpaceUsedMap; // The DbSpaceUsed cache Map of specified user db id

	SqliteSchemaUserRepository * sqliteSchemaUserRepository = SqliteSchemaUserRepository::getInstance();

	uint64_t autovacuumOverhead(uint64_t userDbId, uint64_t filePgcnt, int pageSize);
	uint64_t getInUsePage(uint64_t userDbId);
	TblIdxSpaceUsedList & getTblIdxSpaceUsedList(uint64_t userDbId);
	uint64_t getUserPayload(uint64_t userDbId);
	int isWithoutRowid(std::wstring name);
	void sumTblIdxSpaceUsed(TblIdxSpaceUsed &item, TblIdxSpaceUsed &result);
	double percent(uint64_t val, uint64_t total);
};
