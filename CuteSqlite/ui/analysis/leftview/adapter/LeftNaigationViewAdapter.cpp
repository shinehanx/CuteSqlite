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

 * @file   LeftNaigationViewAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "LeftNaigationViewAdapter.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "utils/Log.h"
#include "utils/SqlUtil.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/MenuUtil.h"
#include "common/AppContext.h"

LeftNaigationViewAdapter::LeftNaigationViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
	this->dataView->SetImageList(imageList);

	createPerfReportMenu();
}


LeftNaigationViewAdapter::~LeftNaigationViewAdapter()
{
	if (imageList.IsNull()) imageList.Destroy();
	if (perfAnalysisIcon) ::DeleteObject(perfAnalysisIcon);
	if (storeAnalysisIcon) ::DeleteObject(storeAnalysisIcon);
	if (dbParamsIcon) ::DeleteObject(dbParamsIcon);
	if (folderIcon) ::DeleteObject(folderIcon);
	if (sqlLogIcon) ::DeleteObject(sqlLogIcon);
	if (analysisReportIcon) ::DeleteObject(analysisReportIcon);
	if (analysisReportDirtyIcon) ::DeleteObject(analysisReportDirtyIcon);
	if (dbStoreAnalysisIcon) ::DeleteObject(dbStoreAnalysisIcon);
	if (subDbParamsIcon) ::DeleteObject(subDbParamsIcon);

	if (openPerfReportIcon) ::DeleteObject(openPerfReportIcon);
	if (dropPerfReportIcon) ::DeleteObject(dropPerfReportIcon);

	if (!imageList.IsNull()) imageList.Destroy();
	if (!perfReportMenu.IsNull()) perfReportMenu.DestroyMenu();
}

void LeftNaigationViewAdapter::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	perfAnalysisIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\perf-analysis.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	storeAnalysisIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\store-analysis.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dbParamsIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\db-params.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	folderIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\folder.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	sqlLogIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\sql-log.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	analysisReportIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\analysis-report.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	analysisReportDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\analysis-report-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dbStoreAnalysisIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE); 
	subDbParamsIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE); 

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(perfAnalysisIcon); // 0 - performance analysis	
	imageList.AddIcon(storeAnalysisIcon); // 1- store analysis
	imageList.AddIcon(dbParamsIcon); // 2- database params 
	imageList.AddIcon(folderIcon); // 3  - folder	
	imageList.AddIcon(sqlLogIcon); // 4- sql log
	imageList.AddIcon(analysisReportIcon); // 5- perf analysis report
	imageList.AddIcon(analysisReportDirtyIcon); // 6- dirty perf analysis report 
	imageList.AddIcon(dbStoreAnalysisIcon); // 7- database store analysis 
	imageList.AddIcon(subDbParamsIcon); // 8- database params
	
	openPerfReportIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\open-perf-report.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropPerfReportIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\drop-perf-report.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}



void LeftNaigationViewAdapter::loadTreeView()
{
	dataView->DeleteAllItems();
	hPerfAnalysisItem = dataView->InsertItem(S(L"perf-analysis").c_str(), 0, 0, NULL, TVI_LAST);
	hStoreAnalysisItem = dataView->InsertItem(S(L"store-analysis").c_str(), 1, 1, NULL, TVI_LAST);
	hDbParamsItem = dataView->InsertItem(S(L"db-params").c_str(), 2, 2, NULL, TVI_LAST);

	// Performance Analysis
	hSqlLogItem = dataView->InsertItem(S(L"sql-log").c_str(), 4, 4, hPerfAnalysisItem, TVI_LAST);
	hPerfReportsFolderItem = dataView->InsertItem(S(L"perf-analysis-reports").c_str(), 3, 3, hPerfAnalysisItem, TVI_LAST);
	loadReportsForPerfReportsFolder();
	loadDatabseForStoreAnalysisItem();
	loadDatabseForDbParamsItem();
	
	// Store Analysis	
	dataView->Expand(hPerfAnalysisItem);
	dataView->Expand(hPerfReportsFolderItem);
	dataView->Expand(hStoreAnalysisItem);
	dataView->Expand(hDbParamsItem);
}

void LeftNaigationViewAdapter::loadReportsForPerfReportsFolder()
{
	PerfAnalysisReportList reportList;
	try {
		reportList = selectSqlAnalysisService->getPerfAnalysisReportList();
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}

	for (auto & report : reportList) {
		addPerfAnalysisReport(report.userDbId, report.sqlLogId, true);
	}
}

void LeftNaigationViewAdapter::addPerfAnalysisReport(uint64_t userDbId, uint64_t sqlLogId, bool isSaved)
{
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		return ;
	}
	SqlLog sqlLog;
	try {
		sqlLog = sqlLogService->getSqlLog(sqlLogId);
		if (sqlLog.sql.empty()) {
			Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
			return ;
		}
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}
	

	if (!SqlUtil::isSelectSql(sqlLog.sql)) {
		return ;
	}

	std::wstring title = S(L"sql-perf-report-prefix");
	title.append(std::to_wstring(sqlLogId));

	CTreeItem childItem = hPerfReportsFolderItem.GetChild();
	bool isFound = false;
	while (!childItem.IsNull()) {
		uint64_t itemData = static_cast<uint64_t>(childItem.GetData());
		if (itemData == sqlLogId) {
			isFound = true;
			break;
		}
		childItem = childItem.GetNextSibling();
	}
	if (isFound) {
		childItem.Select();
		return;
	}
	int iImage = isSaved ? 5 : 6; // 5 - 
	CTreeItem reportTreeItem = dataView->InsertItem(title.c_str(), iImage, iImage, hPerfReportsFolderItem, TVI_LAST);
	reportTreeItem.SetData(sqlLogId);
	reportTreeItem.Select();
}

void LeftNaigationViewAdapter::loadDatabseForStoreAnalysisItem()
{
	UserDbList dbs;
	try {
		dbs = databaseService->getAllUserDbs();
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}

	for (auto & db : dbs) {
		addDatabaseToStoreAnalysisItem(db);
	}
}


void LeftNaigationViewAdapter::loadDatabseForDbParamsItem()
{
	UserDbList dbs;
	try {
		dbs = databaseService->getAllUserDbs();
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}

	for (auto & db : dbs) {
		addDatabaseToDbParamsItem(db);
	}
}

void LeftNaigationViewAdapter::addDatabaseToStoreAnalysisItem(UserDb & userDb)
{
	ATLASSERT(userDb.id);

	std::wstring title = userDb.name;
	CTreeItem childItem = hStoreAnalysisItem.GetChild();
	bool isFound = false;
	while (!childItem.IsNull()) {
		uint64_t itemData = static_cast<uint64_t>(childItem.GetData());
		if (itemData == userDb.id) {
			isFound = true;
			break;
		}
		childItem = childItem.GetNextSibling();
	}
	if (isFound) {
		return;
	}
	int iImage = 7; // 7 - database store analysis
	CTreeItem treeItem = dataView->InsertItem(title.c_str(), iImage, iImage, hStoreAnalysisItem, TVI_LAST);
	treeItem.SetData(userDb.id);
}


void LeftNaigationViewAdapter::addDatabaseToDbParamsItem(UserDb & userDb)
{
	ATLASSERT(userDb.id);

	std::wstring title = userDb.name;
	CTreeItem childItem = hDbParamsItem.GetChild();
	bool isFound = false;
	while (!childItem.IsNull()) {
		uint64_t itemData = static_cast<uint64_t>(childItem.GetData());
		if (itemData == userDb.id) {
			isFound = true;
			break;
		}
		childItem = childItem.GetNextSibling();
	}
	if (isFound) {
		return;
	}
	int iImage = 8; // 8 - database params
	CTreeItem treeItem = dataView->InsertItem(title.c_str(), iImage, iImage, hDbParamsItem, TVI_LAST);
	treeItem.SetData(userDb.id);
}

void LeftNaigationViewAdapter::savePerfAnalysisReport(uint64_t sqlLogId)
{
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		return ;
	}
	SqlLog sqlLog;
	try {
		sqlLog = sqlLogService->getSqlLog(sqlLogId);
		if (sqlLog.sql.empty()) {
			Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
			return ;
		}
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}

	CTreeItem childItem = hPerfReportsFolderItem.GetChild();
	bool isFound = false;
	while (!childItem.IsNull()) {
		uint64_t itemData = static_cast<uint64_t>(childItem.GetData());
		if (itemData == sqlLogId) {
			isFound = true;
			break;
		}
		childItem = childItem.GetNextSibling();
	}

	if (isFound) {
		childItem.SetImage(5, 5);
		return;
	}
}

void LeftNaigationViewAdapter::createPerfReportMenu()
{
	perfReportMenu.CreatePopupMenu();
	perfReportMenu.AppendMenu(MF_STRING, Config::ANALYSIS_OPEN_PERF_REPORT_MENU_ID, S(L"open-perf-report").c_str());
	perfReportMenu.AppendMenu(MF_STRING, Config::ANALYSIS_DROP_PERF_REPORT_MENU_ID, S(L"drop-perf-report").c_str());
	

	initMenuInfo(perfReportMenu.m_hMenu);

	MenuUtil::addIconToMenuItem(perfReportMenu.m_hMenu, Config::ANALYSIS_OPEN_PERF_REPORT_MENU_ID, MF_BYCOMMAND, openPerfReportIcon);
	MenuUtil::addIconToMenuItem(perfReportMenu.m_hMenu, Config::ANALYSIS_DROP_PERF_REPORT_MENU_ID, MF_BYCOMMAND, dropPerfReportIcon);
	
}


void LeftNaigationViewAdapter::popupPerfReportMenu(CPoint pt)
{
	perfReportMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

void LeftNaigationViewAdapter::openPerfAnalysisReport(uint64_t sqlLogId)
{
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		return ;
	}
	SqlLog sqlLog;
	try {
		sqlLog = sqlLogService->getSqlLog(sqlLogId);
		if (sqlLog.sql.empty()) {
			Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
			return ;
		}
		AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_SQL_ID, WPARAM(sqlLog.userDbId), LPARAM(sqlLogId));
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}	
}

void LeftNaigationViewAdapter::dropPerfAnalysisReport(uint64_t sqlLogId)
{
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		return ;
	}
	SqlLog sqlLog;
	try {
		sqlLog = sqlLogService->getSqlLog(sqlLogId);
		if (sqlLog.sql.empty()) {
			Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
			return ;
		}

		selectSqlAnalysisService->dropPerfAnalysisReport(sqlLogId);
		AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_DROP_PERF_REPORT_ID, WPARAM(sqlLog.userDbId), LPARAM(sqlLogId));
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
		return;
	}
}



