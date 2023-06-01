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

 * @file   SettingService.h
 * @brief  the settings service class
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <unordered_map>
#include "core/entity/Entity.h"
#include "core/common/service/BaseService.h"
#include "core/repository/system/SysInitRepository.h"

class SettingService: public BaseService<SettingService, SysInitRepository>
{
public:
	SettingService();
	~SettingService();
	//--------------���ݿ�---------------------------------------------------
	std::wstring getSysInit(const std::wstring & key);
	void setSysInit(const std::wstring & key, const std::wstring & val);
	// ��ȡ���ݿ����е�������
	Setting getAllSysSetting(bool isReload = false);
	
	//--------------ini---------------------------------------------------
	// ��ȡini���з����Լ�������<group, Setting>
	IniSetting getAllIniSetting(bool isReload = false);
	// ���ini�����ļ���ֵ key=val
	std::wstring getGenderIniVal(const std::wstring & key);
	// ������ҳ����ͼƬ,����<first-����ͼ·��,twice-ͼƬ·��>
	std::pair<std::wstring, std::wstring> updateHomeBkgImage(std::wstring & avatar);
	// ��ȡini������
	Setting getAllGenderSetting();
	Setting getSettingBySection(const std::wstring & section);
	std::wstring getValBySectionAndKey(const std::wstring & section, const std::wstring & key);
	void saveLanguagePath(std::wstring & langIniPath);
private:
	
	//--------------���ݿ�---------------------------------------------------
	// ����sys_init���е�����
	Setting sysSetting;

	//--------------ini---------------------------------------------------
	// ini���з����Լ�������<group, Setting>
	IniSetting iniSetting;
};
