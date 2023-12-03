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

 * @file   PerfAnalysisPage.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-03
 *********************************************************************/
#pragma once
#pragma once
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/common/button/QImageButton.h"
#include "common/Config.h"
#include "core/entity/Entity.h"
#include "core/service/sqllog/SqlLogService.h"

class PerfAnalysisPage : public QTabPage<PerfAnalysisPage>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(PerfAnalysisPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	PerfAnalysisPage(uint64_t sqlLogId);
	uint64_t getSqlLogId();
private:
	bool isNeedReload = true;
	uint64_t sqlLogId = 0;
	SqlLog sqlLog;

	COLORREF bkgColor = RGB(238, 238, 238);	
	COLORREF textColor = RGB(64, 64, 64);
	COLORREF sectionColor = RGB(8, 8, 8);
	CBrush bkgBrush;
	HFONT textFont = nullptr;
	HFONT sectionFont = nullptr;
	
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	void createOrShowUI();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
};
