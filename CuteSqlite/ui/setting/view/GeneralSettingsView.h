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

 * @file   GeneralSettingsView.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include <atlcrack.h>
#include <atlctrls.h>
#include "ui/common/view/QSettingView.h"
#include "ui/common/button/QImageButton.h"
#include "core/service/system/SettingService.h"

class GeneralSettingsView : public QSettingView 
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(GeneralSettingsView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER_EX(Config::SETTING_LANGUAGE_COMBOBOX_ID, CBN_SELENDOK, OnChangeLanguageComboBox)
		CHAIN_MSG_MAP(QSettingView)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	// 三大金刚，都要声明
	virtual void setup();
	virtual void loadWindow();
protected:
	CComboBox languageComboBox;

	SettingService * settingSerivce = SettingService::getInstance();
	std::vector<std::pair<std::wstring,std::wstring>> languages; // 参数[<language, iniFile>]的数组,与langugageComboBox保持一致的下标
	

	virtual void createOrShowUI();
	void createOrShowComboboxes(CRect & clientRect);

	void loadLanguageComboBox();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnChangeLanguageComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	
};
