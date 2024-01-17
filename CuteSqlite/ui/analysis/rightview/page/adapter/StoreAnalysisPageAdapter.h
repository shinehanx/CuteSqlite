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

 * @file   StoreAnalysisPageAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "ui/analysis/rightview/page/supplier/StoreAnalysisSupplier.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/analysis/StoreAnalysisService.h"

class StoreAnalysisPageAdapter : public QAdapter<StoreAnalysisPageAdapter, CWindow>
{
public:
	StoreAnalysisPageAdapter(HWND parentHwnd, CWindow * view, StoreAnalysisSupplier * supplier = nullptr);
	~StoreAnalysisPageAdapter();

	StoreAnalysisItems getStoreAnalysisItemsOfDbDiskUsed();
	StoreAnalysisItems getStoreAnalysisItemsOfAllTblIdxPageCnt();
	StoreAnalysisItems getStoreAnalysisItemsOfSeperateTblIdxPageCnt();
	StoreAnalysisItems getStoreAnalysisItemsOfAllTblIdxReport(bool showFrag = false);
private:
	const COLORREF PAGES_COLOR = RGB(49, 139, 202);
	const COLORREF BYTES_COLOR = RGB(112, 146, 190);
	const COLORREF TABLE_COLOR = RGB(0, 128, 0);
	const COLORREF INDEX_COLOR = RGB(255, 128, 0);
	const COLORREF DEPTH_COLOR = RGB(0, 0, 160);
	StoreAnalysisSupplier * supplier = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();
	StoreAnalysisService * storeAnalysisService = StoreAnalysisService::getInstance();
	double percent(uint64_t val, uint64_t total);
};
