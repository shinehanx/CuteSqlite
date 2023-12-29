#include "stdafx.h"
#include "GeneralSettingsView.h"
#include "common/AppContext.h"
#include "utils/ResourceUtil.h"
#include "utils/ProfileUtil.h"
#include "utils/DateUtil.h"
#include "utils/Log.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/message/QPopAnimate.h"

void GeneralSettingsView::setup()
{

}

void GeneralSettingsView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowComboboxes(clientRect);
}


void GeneralSettingsView::createOrShowComboboxes(CRect & clientRect)
{
	int x = clientRect.right - 40 - 150, y = 85, w = 150, h = 35;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowComboBox(m_hWnd, languageComboBox, Config::SETTING_LANGUAGE_COMBOBOX_ID, rect, clientRect);
}

void GeneralSettingsView::loadWindow()
{
	loadLanguageComboBox();
}
/**
 * 加载语言.
 * 
 */
void GeneralSettingsView::loadLanguageComboBox()
{
	
	std::wstring findDir = ResourceUtil::getProductBinDir();
	findDir.append(L"res\\language\\");
	ProfileUtil::parseLanguagesFromDir(findDir, languages);
	if (languages.empty()) {
		return ;
	}
	std::wstring selLanguage = settingSerivce->getValBySectionAndKey(L"LANG", L"language");
	languageComboBox.ResetContent();
	int nSelItem = -1;
	int n = static_cast<int>(languages.size());
	for (int i = 0; i < n; i++) {
		auto language = languages.at(i);
		languageComboBox.AddString(language.first.c_str());
		if (language.first == selLanguage) {
			nSelItem = i;
		}		
	}

	if (nSelItem == -1) {
		return;
	}
	languageComboBox.SetCurSel(nSelItem);
}

/**
 * change language
 * 
 * @param uNotifyCode
 * @param nID
 * @param hwnd
 * @return 
 */
LRESULT GeneralSettingsView::OnChangeLanguageComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = languageComboBox.GetCurSel();
	ATLASSERT(nSelItem < languages.size());
	auto pair = languages.at(nSelItem);
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring iniFilePath = pair.second;
	std::wstring savePath = StringUtil::replace(iniFilePath, binDir, std::wstring(L""));
	try {
		
		//AppContext::getInstance()->dispatch(Config::MSG_CHANGE_LANGUAGE_ID);
		if (QMessageBox::confirm(m_hWnd, S(L"change-language-restart-text"), S(L"close-and-restart"), S(L"do-not-restart")) 
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			settingSerivce->saveLanguagePath(savePath);
			settingSerivce->getAllIniSetting(true); // 重新加载
			::SendMessageW(AppContext::getInstance()->getMainFrmHwnd(), WM_CLOSE, NULL, NULL);
			return 0;
		}else {
			settingSerivce->saveLanguagePath(savePath);
			settingSerivce->getAllIniSetting(true); // 重新加载
		}
		
	} catch (QRuntimeException &e) {
		Q_ERROR(L"change language has error, code:{}, msg:{}", e.getCode(), e.getMsg());
		// 处理错误
	}
	
	return 0;
}

/**
 * 子类实现这个方法，扩展画面内容.
 * 
 * @param dc 上下文DC
 * @param paintRect 窗口的范围
 */
void GeneralSettingsView::paintItem(CDC &dc, CRect & paintRect) 
{
	// 1.language section
	paintSection(dc, CPoint(40, 45), paintRect, S(L"language-setting").c_str(), nullptr);
}


LRESULT GeneralSettingsView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QSettingView::OnCreate(uMsg, wParam, lParam, bHandled);	
	
	return 0;
}

LRESULT GeneralSettingsView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QSettingView::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (languageComboBox.IsWindow()) languageComboBox.DestroyWindow();
	
	return 0;
}

