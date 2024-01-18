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

 * @file   StoreAnalysisService.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-16
 *********************************************************************/
#include "stdafx.h"
#include "StoreAnalysisService.h"
#include <algorithm>

int StoreAnalysisService::getPageSize(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	return getRepository()->getPageSize(userDbId);
}

uint64_t StoreAnalysisService::getPageCount(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	return getRepository()->getPageCount(userDbId);
}

uint64_t StoreAnalysisService::getFreePageCount(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	return getRepository()->getFreePageCount(userDbId);
}

DbSpaceUsed & StoreAnalysisService::getDbSpaceUsed(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	if (!cacheDbSpaceUsedMap.empty()) {
		auto iter = cacheDbSpaceUsedMap.find(userDbId);
		if (iter != cacheDbSpaceUsedMap.end()) {
			return iter->second;
		}
	}
	DbSpaceUsed result;
	result.pageSize = getPageSize(userDbId);
	result.filePgcnt = getPageCount(userDbId);
	result.fileBytes = result.pageSize * result.filePgcnt;
	result.avPgcnt = autovacuumOverhead(userDbId, result.filePgcnt, result.pageSize);
	result.avPercent = result.avPgcnt * 100.0 / result.filePgcnt;
	result.inusePgcnt = getInUsePage(userDbId);
	result.inusePercent = result.inusePgcnt * 100.0 / result.filePgcnt;
	result.freePgcnt = result.filePgcnt - result.inusePgcnt - result.avPgcnt;
	result.freePercent = result.freePgcnt * 100.0 / result.filePgcnt;
	result.freePgcnt2 = getFreePageCount(userDbId);
	result.freePercent2 = result.freePgcnt2 * 100.0 / result.filePgcnt;
	result.filePgcnt2 = result.inusePgcnt + result.freePgcnt2 + result.avPgcnt;
	result.ntable = sqliteSchemaUserRepository->getTableCount(userDbId);
	result.nindex = sqliteSchemaUserRepository->getIndexCount(userDbId);
	result.nautoindex = sqliteSchemaUserRepository->getAutoIndexCount(userDbId);
	result.nmanindex = result.nindex - result.nautoindex;
	result.userPayload = getUserPayload(userDbId);
	result.userPercent = result.userPayload * 100.0 / result.fileBytes;

	cacheDbSpaceUsedMap[userDbId] = result;
	return cacheDbSpaceUsedMap.at(userDbId);
}


TblIdxEntryCntVector StoreAnalysisService::getSeperateTblEntryCntList(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	TblIdxEntryCntVector result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}

	uint64_t totalEntryCount = 0;
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.isIndex) {
			continue;
		}
		std::wstring & tblName = item.tblName;
		totalEntryCount += (item.isWithoutRowid || item.isIndex) ? item.nentry : item.leafEntries; // The record count
		auto & iter = std::find_if(result.begin(), result.end(), [&tblName](auto & entryItem){
			return entryItem.name == tblName;
		});
		if (iter == result.end()) {
			TblIdxEntryCnt cnt;
			cnt.name = tblName;
			if (item.isWithoutRowid || item.isIndex) {
				cnt.entryCnt = item.nentry;
			} else {
				cnt.entryCnt = item.leafEntries;
			}
			cnt.percent = 0;
			result.push_back(cnt);
			continue;
		}
		TblIdxEntryCnt & cnt = *iter;
		if (item.isWithoutRowid || item.isIndex) {
			cnt.entryCnt += item.nentry;
		} else {
			cnt.entryCnt += item.leafEntries;
		}		
	}
	
	for (auto & item : result) {
		item.percent = percent(item.entryCnt, totalEntryCount);
	}
	
	// Note: Only std::vector can be sorted by std::sort, std::list can't be sorted
	// first sort - sorted by name ASC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.name < item2.name;
	});
	// second sort - sorted by pageCnt DESC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.entryCnt > item2.entryCnt;
	});

	return result;
}

TblIdxPageCntVector StoreAnalysisService::getAllTblIdxPageCntList(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	TblIdxPageCntVector result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}

	for (auto & item : tblIdxSpaceUsedList) {
		std::wstring & tblName = item.tblName;
		auto & iter = std::find_if(result.begin(), result.end(), [&tblName](auto & tblIdxPageCnt){
			return tblIdxPageCnt.name == tblName;
		});
		if (iter == result.end()) {
			TblIdxPageCnt pgcnt;
			pgcnt.name = tblName;
			pgcnt.pageCnt = item.intPages + item.leafPages + item.ovflPages;
			pgcnt.percent = 0;
			result.push_back(pgcnt);
			continue;
		}
		TblIdxPageCnt & pgcnt = *iter;
		pgcnt.pageCnt += item.intPages + item.leafPages + item.ovflPages;
	}
	
	uint64_t filePageCount = getPageCount(userDbId);
	for (auto & item : result) {
		item.percent = percent(item.pageCnt, filePageCount);
	}
	
	// Note: Only std::vector can be sorted by std::sort, std::list can't be sorted
	// first sort - sorted by name ASC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.name < item2.name;
	});
	// second sort - sorted by pageCnt DESC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.pageCnt > item2.pageCnt;
	});

	return result;
}


TblIdxPageCntVector StoreAnalysisService::getSeperateTblIdxPageCntList(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	TblIdxPageCntVector result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}

	for (auto & item : tblIdxSpaceUsedList) {
		std::wstring & name = item.name;
		auto & iter = std::find_if(result.begin(), result.end(), [&name](auto & tblIdxPageCnt){
			return tblIdxPageCnt.name == name;
		});
		if (iter == result.end()) {
			TblIdxPageCnt pgcnt;
			pgcnt.name = name;
			pgcnt.pageCnt = item.intPages + item.leafPages + item.ovflPages;
			pgcnt.percent = 0;
			result.push_back(pgcnt);
			continue;
		}
		TblIdxPageCnt & pgcnt = *iter;
		pgcnt.pageCnt += item.intPages + item.leafPages + item.ovflPages;
	}
	
	uint64_t filePageCount = getPageCount(userDbId);
	for (auto & item : result) {
		item.percent = percent(item.pageCnt, filePageCount);
	}
	
	// Note: Only std::vector can be sorted by std::sort function, std::list can't be sorted
	// first sort - sorted by name ASC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.name < item2.name;
	});
	// second sort - sorted by pageCnt DESC
	std::sort(result.begin(), result.end(), [](auto & item1, auto & item2) {
		return item1.pageCnt > item2.pageCnt;
	});

	return result;
}


TblIdxSpaceUsed StoreAnalysisService::getAllTblAndIdxReport(uint64_t userDbId)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}


TblIdxSpaceUsed StoreAnalysisService::getAllTableReport(uint64_t userDbId)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.isIndex) {
			continue;
		}
		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}


TblIdxSpaceUsed StoreAnalysisService::getAllIndexReport(uint64_t userDbId)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (!item.isIndex) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

/**
 * Condition:SpaceUsed.tblName == tblName.
 * 
 * @param userDbId
 * @param tblName
 * @return 
 */
TblIdxSpaceUsed StoreAnalysisService::getSpecifiedTblIndexReport(uint64_t userDbId, const std::wstring& tblName)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

/**
 * Conditioin: SpaceUsed.name == tblName.
 * 
 * @param userDbId
 * @param tblName
 * @return 
 */
TblIdxSpaceUsed StoreAnalysisService::getSpecifiedTblOnlyReport(uint64_t userDbId, const std::wstring& tblName)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.name != tblName || item.isIndex) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

/**
 * Conditioin: SpaceUsed.tblName == tblName and SpaceUsed.isIndex == true.
 * 
 * @param userDbId
 * @param tblName
 * @return 
 */
TblIdxSpaceUsed StoreAnalysisService::getSpecifiedTblThenIdxOnlyReport(uint64_t userDbId, const std::wstring & tblName)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName || !item.isIndex) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

/**
 * Conditioin: SpaceUsed.tblName == tblName and SpaceUsed.name == idxName and SpaceUsed.isIndex == true..
 * 
 * @param userDbId
 * @param tblName
 * @param idxName
 * @return 
 */
TblIdxSpaceUsed StoreAnalysisService::getSpecifiedIdxReport(uint64_t userDbId, const std::wstring & tblName, const std::wstring & idxName)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName || item.name != idxName || !item.isIndex) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

/**
 * Conditioin: SpaceUsed.tblName == tblName.
 * 
 * @param userDbId
 * @param tblName
 * @return 
 */
TblIdxSpaceUsed StoreAnalysisService::getSpecifiedTblReport(uint64_t userDbId, const std::wstring & tblName)
{
	TblIdxSpaceUsed result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName) {
			continue;
		}

		sumTblIdxSpaceUsed(item, result);
	}

	return result;
}

void StoreAnalysisService::sumTblIdxSpaceUsed(TblIdxSpaceUsed &item, TblIdxSpaceUsed &result)
{
	if (item.isWithoutRowid || item.isIndex) {
		result.nentry += item.nentry;
	} else {
		result.nentry += item.leafEntries;
	}

	result.payload += item.payload;
	result.ovflPayload += item.ovflPayload;
	result.mxPayload = max(result.mxPayload, item.mxPayload);
	result.ovflCnt += item.ovflCnt;
	result.leafPages += item.leafPages;
	result.intPages += item.intPages;
	result.ovflPages += item.ovflPages;
	result.leafUnused += item.leafUnused;
	result.intUnused += item.intUnused;
	result.ovflUnused += item.ovflUnused;
	result.gapCnt += item.gapCnt;
	result.compressedSize += item.compressedSize;
	result.depth = max(result.depth, item.depth);
	result.cnt += 1;
}



std::vector<std::wstring> StoreAnalysisService::getTableNames(uint64_t userDbId)
{
	std::vector<std::wstring> result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}

	for (auto & item : tblIdxSpaceUsedList) {
		auto & tblName = item.tblName;
		auto & iter = std::find(result.begin(), result.end(), tblName);
		if (iter == result.end()) {
			result.push_back(tblName);
		}
	}

	std::sort(result.begin(), result.end(), [](auto &item1, auto &item2) {
		return item1 < item2;
	});

	return result;
}


std::vector<std::wstring> StoreAnalysisService::getIndexNamesByTblName(uint64_t userDbId, const std::wstring & tblName)
{
	std::vector<std::wstring> result;
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return result;
	}

	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName || !item.isIndex) {
			continue;
		}
		auto & iter = std::find(result.begin(), result.end(), item.name);
		if (iter == result.end()) {
			result.push_back(item.name);
		}
	}
	return result;
}


int StoreAnalysisService::getSpaceUsedCountByTblName(uint64_t userDbId, const std::wstring & tblName)
{
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return 0;
	}
	int n = 0;
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.tblName != tblName) {
			continue;
		}
		n++;
	}
	return n;
}

void StoreAnalysisService::clearCache(uint64_t userDbId)
{
	auto iter1 = cacheTblIdxSpaceUsedListMap.find(userDbId);
	if (iter1 != cacheTblIdxSpaceUsedListMap.end()) {
		cacheTblIdxSpaceUsedListMap.erase(iter1);
	}

	auto iter2 = cacheDbSpaceUsedMap.find(userDbId);
	if (iter2 != cacheDbSpaceUsedMap.end()) {
		cacheDbSpaceUsedMap.erase(iter2);
	}
}

uint64_t StoreAnalysisService::autovacuumOverhead(uint64_t userDbId, uint64_t filePgcnt, int pageSize)
{
	// Set $autovacuum to non-zero for databases that support auto-vacuum.
	uint64_t autovacuum = getRepository()->getAutovacuum(userDbId);

	// If the database is not an auto-vacuum database or the file consists
	// of one page only then there is no overhead for auto-vacuum. Return zero.
	if (0 == autovacuum || filePgcnt == 1) {
		return 0;
	}

	// The number of entries on each pointer map page. The layout of the
	// database file is one pointer-map page, followed by $ptrsPerPage other
	// pages, followed by a pointer-map page etc. The first pointer-map page
	// is the second page of the file overall.
	double ptrsPerPage = double(pageSize / 5);

	// Return the number of pointer map pages in the database.
	return static_cast<uint64_t>(ceil( (filePgcnt - 1.0) / (ptrsPerPage + 1.0) ));
}

uint64_t StoreAnalysisService::getInUsePage(uint64_t userDbId)
{
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return 0;
	}

	uint64_t result = 0;
	for (auto & item : tblIdxSpaceUsedList) {
		uint64_t sum = item.leafPages + item.intPages + item.ovflPages;
		result += sum;
	}
	return result;
}

TblIdxSpaceUsedList & StoreAnalysisService::getTblIdxSpaceUsedList(uint64_t userDbId)
{
	// Get from cache map
	if (!cacheTblIdxSpaceUsedListMap.empty()) {
		auto iter = cacheTblIdxSpaceUsedListMap.find(userDbId);
		if (iter != cacheTblIdxSpaceUsedListMap.end()) {
			return iter->second;
		}
	}

	TblIdxSpaceUsedList tblIdxSpaceUsedList;
	SqliteSchemaList schemaList = sqliteSchemaUserRepository->getListGtRootpage(userDbId);
	schemaList.push_back({ L"table", L"sqlite_schema", L"sqlite_schema", 0, L"" });
	for (auto & schema : schemaList) {		
		TblIdxSpaceUsed tblIdxSpaceUsed = getRepository()->getTblIdxSpaceUsedByName(userDbId, schema.name);
		tblIdxSpaceUsed.name = schema.name;
		tblIdxSpaceUsed.tblName = schema.tblName;
		tblIdxSpaceUsed.isIndex = int(schema.name != schema.tblName);
		tblIdxSpaceUsed.isWithoutRowid = isWithoutRowid(schema.name);
		tblIdxSpaceUsedList.push_back(tblIdxSpaceUsed);
	}
		
	cacheTblIdxSpaceUsedListMap[userDbId] = tblIdxSpaceUsedList;
	return cacheTblIdxSpaceUsedListMap.at(userDbId);	
}

uint64_t StoreAnalysisService::getUserPayload(uint64_t userDbId)
{
	TblIdxSpaceUsedList & tblIdxSpaceUsedList = getTblIdxSpaceUsedList(userDbId);
	if (tblIdxSpaceUsedList.empty()) {
		return 0;
	}

	uint64_t result = 0;
	for (auto & item : tblIdxSpaceUsedList) {
		if (item.name == L"sqlite_schema") {
			continue;
		}
		uint64_t sum = item.payload;
		result += sum;
	}
	return result;
}

int StoreAnalysisService::isWithoutRowid(std::wstring name)
{
	return 0;
}

double StoreAnalysisService::percent(uint64_t val, uint64_t total)
{
	if (total == 0.0) {
		return 0;
	}
	double dval = val * 100.0 / total;
	dval = round(dval * 100) / 100;
	return dval;
}