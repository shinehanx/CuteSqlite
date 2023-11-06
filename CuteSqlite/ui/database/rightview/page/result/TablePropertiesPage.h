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

 * @file   ResultPropertiesPage.h
 * @brief  Execute sql statement and show the info of query result
 * 
 * @author Xuehan Qin
 * @date   2023-11-06
 *********************************************************************/
#pragma once
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/database/rightview/page/supply/QueryPageSupplier.h"
#include "ui/common/image/QStaticImage.h"

class TablePropertiesPage : public QTabPage<QueryPageSupplier> {
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(TablePropertiesPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)		
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void setup(QueryPageSupplier * supplier);

	void clear();
protected:
	COLORREF lineColor = RGB(127, 225, 244);
	COLORREF headerBkgColor = RGB(127, 225, 244);
	COLORREF headerTextColor = RGB(50, 50, 50);
	COLORREF doubleRowColor = RGB(238, 238, 238);

	HFONT titleFont = nullptr;
	HFONT sectionFont = nullptr;
	HFONT textFont = nullptr;
	HBRUSH headerBrush = nullptr;
	HBRUSH doubleRowBrush = nullptr;
	CPen linePen;

	QStaticImage titleImage;
	CEdit titleEdit;

	std::vector<CEdit *> headerPtrs;
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

	// ddl section
	CEdit ddlSection;
	CEdit ddlHeader;
	CEdit ddlContent;

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();

	virtual void createOrShowUI();
	virtual void loadWindow();

	void createOrShowTitleElems(CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect);
	void createOrShowColumnsSectionElems(CRect & clientRect);
	void createOrShowColumnsList(CRect & clientRect);
	void createOrShowIndexesSectionElems(CRect & clientRect);
	void createOrShowIndexesList(CRect & clientRect);

	void createOrShowDdlSectionElems(CRect & clientRect);

	virtual void paintItem(CDC & dc, CRect & paintRect);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	size_t inEditArray(HWND hwnd, const std::vector<CEdit *> & ptrs);
private:
	
};
