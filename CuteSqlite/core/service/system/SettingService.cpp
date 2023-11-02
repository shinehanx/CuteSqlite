/*****************************************************************//**
 * @file   SettingService.cpp
 * @brief  ϵͳ����
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
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
 * ϵͳ���ñ�sys_init�����ݣ�KEYƥ�䵽.
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
 * key,val���浽���ݱ�sys_init��.
 * 
 * @param key
 * @param val
 */
void SettingService::setSysInit(const std::wstring & key, const std::wstring & val)
{
	getRepository()->set(key, val);
	getAllSysSetting(true);//���¼������ݿ�sys_init�����е�ѡ��
}


/**
 * �������ݿ�sys_init�����е�ѡ��
 * 
 * @param isReload �Ƿ����¼���
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
 * ��ȡini����������<group, Setting>.
 * 
 * @param isReload �Ƿ����¼���
 * @return ���з���������<group, Setting>
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
 * ��[language].ini�����ļ��ж�ȡ�����Ա�.
 * 
 * @return �Ա�������
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
 * �ҽڵ�ָ����keyֵ.
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
 * ���������ļ�.
 * 
 * @param langIniPath �����ļ�·��(���·��)
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
 * ������ҳ����ͼƬ,��������ͼ·����ͼƬ·��>.
 * 
 * @param [in]imagePath ͼƬ·��
 * @return pair<first-ͼƬ·��,twice-����ͼ·��>.
 */
std::pair<std::wstring, std::wstring> SettingService::updateHomeBkgImage(std::wstring & imagePath)
{
	std::pair<std::wstring, std::wstring> pair;
	//�ļ�������
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
	// ��ͼ����
	std::wstring newFileName; 	
	
	newFileName.append(L"home-bkg-image").append(datetime).append(L".").append(fileExt);
	// �ϴ�ͼƬ·��
	std::wstring uploadPath = newPath;
	uploadPath.append(newFileName);

	// ����ͼ����
	std::wstring previewFileName; 
	previewFileName.append(L"home-bkg-image").append(datetime).append(L".preview.").append(fileExt);
	// �ϴ���Ƭ����ͼ·��
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

	// �������ݿ⵽sys_init���ֶ�name=home-bkg-image, val=${relativePath}
	relativePath.append(newFileName);
	setSysInit(L"home-bkg-image", relativePath);
	return pair;
}
