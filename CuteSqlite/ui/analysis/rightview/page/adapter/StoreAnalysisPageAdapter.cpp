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

 * @file   StoreAnalysisPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "StoreAnalysisPageAdapter.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

StoreAnalysisPageAdapter::StoreAnalysisPageAdapter(HWND parentHwnd, CWindow * view, StoreAnalysisSupplier * supplier)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	this->supplier = supplier;	
}


StoreAnalysisPageAdapter::~StoreAnalysisPageAdapter()
{
	storeAnalysisService->clearCache(supplier->getRuntimeUserDbId());
}

/**
 * 0.Disk-Space Utilization Report For {dbPath}.
 * 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfDbDiskUsed()
{
	StoreAnalysisItems result;
	DbSpaceUsed & dbSpaceUsed = storeAnalysisService->getDbSpaceUsed(supplier->getRuntimeUserDbId());

	result.push_back({ BYTES_COLOR, S(L"file-bytes-text"), S(L"file-bytes-tips"), std::to_wstring(dbSpaceUsed.fileBytes), 100.0 });
	result.push_back({ BYTES_COLOR, S(L"page-size-text"), S(L"page-size-tips"), std::to_wstring(dbSpaceUsed.pageSize), 100.0 });
	result.push_back({ PAGES_COLOR, S(L"file-pgcnt-text"), S(L"file-pgcnt-tips"), std::to_wstring(dbSpaceUsed.filePgcnt), 100.0 });
	result.push_back({ PAGES_COLOR, S(L"file-pgcnt2-text"), S(L"file-pgcnt2-tips"), std::to_wstring(dbSpaceUsed.filePgcnt2), 100.0 });
	result.push_back({ PAGES_COLOR, S(L"inuse-pgcnt-text"), S(L"inuse-pgcnt-tips"), std::to_wstring(dbSpaceUsed.inusePgcnt), dbSpaceUsed.inusePercent });
	result.push_back({ PAGES_COLOR, S(L"free-pgcnt2-text"), S(L"free-pgcnt2-tips"), std::to_wstring(dbSpaceUsed.freePgcnt2), dbSpaceUsed.freePercent2 });
	result.push_back({ PAGES_COLOR, S(L"free-pgcnt-text"), S(L"free-pgcnt-tips"), std::to_wstring(dbSpaceUsed.freePgcnt), dbSpaceUsed.freePercent });
	result.push_back({ PAGES_COLOR, S(L"av-pgcnt-text"), S(L"av-pgcnt-tips"), std::to_wstring(dbSpaceUsed.avPgcnt), dbSpaceUsed.avPercent });
	result.push_back({ TABLE_COLOR, S(L"ntable-text"), S(L"ntable-tips"), std::to_wstring(dbSpaceUsed.ntable), 100.0 });
	result.push_back({ INDEX_COLOR, S(L"nindex-text"), S(L"nindex-tips"), std::to_wstring(dbSpaceUsed.nindex), 100.0 });
	result.push_back({ INDEX_COLOR, S(L"nmanindex-text"), S(L"nmanindex-tips"), std::to_wstring(dbSpaceUsed.nmanindex), percent(dbSpaceUsed.nmanindex, dbSpaceUsed.nindex) });
	result.push_back({ INDEX_COLOR, S(L"nautoindex-text"), S(L"nautoindex-tips"), std::to_wstring(dbSpaceUsed.nautoindex),  percent(dbSpaceUsed.nautoindex, dbSpaceUsed.nindex) }); 
	result.push_back({ BYTES_COLOR, S(L"user-payload-text"), S(L"user-payload-tips"), std::to_wstring(dbSpaceUsed.userPayload), dbSpaceUsed.userPercent });
	 
	return result;
}


StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfAllTblEntries()
{
	StoreAnalysisItems result;
	TblIdxEntryCntVector list = storeAnalysisService->getSeperateTblEntryCntList(supplier->getRuntimeUserDbId());
	for (auto & item : list) {
		result.push_back({ ENTRY_COLOR, item.name, L"", std::to_wstring(item.entryCnt), item.percent });
	}
	
	return result;
}

/**
 * 2.Page counts for all tables with their indices.
 * 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfAllTblIdxPageCnt()
{
	StoreAnalysisItems result;
	TblIdxPageCntVector list = storeAnalysisService->getAllTblIdxPageCntList(supplier->getRuntimeUserDbId());

	
	for (auto & item : list) {
		result.push_back({ PAGES_COLOR, item.name, L"", std::to_wstring(item.pageCnt), item.percent });
	}
	
	return result;
}

/**
 * 3.Page counts for all tables and indices separately.
 * 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfSeperateTblIdxPageCnt()
{
	StoreAnalysisItems result;
	TblIdxPageCntVector list = storeAnalysisService->getSeperateTblIdxPageCntList(supplier->getRuntimeUserDbId());
	for (auto & item : list) {
		result.push_back({ PAGES_COLOR, item.name, L"", std::to_wstring(item.pageCnt), item.percent });
	}
	
	return result;
}

/**
 * 4.All tables and indices
 * 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfAllTblIdxReport(bool showFrag /* = false */)
{	
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getAllTblAndIdxReport(supplier->getRuntimeUserDbId());
	return reportStoreAnalysisItems(tiSpaceUsed, showFrag);
}

/**
 * 5.All tables report
 * 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfAllTblReport(bool showFrag /*= false*/)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getAllTableReport(supplier->getRuntimeUserDbId());
	return reportStoreAnalysisItems(tiSpaceUsed, showFrag);}

/**
 * 6.All indices.
 * 
 * @param showFrag
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfAllIdxReport(bool showFrag /*= false*/)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getAllIndexReport(supplier->getRuntimeUserDbId());
	return reportStoreAnalysisItems(tiSpaceUsed, showFrag);
}

/**
 * 7.Table {tblName} and all its indices.
 * 
 * @param userDbId
 * @param tblName
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfTblIdxReport(uint64_t userDbId, std::wstring& tblName)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getSpecifiedTblIndexReport(supplier->getRuntimeUserDbId(), tblName);
	return reportStoreAnalysisItems(tiSpaceUsed, false);
}

/**
 * 8.Table $tblName w/o any indices.
 * 
 * @param userDbId
 * @param tblName
 * @param showFrag 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfTblOnlyReport(uint64_t userDbId, std::wstring& tblName, bool showFrag /*= false*/)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getSpecifiedTblOnlyReport(supplier->getRuntimeUserDbId(), tblName);
	return reportStoreAnalysisItems(tiSpaceUsed, showFrag);
}

/**
 * 9.Indices of table {tblName}
 * 
 * @param userDbId
 * @param tblName
 * @param showFrag 
 * @return 
 */
StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfIdxOnlyReport(uint64_t userDbId, const std::wstring & tblName)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getSpecifiedTblThenIdxOnlyReport(supplier->getRuntimeUserDbId(), tblName);
	return reportStoreAnalysisItems(tiSpaceUsed, false);
}


StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfIdxReport(uint64_t userDbId, const std::wstring & tblName, const std::wstring & idxName, bool showFrag /*= false*/)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getSpecifiedIdxReport(supplier->getRuntimeUserDbId(), tblName, idxName);
	return reportStoreAnalysisItems(tiSpaceUsed, showFrag);
}


StoreAnalysisItems StoreAnalysisPageAdapter::getStoreAnalysisItemsOfTblReport(uint64_t userDbId, const std::wstring & tblName)
{
	TblIdxSpaceUsed tiSpaceUsed = storeAnalysisService->getSpecifiedTblReport(supplier->getRuntimeUserDbId(), tblName);
	return reportStoreAnalysisItems(tiSpaceUsed, false);
}


StoreAnalysisItems StoreAnalysisPageAdapter::reportStoreAnalysisItems(TblIdxSpaceUsed &tiSpaceUsed, bool showFrag)
{
	StoreAnalysisItems result;
	if (!tiSpaceUsed.cnt) {
		return result;
	}
	DbSpaceUsed dbSpaceUsed = storeAnalysisService->getDbSpaceUsed(supplier->getRuntimeUserDbId());

	uint64_t totalPages = tiSpaceUsed.leafPages + tiSpaceUsed.intPages + tiSpaceUsed.ovflPages;
	uint64_t filePages = dbSpaceUsed.filePgcnt;
	double totalPagesPercent = percent(totalPages, filePages);
	result.push_back({ BYTES_COLOR, S(L"total-pages-percent-text"), S(L"total-pages-percent-tips"), formatDouble(totalPagesPercent).append(L"%"), totalPagesPercent });
	result.push_back({ ENTRY_COLOR, S(L"nentry-text"), S(L"nentry-tips"), std::to_wstring(tiSpaceUsed.nentry), 100.0 });
	uint64_t storage = totalPages * dbSpaceUsed.pageSize;
	result.push_back({ BYTES_COLOR, S(L"storage-text"), S(L"storage-tips"), std::to_wstring(storage), 100.0 });

	int isCompressed = 0;
	int compressOverhead = 0;
	if (!isCompressed && storage > tiSpaceUsed.compressedSize) {
		isCompressed = 1;
		compressOverhead = 14;
	}
	if (tiSpaceUsed.compressedSize != storage) {
		uint64_t compressedSize = tiSpaceUsed.compressedSize + compressOverhead * totalPages;
		double pct = percent(compressedSize, storage);
		result.push_back({ BYTES_COLOR, S(L"compressed-size-text"), S(L"compressed-size-tips"), std::to_wstring(compressedSize), pct });
	}
	double payloadPercent = percent(tiSpaceUsed.payload, storage);
	result.push_back({ BYTES_COLOR, S(L"payload-text"), S(L"payload-tips"), std::to_wstring(tiSpaceUsed.payload), payloadPercent });

	uint64_t totalUnused = tiSpaceUsed.ovflUnused + tiSpaceUsed.intUnused + tiSpaceUsed.leafUnused;
	uint64_t totalMeta = storage - tiSpaceUsed.payload - totalUnused;
	totalMeta = totalMeta + 4 * (tiSpaceUsed.ovflPages - tiSpaceUsed.ovflCnt);
	double metaPercent = percent(totalMeta, storage);
	result.push_back({ BYTES_COLOR, S(L"total-meta-text"), S(L"total-meta-tips"), std::to_wstring(totalMeta), metaPercent});
	if (tiSpaceUsed.cnt == 1) {
		result.push_back({ DEPTH_COLOR, S(L"depth-text"), S(L"depth-tips"), std::to_wstring(tiSpaceUsed.depth), 100.0 });
	}
	
	double avgPayload = tiSpaceUsed.nentry ? tiSpaceUsed.payload * 1.0 / tiSpaceUsed.nentry : 0.0;
	result.push_back({ BYTES_COLOR, S(L"avg-payload-text"), S(L"avg-payload-tips"), formatDouble(avgPayload), 100.0 });

	double avgUnused = tiSpaceUsed.nentry ? totalUnused * 1.0  / tiSpaceUsed.nentry: 0.0;
	result.push_back({ BYTES_COLOR, S(L"avg-unused-text"), S(L"avg-unused-tips"), formatDouble(avgUnused), 100.0 });

	double avgMeta = tiSpaceUsed.nentry ? totalMeta * 1.0  / tiSpaceUsed.nentry: 0.0;
	result.push_back({ BYTES_COLOR, S(L"avg-meta-text"), S(L"avg-meta-tips"), formatDouble(avgMeta), 100.0 });
	
	// todo ...
	// Get avg_fanout
	bool isHasAvgFanout = false;
	//uint64_t avgFanout = storeAnalysisService->getAllTblAndIdxAvgFanout(supplier->getRuntimeUserDbId());

	if (showFrag && totalPages > 1) {
		double fragmentation = percent(tiSpaceUsed.gapCnt, totalPages - 1);
		result.push_back({ BYTES_COLOR, S(L"gap-cnt-text"), S(L"gap-cnt-tips"), std::to_wstring(tiSpaceUsed.gapCnt),  fragmentation });
	}

	result.push_back({ BYTES_COLOR, S(L"mx-payload-text"), S(L"mx-payload-tips"), std::to_wstring(tiSpaceUsed.mxPayload), 100.0 });

	double ovflCntPercent = percent(tiSpaceUsed.ovflCnt, tiSpaceUsed.nentry);
	result.push_back({ PAGES_COLOR, S(L"ovfl-cnt-text"), S(L"ovfl-cnt-tips"), std::to_wstring(tiSpaceUsed.ovflCnt), ovflCntPercent });

	if (tiSpaceUsed.intPages > 0) {
		result.push_back({ PAGES_COLOR, S(L"int-pages-text"), S(L"int-pages-tips"), std::to_wstring(tiSpaceUsed.intPages), 100.0 });
	}
	result.push_back({ PAGES_COLOR, S(L"leaf-pages-text"), S(L"leaf-pages-tips"), std::to_wstring(tiSpaceUsed.leafPages), 100.0 });
	result.push_back({ PAGES_COLOR, S(L"ovfl-pages-text"), S(L"ovfl-pages-tips"), std::to_wstring(tiSpaceUsed.ovflPages), 100.0 });
	result.push_back({ PAGES_COLOR, S(L"total-pages-text"), S(L"total-pages-tips"), std::to_wstring(totalPages), 100.0 });

	if (tiSpaceUsed.intUnused) {
		double intUnusedPercent = percent(tiSpaceUsed.intUnused, tiSpaceUsed.intPages * dbSpaceUsed.pageSize);
		result.push_back({ BYTES_COLOR, S(L"int-unused-text"), S(L"int-unused-tips"), std::to_wstring(tiSpaceUsed.intUnused), intUnusedPercent });
	}

	double leafUnusedPercent = percent(tiSpaceUsed.leafUnused, tiSpaceUsed.leafPages * dbSpaceUsed.pageSize);
	result.push_back({ BYTES_COLOR, S(L"leaf-unused-text"), S(L"leaf-unused-tips"), std::to_wstring(tiSpaceUsed.leafUnused), leafUnusedPercent });

	double ovflUnusedPercent = percent(tiSpaceUsed.ovflUnused, tiSpaceUsed.ovflPages * dbSpaceUsed.pageSize);
	result.push_back({ BYTES_COLOR, S(L"ovfl-unused-text"), S(L"ovfl-unused-tips"), std::to_wstring(tiSpaceUsed.ovflUnused), ovflUnusedPercent });

	double totalUnusedPercent = percent(totalUnused, storage);
	result.push_back({ BYTES_COLOR, S(L"total-unused-text"), S(L"total-unused-tips"), std::to_wstring(totalUnused), totalUnusedPercent });

	return result;
}

double StoreAnalysisPageAdapter::percent(uint64_t val, uint64_t total)
{
	if (total == 0.0) {
		return 0;
	}
	double dval = val * 100.0 / total;
	return ((double)((int64_t)((dval + 0.005) * 100 ))) / 100;
}

std::wstring StoreAnalysisPageAdapter::formatDouble(double val)
{
	if (val == 0.0) {
		return 0;
	}
	val = round(val * 100) / 100;
	wchar_t buff[128];
	swprintf_s(buff, 128, L"%.2f", val);
	return std::wstring(buff);
}

