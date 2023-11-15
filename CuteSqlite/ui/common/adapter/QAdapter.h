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

 * @file   QAdapter.h
 * @brief  The template base class for all adapter classes
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once

#include <atlctrls.h>

/**
 * template T - subclass ,V - dataView class such as listview/treeview
 */
template <typename T, typename V = ATL::CWindow>
class QAdapter
{
public:
	static T * getInstance(HWND parentHwnd, V * dataView = nullptr); // singleton
	void initMenuInfo(HMENU hMenu);
protected:	
	static T * theInstance;
	HWND parentHwnd;
	CBrush menuBrush ;
	V * dataView;
};

template <typename T, typename V /*= ATL::CWindow*/>
void QAdapter<T, V>::initMenuInfo(HMENU hMenu)
{
	MENUINFO mi;
	menuBrush.CreateSolidBrush(RGB(255,255,255));//RGB(255,128,128));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE; 
	mi.hbrBack = (HBRUSH)menuBrush;
	mi.dwStyle = MNS_CHECKORBMP;
	::SetMenuInfo(hMenu,&mi);
	menuBrush.DeleteObject();
}

template <typename T, typename V>
T * QAdapter<T, V>::theInstance = nullptr; // singleton


// singleton
template <typename T, typename V>
T * QAdapter<T, V>::getInstance(HWND parentHwnd, V * dataView /*= nullptr*/)
{
	if (QAdapter::theInstance == nullptr) {
		QAdapter::theInstance = new T(parentHwnd, dataView);
	}
	QAdapter::theInstance->parentHwnd = parentHwnd;
	QAdapter::theInstance->dataView = dataView;
	return QAdapter::theInstance;
}