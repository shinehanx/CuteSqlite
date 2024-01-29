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

 * @file   TablePropertiesPage.cpp
 * @brief  Table Properties
 * 
 * @author Xuehan Qin
 * @date   2023-11-06
 *********************************************************************/
#pragma once
// for table structure
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/button/QImageButton.h"
// for table analysis
#include "core/service/analysis/StoreAnalysisService.h"
#include "ui/analysis/rightview/page/adapter/StoreAnalysisPageAdapter.h"
#include "ui/analysis/rightview/page/elem/StoreAnalysisElem.h"

class TablePropertiesPage : public QTabPage<QueryPageSupplier> {
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(TablePropertiesPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		COMMAND_HANDLER_EX(Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshButton)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void setup(QueryPageSupplier * supplier);

	void active();
	bool getIsCreateOrShow() const { return isCreateOrShow; }
	void setIsCreateOrShow(bool val) { isCreateOrShow = val; }
protected:
	bool isCreateOrShow = false;
	COLORREF lineColor = RGB(127, 225, 244);
	COLORREF headerBkgColor = RGB(81, 81, 81);
	COLORREF headerTextColor = RGB(255, 255, 255);
	COLORREF doubleRowColor = RGB(238, 238, 238);
	HICON primaryIcon = nullptr;

	HFONT titleFont = nullptr;
	HFONT sectionFont = nullptr;
	HFONT textFont = nullptr;
	CBrush headerBrush;
	CBrush doubleRowBrush;
	CPen linePen;

	QStaticImage titleImage;
	CEdit titleEdit;
	QImageButton refreshButton;

	// columns section
	CEdit columnsSection;
	CEdit primaryKeyHeader, columnNameHeader, columnTypeHeader;
	std::vector<CEdit *> primaryKeyPtrs;
	std::vector<CEdit *> columnNamePtrs;
	std::vector<CEdit *> columnTypePtrs;
	
	// indexes section
	CEdit indexesSection;
	CEdit idxPrimaryHeader, idxNameHeader, idxColumnsHeader, idxTypeHeader;
	std::vector<CEdit *> idxPrimaryPtrs;
	std::vector<CEdit *> idxNamePtrs;
	std::vector<CEdit *> idxColumnsPtrs;
	std::vector<CEdit *> idxTypePtrs;

	CEdit foreignKeySection;
	CEdit frkNameHeader, frkColumnsHeader, frkReferedTableHeader, 
		frkReferedColumnsHeader, frkOnUpdateHeader, frkOnDeleteHeader;
	std::vector<CEdit *> frkNamePtrs, frkColumnsPtrs, frkReferedTablePtrs, 
		frkReferedColumnsPtrs, frkOnUpdatePtrs, frkOnDeletePtrs;

	 //store CEdit points of the primary key 
	std::vector<std::pair<CEdit *, int>> primaryBitmapPtrs;
	// ddl section
	CEdit ddlSection;
	CEdit ddlHeader;
	CEdit ddlContent;
	// scroll bar
	TEXTMETRIC tm;
	SCROLLINFO si;
	int nHeightSum = 0;
	int nWidthSum = 0;
	int cxChar = 5, cyChar = 5, 
		iVscrollPos = 5, vScrollPages = 5,
		iHscrollPos = 5, hScrollPages = 5;

	void initVScrollBar(CSize & clientSize);
	void initHScrollBar(CSize & clientSize);

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();

	// For table analysis
	std::vector<StoreAnalysisElem *> storeAnalysisElemPtrs;
	StoreAnalysisService * storeAnalysisService = StoreAnalysisService::getInstance();
	StoreAnalysisSupplier storeAnalysisSupplier;
	StoreAnalysisPageAdapter * storeAnalysisAdapter = nullptr;
	
	virtual void createOrShowUI();
	virtual void loadWindow();

	// for table structure
	void createOrShowTitleElems(CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect);
	void createOrShowColumnsSectionElems(CRect & clientRect);
	void createOrShowColumnsList(CRect & clientRect);
	void createOrShowIndexesSectionElems(CRect & clientRect);
	void createOrShowIndexesList(CRect & clientRect);
	void createOrShowForeignKeySectionElems(CRect & clientRect);
	void createOrShowForeignKeyList(CRect & clientRect);
	void createOrShowDdlSectionElems(CRect & clientRect);

	// for table analysis
	void createOrShowTableStoreAnalysisElems(CRect & clientRect);
	void createOrShowStoreAnalysisElem(StoreAnalysisElem &win, CRect & rect, CRect clientRect);

	virtual void paintItem(CDC & dc, CRect & paintRect);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	size_t inEditArray(HWND hwnd, const std::vector<CEdit *> & ptrs);	
	void destroyPtrs(std::vector<CEdit *> & ptrs);
	void drawPrimaryKey(CDC & dc, CRect & rect, int nItem);

	// for table analysis
	StoreAnalysisElem * getStoreAnalysisElemPtr(const std::wstring & title);
	void clearStoreAnalysisElemPtrs();
	// Invalidate the all of QHorizontalBar window in the all StoreAnalysisElem
	void updateSubWindow();
};
