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

 * @file   QSqlEdit.h
 * @brief  SQL statement editor, hightlit the sql tags, and show the line nums
 * 
 * @author Xuehan Qin
 * @date   2023-05-24
 *********************************************************************/
#pragma once
#include <string>
#include <vector>
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>

const COLORREF black = RGB(0, 0, 0);
const COLORREF white = RGB(0xff, 0xff, 0xff);

class QSqlEdit : public CWindowImpl<QSqlEdit> 
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QSqlEdit)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		// REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	HWND Create(HWND hWndParent, const ATL::_U_RECT& rect, DWORD dwExStyle = 0, UINT nID = 0);

	void initEdit(int nSize, const TCHAR* face);
	int getMarginWidthN(int margin);
	void setMarginWidthN(int margin, int mask);
	long lineFromPosition(long pos);
	void toggleFold(long line);
	void updateLineNumberWidth(void);

	std::wstring getSelText();
	std::wstring getText();
	std::wstring getCurLineText();
	std::wstring getCurPreLineText();
	std::wstring getCurWord();
	size_t getCurPosInLine();

	// replace selected text to editor
	void replaceSelText(std::wstring & text);
	void setText(const std::wstring & text);
	void addText(const std::wstring & text);
	void clearText();

	// SCI_AUTOSHOW
	void autoShow(size_t lengthEntered, const std::vector<std::wstring> & tags);
	void autoComplete();
	void autoReplaceWord();
	
protected:

	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;

	void init();
	void setDefaultColorFont(int nSize, const TCHAR* face);
	//×Ô¶¯Fold
	void setFold(BOOL bFold = TRUE);
	
	void setSqlSyntax();
	void createOrShowUI();	
	void crateOrShowEditor(CRect &clientRect);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, CSize size);
};
