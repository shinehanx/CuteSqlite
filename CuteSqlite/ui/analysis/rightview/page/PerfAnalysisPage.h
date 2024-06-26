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
#include "ui/common/edit/QSqlEdit.h"
#include "ui/analysis/rightview/page/supplier/PerfAnalysisSupplier.h"
#include "ui/analysis/rightview/page/adapter/PerfAnalysisPageAdapter.h"
#include "ui/analysis/rightview/page/elem/WhereOrderClauseAnalysisElem.h"
#include "ui/analysis/rightview/page/elem/TableJoinAnalysisElem.h"
#include "ui/analysis/rightview/page/elem/SelectColumnsAnalysisElem.h"
#include "ui/common/image/QStaticImage.h"

class PerfAnalysisPage : public QTabPage<PerfAnalysisSupplier>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(PerfAnalysisPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		COMMAND_HANDLER_EX(Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshButton)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_CREATE_INDEX_ID, OnClickCreateIdxButton)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLORBTN(OnCtlBtnColor)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	PerfAnalysisPage(uint64_t sqlLogId);
	~PerfAnalysisPage();
	uint64_t getSqlLogId();
	void save();
	bool isSaved();
private:
	bool isNeedReload = true;
	
	COLORREF textColor = RGB(64, 64, 64);
	COLORREF sectionColor = RGB(8, 8, 8);
	COLORREF btnBkgColor = RGB(210, 210, 210);

	COLORREF lineColor = RGB(127, 225, 244);
	COLORREF headerBkgColor = RGB(81, 81, 81);
	COLORREF headerTextColor = RGB(255, 255, 255);

	HFONT titleFont = nullptr;
	HFONT textFont = nullptr;
	HFONT sectionFont = nullptr;

	CPen linePen;
	CBrush headerBrush;

	QStaticImage titleImage;
	CEdit titleEdit;
	QImageButton refreshButton;

	// orignal sql statement
	CStatic origSqlLabel;
	QSqlEdit origSqlEditor;

	// Explain query plan
	CStatic explainQueryPlanLabel;
	std::vector<CStatic *> expQueryPlanPtrs;

	// WHERE clause analysis
	CStatic whereAnalysisLabel;
	std::vector<WhereOrderClauseAnalysisElem *> whereAnalysisElemPtrs;

	// ORDER clause analysis
	CStatic orderAnalysisLabel;
	std::vector<WhereOrderClauseAnalysisElem *> orderAnalysisElemPtrs;

	// Covering Indexes
	CStatic coveringIndexLabel;
	std::vector<WhereOrderClauseAnalysisElem *> coveringIndexElemPtrs;

	// select columns analysis
	CStatic selectColumnsAnalysisLabel;
	SelectColumnsAnalysisElem * selectColumnsAnalysisElem = nullptr;
	
	PerfAnalysisPageAdapter * adapter = nullptr;
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	CStatic tableJoinAnalysisLabel;
	TableJoinAnalysisElem * tableJoinAnalysisElem = nullptr;

	// scroll bar
	TEXTMETRIC tm;
	SCROLLINFO si;
	int nHeightSum = 0;
	int cxChar = 5, cyChar = 5, iVscrollPos = 5, vScrollPages = 5;
	void initScrollBar(CSize & clientSize);

	virtual void createOrShowUI();
	void createOrShowTitleElems(CRect & clientRect);
	void createOrShowOrigSqlEditor(CRect &clientRect);
	void crateOrShowEditor(QSqlEdit &win, CRect &rect, CRect &clientRect);
	void createOrShowExpQueryPlanElems(CRect &clientRect);
	void createOrShowWhereAnalysisElems(CRect &clientRect);
	void createOrShowWhereAnalysisItemsForTable(CRect &clientRect);
	void createOrShowOrderAnalysisElems(CRect &clientRect);
	void createOrShowOrderAnalysisItemsForTable(CRect &clientRect);
	void createOrShowCoveringIndexesElems(CRect &clientRect);
	
	void createOrShowCoveringIndexItemsForTable(CRect &clientRect);
	void createOrShowTableJoinAnalysisElems(CRect &clientRect);
	void createOrShowTableJoinAnalysisElem(TableJoinAnalysisElem & win, CRect & rect, CRect & clientRect);
	void createOrShowClauseAnalysisElem(WhereOrderClauseAnalysisElem & win, CRect & rect, CRect & clientRect);	
	void createOrShowSelectColumnsAnalysisElems(CRect &clientRect);
	void createOrShowSelectColumnsAnalysisElem(SelectColumnsAnalysisElem & win, CRect & rect, CRect & clientRect);	
	void createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	

	void clearExpQueryPlanPtrs();
	void clearWhereAnalysisElemPtrs();	
	void clearOrderAnalysisElemPtrs();
	void clearCoveringIndexElemPtrs();

	virtual void loadWindow();
	void loadOrigSqlEditor();
	

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickCreateIdxButton(UINT uMsg, WPARAM wParam, LPARAM lParam);

	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlBtnColor(HDC hdc, HWND hwnd);
};
