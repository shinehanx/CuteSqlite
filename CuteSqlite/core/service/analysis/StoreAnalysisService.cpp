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
		item.percent = static_cast<float>(item.pageCnt * 100.0 / filePageCount);
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
		item.percent = static_cast<float>(item.pageCnt * 100.0 / filePageCount);
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

	return result;
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
