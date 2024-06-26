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

 * @file   LeftNaigationViewAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/sqllog/SqlLogService.h"
#include "core/service/analysis/SelectSqlAnalysisService.h"
#include "core/service/db/DatabaseService.h"

class LeftNaigationViewAdapter : public QAdapter<LeftNaigationViewAdapter, CTreeViewCtrlEx>
{
public:
	LeftNaigationViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view);
	~LeftNaigationViewAdapter();
	void loadTreeView();
	void addPerfAnalysisReport(uint64_t userDbId, uint64_t sqlLogId, bool isSaved = false);
	void savePerfAnalysisReport(uint64_t sqlLogId);
	void openPerfAnalysisReport(uint64_t sqlLogId);
	void dropPerfAnalysisReport(uint64_t sqlLogId);

	void popupPerfReportMenu(CPoint pt);

	void addDbTreeItem(uint64_t userDbId);
	void removeDbTreeItem(uint64_t userDbId);
	void dirtyDbPragmaParamTreeItem(uint64_t userDbId, bool isDirty);
	void dirtyDbQuickConfigTreeItem(uint64_t userDbId, bool isDirty);
private:
	HICON perfAnalysisIcon = nullptr;
	HICON storeAnalysisIcon = nullptr;
	HICON dbParamsIcon = nullptr;
	HICON folderIcon = nullptr;
	HICON sqlLogIcon = nullptr;
	HICON analysisReportIcon = nullptr;
	HICON analysisReportDirtyIcon = nullptr;
	HICON dbStoreAnalysisIcon = nullptr;
	HICON subDbParamsIcon = nullptr;
	HICON subPragmaParamsIcon = nullptr;
	HICON subPragmaParamsDirtyIcon = nullptr;
	HICON subQuickConfigParamsIcon = nullptr;
	HICON subQuickConfigParamsDirtyIcon = nullptr;

	HICON openPerfReportIcon = nullptr;
	HICON dropPerfReportIcon = nullptr;

	CTreeItem hPerfAnalysisItem;
	CTreeItem hStoreAnalysisItem;
	CTreeItem hDbParamsItem;

	CTreeItem hSqlLogItem;
	CTreeItem hPerfReportsFolderItem;
	
	CImageList imageList;

	CMenu perfReportMenu;

	SqlLogService * sqlLogService = SqlLogService::getInstance();
	SelectSqlAnalysisService * selectSqlAnalysisService = SelectSqlAnalysisService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();

	void createImageList();
	void loadReportsForPerfReportsFolder();
	void loadDatabseForStoreAnalysisItem();
	void loadDatabseForDbParamsItem();
	void createPerfReportMenu();
	void addDatabaseToStoreAnalysisItem(UserDb & userDb);
	void addDatabaseToDbParamsItem(UserDb & userDb);
};
