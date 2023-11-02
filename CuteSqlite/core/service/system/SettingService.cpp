/*****************************************************************//**
 * @file   SettingService.cpp
 * @brief  系统服务
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-02
 *********************************************************************/
#include "stdafx.h"
#include <atlfile.h>
#include "SettingService.h"
#include "core/entity/Entity.h"
#include "core/common/exception/QRuntimeException.h"
#include "utils/Log.h"
#include "utils/ProfileUtil.h"
#include "utils/GdiPlusUtil.h"
#include "utils/DateUtil.h"

SettingService::SettingService()
{

}


SettingService::~SettingService()
{

}

/**
 * 系统配置表sys_init的数据，KEY匹配到.
 * 
 * @param key
 * @return 
 */
std::wstring SettingService::getSysInit(const std::wstring & key)
{
	auto settings = getAllSysSetting();
	for (auto pair : settings) {
		if (pair.first == key) {
			return pair.second;
		}
	}
	return L"";
}

/**
 * key,val保存到数据表sys_init中.
 * 
 * @param key
 * @param val
 */
void SettingService::setSysInit(const std::wstring & key, const std::wstring & val)
{
	getRepository()->set(key, val);
	getAllSysSetting(true);//重新加载数据库sys_init表所有的选项
}


/**
 * 加载数据库sys_init表所有的选项
 * 
 * @param isReload 是否重新加载
 * @return 
 */
Setting SettingService::getAllSysSetting(bool isReload)
{
	if (!isReload && !sysSetting.empty()) {
		return sysSetting;
	}
	sysSetting.clear();
	SysInitList allList = getRepository()->getAll();
	for (auto item : allList) {
		sysSetting.push_back({item.name, item.val});
	}

	return sysSetting;
}

/**
 * 获取ini所有配置项<group, Setting>.
 * 
 * @param isReload 是否重新加载
 * @return 所有分组配置项<group, Setting>
 */
IniSetting SettingService::getAllIniSetting(bool isReload)
{
	if (!isReload && iniSetting.empty() == false) {
		return iniSetting;
	}

	iniSetting.clear();
	std::vector<std::wstring> setions;
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring iniFile = getSysInit(L"lang-file");
	std::wstring iniPath = binDir + iniFile;

	if (_waccess(iniPath.c_str(), 0) != 0) {
		Q_ERROR(L"the ini setting file is not exists, path:{}", iniPath);
		throw QRuntimeException(L"000002", L"sorry, the setting file is not exists!");
	}

	std::vector<std::wstring> sections;	
	ProfileUtil::parseSections(iniPath, sections);

	for (auto item : sections) {
		Setting setting;
		ProfileUtil::parseSectionSettings(iniPath, item, setting);
		if (!setting.empty()) {
			iniSetting[item] = setting;
		}
		
	}
	return iniSetting;

}

/**
 * 从[language].ini配置文件中读取所有性别.
 * 
 * @return 性别所有项
 */
Setting SettingService::getAllGenderSetting()
{
	Setting genders;
	IniSetting settings = getAllIniSetting();
	if (settings.empty()) {
		return genders;
	}
	genders = settings[L"GENDER"];
	return genders;
}

Setting SettingService::getSettingBySection(const std::wstring & section)
{
	Setting sectionSetting;
	IniSetting settings = getAllIniSetting();
	if (settings.empty()) {
		return sectionSetting;
	}
	sectionSetting = settings[section];
	return sectionSetting;
}

/**
 * 找节点指定的key值.
 * 
 * @param section
 * @param key
 * @return 
 */
std::wstring SettingService::getValBySectionAndKey(const std::wstring & section, const std::wstring & key)
{
	Setting setting = getSettingBySection(section);
	auto iterator = std::find_if(setting.begin(), setting.end(), [&key](std::pair<std::wstring, std::wstring> &pair) {
		return pair.first == key;
	});

	if (iterator == setting.end()) {
		return L"";
	}
	return (*iterator).second;
}

/**
 * 保存语言文件.
 * 
 * @param langIniPath 语言文件路径(相对路径)
 */
void SettingService::saveLanguagePath(std::wstring & langIniPath)
{
	setSysInit(std::wstring(L"lang-file"), langIniPath);
}

std::wstring SettingService::getGenderIniVal(const std::wstring & key)
{
	Setting settings = getSettingBySection(L"GENDER");
	for (auto item : settings) {
		if (item.first == key) {
			return item.second;
		}
	}
	return L"";
}


/**
 * 更新主页背景图片,返回缩略图路径和图片路径>.
 * 
 * @param [in]imagePath 图片路径
 * @return pair<first-图片路径,twice-缩略图路径>.
 */
std::pair<std::wstring, std::wstring> SettingService::updateHomeBkgImage(std::wstring & imagePath)
{
	std::pair<std::wstring, std::wstring> pair;
	//文件不存在
	if (_waccess(imagePath.c_str(), 0) != 0) {
		return pair;
	}
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring relativePath = L"upload\\home\\" + DateUtil::getCurrentDateTime(L"%Y-%m-%d") + L"\\";
	std::wstring newPath = binDir + relativePath;

	if (_waccess(newPath.c_str(), 0) != 0) {
		FileUtil::createDirectory(newPath);
	}
	
	std::wstring fileExt = FileUtil::getFileExt(imagePath);
	std::wstring datetime = DateUtil::getCurrentDateTime(L"-%Y%m%d-%H%M%S");
	// 新图名称
	std::wstring newFileName; 	
	
	newFileName.append(L"home-bkg-image").append(datetime).append(L".").append(fileExt);
	// 上传图片路径
	std::wstring uploadPath = newPath;
	uploadPath.append(newFileName);

	// 缩略图名称
	std::wstring previewFileName; 
	previewFileName.append(L"home-bkg-image").append(datetime).append(L".preview.").append(fileExt);
	// 上传照片缩略图路径
	std::wstring uploadPreviewPath = newPath;
	uploadPreviewPath.append(previewFileName);

	if (!FileUtil::copy(imagePath, uploadPath)) {
		Q_ERROR(L"copy failed, src path:{}, new path:{}", imagePath, newPath);
		return pair;
	}
	
	if (!GdiPlusUtil::thumbImage(uploadPath, uploadPreviewPath, 160 ,90)) {
		Q_ERROR(L"thumbImage failed, src path:{}, dst path:{}", newPath, uploadPreviewPath);
		return pair;
	}

	pair.first = uploadPath;
	pair.second = uploadPreviewPath;

	// 保存数据库到sys_init表，字段name=home-bkg-image, val=${relativePath}
	relativePath.append(newFileName);
	setSysInit(L"home-bkg-image", relativePath);
	return pair;
}
