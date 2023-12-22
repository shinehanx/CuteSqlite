#include "stdafx.h"
#include "HomePanel.h"
#include "utils/GdiUtil.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "common/AppContext.h"

void HomePanel::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowDbButtons(clientRect);
	createOrShowUserDbListItems(clientRect);
}

/**
 * createOrShowDbButtons
 * Notice: must declare REFLECT_NOTIFICATIONS, otherwise the QImageButton buttons is not calling DrawItem.
 * 
 * @param clientRect
 */
void HomePanel::createOrShowDbButtons(CRect & clientRect)
{
	std::wstring sectionText = S(L"db-section-text");
	getDbSectionRect(clientRect, sectionText);
	int x = dbSectionRect.right + 2,
		y = dbSectionRect.top + 5,
		w = 16, h = 16;
	CRect rect(x, y, x + w, y + h);
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!createDababaseButton.IsWindow()) {
		normalImagePath = imgDir + L"home\\button\\add-database-button-normal.png";
		pressedImagePath = imgDir + L"home\\button\\add-database-button-pressed.png";
		createDababaseButton.SetIconPath(normalImagePath, pressedImagePath);
		createDababaseButton.SetBkgColors(bkgColor, bkgColor, bkgColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, createDababaseButton, Config::HOME_CREATE_DB_BUTTON_ID, L"", rect, clientRect, BS_OWNERDRAW);
	createDababaseButton.SetToolTip(S(L"create-database")); 
	createDababaseButton.BringWindowToTop();

	rect.OffsetRect(w + 5, 0);
	if (!modDatabaseButton.IsWindow()) {
		normalImagePath = imgDir + L"home\\button\\mod-database-button-normal.png";
		pressedImagePath = imgDir + L"home\\button\\mod-database-button-pressed.png";
		modDatabaseButton.SetIconPath(normalImagePath, pressedImagePath);
		modDatabaseButton.SetBkgColors(bkgColor, bkgColor, bkgColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, modDatabaseButton, Config::HOME_MOD_DB_BUTTON_ID, L"", rect, clientRect, BS_OWNERDRAW);
	modDatabaseButton.SetToolTip(S(L"database"));
}

void HomePanel::createOrShowUserDbListItems(CRect & clientRect)
{
	if (clientRect.Width() < DATABASE_LIST_ITEM_WIDTH) {
		return;
	}
	CRect lastRect = GdiPlusUtil::GetWindowRelativeRect(createDababaseButton.m_hWnd);
	int x = 20, y = lastRect.bottom + 20, 
		w = DATABASE_LIST_ITEM_WIDTH, h = DATABASE_LIST_ITEM_HEIGHT;
	int dbLen = static_cast<int>(dbListItemPtrs.size());
	CRect rect(x, y, x + w, y + h);
	for (int i = 0; i < dbLen; i++) {
		DatabaseListItem * item = dbListItemPtrs.at(i);
		if (i > 0) {
			if (rect.right + w > clientRect.right) {
				rect.left = x;
				rect.top = rect.top + h + 10;
				rect.right = rect.left + w;
				rect.bottom = rect.top + h;
			} else {
				rect.OffsetRect(w + 10, 0);
			}			
		}
		createOrShowListItem(*item, Config::DB_LIST_ITEM_ID_START + i, rect, clientRect);
	}
}

void HomePanel::loadUserDbList()
{
	CRect clientRect;
	GetClientRect(clientRect);
	clearDbListItemPtrs();
	userDbList = databaseService->getAllUserDbs();
	int dbLen = static_cast<int>(userDbList.size());

	int maxWidth = clientRect.Width() - 40;
	int nMax = maxWidth / DATABASE_LIST_ITEM_WIDTH; // max item size

	CRect lastRect = GdiPlusUtil::GetWindowRelativeRect(createDababaseButton.m_hWnd);
	int x = 20, y = lastRect.bottom + 20, 
		w = DATABASE_LIST_ITEM_WIDTH, h = DATABASE_LIST_ITEM_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	for (int i = 0; i < dbLen; i++) {
		auto & userDb = userDbList.at(i);
		DatabaseListItem * item = new DatabaseListItem(userDb);
		if (i > 0) {
			if (rect.right + w > clientRect.right) {
				rect.left = x;
				rect.top = rect.top + h + 10;
				rect.right = rect.left + w;
				rect.bottom = rect.top + h;
			} else {
				rect.OffsetRect(w + 10, 0);
			}			
		}
		createOrShowListItem(*item, Config::DB_LIST_ITEM_ID_START + i, rect, clientRect);
	}

}

void HomePanel::createOrShowListItem(DatabaseListItem & win, UINT id, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | WS_VISIBLE; // SS_NOTIFY - 表示static接受点击事件
		win.Create(m_hWnd, rect, NULL, 0, 0, id);
		dbListItemPtrs.push_back(&win);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void HomePanel::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;

	loadBkgImage();
	loadUserDbList();
}

void HomePanel::loadBkgImage()
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring path = settingService->getSysInit(L"home-bkg-image");
	std::wstring newPath = binDir + path;
	if (!path.empty() && newPath != bkgImagePath) {
		bkgImagePath = binDir + path;
		Gdiplus::Bitmap bitmap(bkgImagePath.c_str(), false);
		Gdiplus::Color color(Gdiplus::Color::Transparent);
		HBITMAP hBitmap;
		bitmap.GetHBITMAP(color, &hBitmap);
		bkgBitmap.Attach(hBitmap);
		::DeleteObject(&bitmap);
	}
	else {
		bkgImagePath = L"";
	}
}


void HomePanel::clearDbListItemPtrs()
{
	if (dbListItemPtrs.empty()) {
		return;
	}
	int n = static_cast<int>(dbListItemPtrs.size());
	for (int i = n - 1; i >= 0; i--) {
		auto ptr = dbListItemPtrs.at(i);
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			delete ptr;
			ptr = nullptr;
		}else if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	dbListItemPtrs.clear();
}

LRESULT HomePanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_HOME_REFRESH_DATABASE_ID);
	bkgBrush.CreateSolidBrush(bkgColor);
	titleFont = FT(L"welcome-text-size");
	homeFont = FT(L"home-text-size");
	sectionFont = FTB(L"home-section-size", true);
	return 0;
}

LRESULT HomePanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_HOME_REFRESH_DATABASE_ID);
	if (bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (titleFont) ::DeleteObject(titleFont);
	if (homeFont) ::DeleteObject(homeFont);
	if (sectionFont) ::DeleteObject(sectionFont);
	
	if (!bkgBitmap.IsNull()) bkgBitmap.DeleteObject();
	if (createDababaseButton.IsWindow()) createDababaseButton.DestroyWindow();
	if (modDatabaseButton.IsWindow()) modDatabaseButton.DestroyWindow();

	clearDbListItemPtrs();
	return 0;
}

LRESULT HomePanel::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	createOrShowUI();
	return 0;
}

LRESULT HomePanel::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!wParam) {
		return 0;
	}
	
	loadWindow();
	return 0;
}

LRESULT HomePanel::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);	
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);

	// Draw welcome and home text
	drawHomeText(mdc, clientRect);
	drawDbSection(mdc, clientRect);
	if (!bkgBitmap.IsNull()) {
		GdiUtil::paintCenterFixStretchBlt(mdc, bkgBitmap);
	}
	
	return 0;
}

void HomePanel::drawHomeText(CMemoryDC &mdc, CRect &clientRect)
{
	int x = clientRect.left, y = 70, w = clientRect.Width(), h = 70;
	CRect rect(x, y, x + w, y + h);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(titleFont);
	COLORREF oldColor = mdc.SetTextColor(titleColor);
	std::wstring welcome = S(L"welcome-text");
	mdc.DrawText(welcome.c_str(), static_cast<int>(welcome.size()), rect, DT_CENTER | DT_VCENTER);
	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);

	rect.OffsetRect(0, h + 50);
	rect.bottom = rect.top + 40;
	oldFont = mdc.SelectFont(homeFont);
	oldColor = mdc.SetTextColor(homeColor);
	std::wstring homeText = S(L"home-text1");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text2");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text3");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text4");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}

void HomePanel::drawDbSection(CMemoryDC &mdc, CRect &clientRect)
{
	HFONT oldFont = mdc.SelectFont(sectionFont);
	COLORREF oldColor = mdc.SetTextColor(titleColor);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	std::wstring sectionText = S(L"db-section-text");
	getDbSectionRect(clientRect, sectionText);

	mdc.DrawText(sectionText.c_str(), static_cast<int>(sectionText.size()), dbSectionRect, DT_LEFT | DT_VCENTER);

	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}

void HomePanel::getDbSectionRect(CRect &clientRect, std::wstring &sectionText)
{
	int x = 20, y = 190 + 4 * 40 + 20, w = clientRect.Width() - 40, h = 40;
	CSize size = StringUtil::getTextSize(sectionText.c_str(), sectionFont);
	w = size.cx + 10;
	dbSectionRect = { x, y, x + w, y + h };
}

BOOL HomePanel::OnEraseBkgnd(CDCHandle dc)
{
	if (!bkgBitmap.IsNull()) {
		bkgBitmap.DeleteObject();
	}

	return TRUE;
}

LRESULT HomePanel::OnClickCreateDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_CREATE_DATABASE_ID);
	return 0;
}

LRESULT HomePanel::OnClickModDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_ACTIVE_PANEL_ID, Config::DATABASE_PANEL, NULL);
	return 0;
}

LRESULT HomePanel::OnClickDbListItem(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nID = (UINT)wParam;
	uint64_t selectedUserDbId = static_cast<uint64_t>(lParam);
	int nSelItem = nID - Config::DB_LIST_ITEM_ID_START;
	if (nSelItem >= dbListItemPtrs.size() || nSelItem < 0 || !selectedUserDbId) {
		return 0;
	}
	
	databaseService->activeUserDb(selectedUserDbId);
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_REFRESH_DATABASE_ID, NULL, NULL);
	AppContext::getInstance()->dispatch(Config::MSG_ACTIVE_PANEL_ID, Config::DATABASE_PANEL, NULL);
	return 0;
}

LRESULT HomePanel::OnRefreshDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	loadUserDbList();
	return 0;
}
