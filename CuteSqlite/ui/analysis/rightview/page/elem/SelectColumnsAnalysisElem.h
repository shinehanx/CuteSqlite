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

 * @file   WhereAnalysisTableIdxElem.h
 * @brief  table index elem UI used for where clause or order by clause analysis in PerAnalysisPage
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
#include "core/service/system/SettingService.h"

#define MAX_ADVICE_COLUMNS_COUNT 20

class SelectColumnsAnalysisElem : public CWindowImpl<SelectColumnsAnalysisElem>
{
public:
	BEGIN_MSG_MAP_EX(SelectColumnsAnalysisElem)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	SelectColumnsAnalysisElem(const SelectColumns & _selectColumns);
	const SelectColumns & getSelectColumns() const { return selectColumns; }
	const Columns getSelectedColumns();
private:
	bool isNeedReload = true;
	const SelectColumns & selectColumns;

	//COLORREF bkgColor = RGB(238, 238, 238);	
	COLORREF bkgColor = RGB(255, 255, 255);
	COLORREF textColor = RGB(64, 64, 64);
	COLORREF hintColor = RGB(71, 109, 208);
	
	COLORREF btnBkgColor = RGB(210, 210, 210);
	CBrush bkgBrush;
	HFONT textFont = nullptr;

	CStatic useColsLabel;
	std::vector<CButton *> selectColumnCheckBoxPtrs;
	CStatic useColsCountLabel;
	CStatic adviceLabel;

	SettingService * settingService = SettingService::getInstance();

	void createOrShowUI();
	void createOrShowLabels(CRect & clientRect);
	void createOrShowCheckBoxes(CRect & clientRect);
	void createOrShowColumnCountElems(CRect & clientRect);
	
	void clearSelectColumnCheckBoxPtrs();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	
};
