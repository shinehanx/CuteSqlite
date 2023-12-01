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
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"

LeftNaigationViewAdapter::LeftNaigationViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
	this->dataView->SetImageList(imageList);
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

	if (!imageList.IsNull()) imageList.Destroy();
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

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(perfAnalysisIcon); // 0 - performance analysis	
	imageList.AddIcon(storeAnalysisIcon); // 1- store analysis
	imageList.AddIcon(dbParamsIcon); // 2- database params 
	imageList.AddIcon(folderIcon); // 3  - folder	
	imageList.AddIcon(sqlLogIcon); // 4- sql log
	imageList.AddIcon(analysisReportIcon); // 5- analysis report
	
	
}

void LeftNaigationViewAdapter::loadTreeView()
{
	dataView->DeleteAllItems();
	HTREEITEM hPerfAnalysisItem = dataView->InsertItem(S(L"perf-analysis").c_str(), 0, 0, NULL, TVI_LAST);
	HTREEITEM hStoreAnalysisItem = dataView->InsertItem(S(L"store-analysis").c_str(), 1, 1, NULL, TVI_LAST);
	HTREEITEM hDbParamsItem = dataView->InsertItem(S(L"db-params").c_str(), 2, 2, NULL, TVI_LAST);

	// Performance Analysis
	HTREEITEM hSqlLogItem = dataView->InsertItem(S(L"sql-log").c_str(), 4, 4, hPerfAnalysisItem, TVI_LAST);
	HTREEITEM hAnalysisReportItem = dataView->InsertItem(S(L"perf-analysis-reports").c_str(), 3, 3, hPerfAnalysisItem, TVI_LAST);
	HTREEITEM hReportItem = dataView->InsertItem(L"report-1", 5, 5, hAnalysisReportItem, TVI_LAST);

	// Store Analysis
	
	dataView->Expand(hPerfAnalysisItem);
	dataView->Expand(hAnalysisReportItem);
}


