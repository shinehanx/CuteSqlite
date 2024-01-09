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

 * @file   QHelpEdit.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-24
 *********************************************************************/
#pragma once

#pragma once
#include <string>
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "ui/common/page/QHelpPage.h"
#include "QSqlEdit.h"
#include "adapter/QHelpEditAdapter.h"

class QHelpEdit : public QHelpPage
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QHelpEdit)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		CHAIN_MSG_MAP(QHelpPage)
		// REFLECT_NOTIFICATIONS() 
	END_MSG_MAP()

	void setup(std::wstring & helpText, std::wstring & content, QHelpEditAdapter * adapter = nullptr);
	std::wstring getSelText();
	std::wstring getText();
	void setText(const std::wstring & text);
	void addText(const std::wstring & text);
	void focus();
	void replaceSelText(std::wstring & text);
	void clearText();
	void setReadOnly(bool readOnly);
	bool getReadOnly();
protected:
	QSqlEdit editor;
	std::wstring content;
	QHelpEditAdapter * adapter = nullptr;

	virtual void createOrShowUI();
	void crateOrShowEditor(QSqlEdit &win, CRect &clientRect);
	
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHandleScnCharAdded(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHandleScnFocusIn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHandleScnDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHandleScnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHandleScnAutoCSelection(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void selectAllCurWord();

};
