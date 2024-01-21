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

 * @file   QParamElem.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#pragma once
#include <string>
#include <vector>
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <atlgdi.h>
#include "core/entity/Entity.h"
#include "common/Config.h"

class QParamElem: public CWindowImpl<QParamElem> {
public:
	BEGIN_MSG_MAP_EX(QParamElem)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)

		COMMAND_HANDLER_EX(Config::QPARAMELEM_EDIT_ELEM_ID, EN_CHANGE, OnValEditElemChange)
		COMMAND_HANDLER_EX(Config::QPARAMELEM_COMBO_EDIT_ID, CBN_SELCHANGE, OnValComboBoxChange)
		COMMAND_HANDLER_EX(Config::QPARAMELEM_COMBO_READ_ID, CBN_SELCHANGE, OnValComboBoxChange)

		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		MSG_WM_CTLCOLORLISTBOX(OnCtlListBoxColor)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	QParamElem(const ParamElemData & data);
	~QParamElem();
	
	
	const ParamElemData & getData() { return data; }
	void setData(const ParamElemData & data);

	const ParamElemData & getNewData() { return newData; }
	void setBkgColor(COLORREF val) { bkgColor = val; }
	
private:
	bool isNeedReload = true;
	ParamElemData data;
	ParamElemData newData;

	COLORREF bkgColor =  RGB(225, 225, 225);
	COLORREF readColor =  RGB(225, 225, 225);
	CBrush bkgBrush;
	CBrush readBrush;

	COLORREF textColor = RGB(64, 64, 64);
	HFONT textFont = nullptr;
	CPen textPen;
	HFONT comboFont = nullptr;
	
	CStatic label;
	CEdit valEdit;
	CComboBox valComboBox;
	CStatic desLabel;

	void createOrShowUI();
	void createOrShowElems(CRect & clientRect);
	void createOrShowComboBox(HWND hwnd, CComboBox &win, UINT id, CRect & rect, CRect &clientRect, bool allowEdit);

	void loadWindow();
	void loadValElem();

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlListBoxColor(HDC hdc, HWND hwnd);

	void OnValEditElemChange(UINT uNotifyCode, int nID, HWND hwnd);
	void OnValComboBoxChange(UINT uNotifyCode, int nID, HWND hwnd);
};
