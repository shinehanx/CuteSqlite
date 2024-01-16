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

 * @file   TableJoinAnalysisElem.h
 * @brief  table join elem UI used for joins clause analysis in PerAnalysisPage
 * 
 * @author Xuehan Qin
 * @date   2023-12-08
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "common/Config.h"
#include "core/entity/Entity.h"
#include "core/service/db/TableService.h"

class TableJoinAnalysisElem : public CWindowImpl<TableJoinAnalysisElem>
{
public:
	BEGIN_MSG_MAP_EX(TableJoinAnalysisElem)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	TableJoinAnalysisElem(const std::wstring &_selectColumnStr, const ByteCodeResults & _byteCodeResults);
	~TableJoinAnalysisElem();
	const ByteCodeResults & getByteCodeResults() const { return byteCodeResults; }
private:
	bool isNeedReload = true;
	const ByteCodeResults & byteCodeResults;
	const std::wstring selectColumnStr;

	//COLORREF bkgColor = RGB(238, 238, 238);	
	COLORREF bkgColor = RGB(255, 255, 255);
	COLORREF textColor = RGB(64, 64, 64);
	COLORREF hintColor = RGB(64, 64, 64);
	
	CBrush bkgBrush;
	HFONT textFont = nullptr;

	CStatic tableJoinLabel;
	CEdit pseudocodeEdit;
	CStatic cycleCountLabel;
	CStatic optimizeAdviceLabel;	

	TableService * tableService = TableService::getInstance();

	void createOrShowUI();
	void createOrShowLabelsAndEdits(CRect & clientRect);
	
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	
	std::wstring getTableJoinPseudocode();
	uint64_t getTableJoinCycleCount();
};
