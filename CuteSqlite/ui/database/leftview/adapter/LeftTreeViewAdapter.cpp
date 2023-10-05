#include "stdafx.h"
#include "LeftTreeViewAdapter.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

LeftTreeViewAdapter::LeftTreeViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
	this->dataView->SetImageList(imageList);
}


LeftTreeViewAdapter::~LeftTreeViewAdapter()
{
	if (databaseBitmap) ::DeleteObject(databaseBitmap);
	if (folderBitmap) ::DeleteObject(folderBitmap);
	if (tableBitmap) ::DeleteObject(tableBitmap);
	if (fieldBitmap) ::DeleteObject(fieldBitmap);
	if (indexBitmap) ::DeleteObject(indexBitmap);
	if (viewBitmap) ::DeleteObject(viewBitmap);
	if (triggerBitmap) ::DeleteObject(triggerBitmap);
}

void LeftTreeViewAdapter::createUserDatabase(std::wstring & dbPath)
{
	try {
		uint64_t userDbId = databaseService->hasUserDb(dbPath);
		if (userDbId && QMessageBox::confirm(parentHwnd, S(L"override-user-db"), S(L"yes"), S(L"no"))
			== Config::CUSTOMER_FORM_NO_BUTTON_ID) {
			return ;
		}
		// remove first , then create database
		if (userDbId) {
			databaseService->removeUserDb(userDbId);
		}

		databaseService->createUserDb(dbPath);
		QPopAnimate::success(parentHwnd, S(L"create-db-success-text"));
		loadTreeView();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

void LeftTreeViewAdapter::openUserDatabase(std::wstring & dbPath)
{
	try {
		uint64_t userDbId = databaseService->hasUserDb(dbPath);
		if (userDbId) {
			QPopAnimate::warn(parentHwnd, S(L"open-db-exists-inlist"));
			return ;
		}

		databaseService->openUserDb(dbPath);
		QPopAnimate::success(parentHwnd, S(L"open-db-success-text"));
		loadTreeView();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

/**
 * active the user db.
 * 
 * @param userDbId
 */
void LeftTreeViewAdapter::activeUserDatabase(uint64_t userDbId)
{
	try {
		databaseService->activeUserDb(userDbId);
		loadDbs(); // reload dbs
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

void LeftTreeViewAdapter::loadTreeView()
{
	dataView->DeleteAllItems();
	dbs.clear();
	try {
		dbs = databaseService->getAllUserDbs();
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
	int n = static_cast<int>(dbs.size());
	HTREEITEM hSelDbItem = nullptr;
	for (int i = 0; i < n; i++) {
		UserDb item = dbs.at(i);
		HTREEITEM hDbItem = dataView->InsertItem(item.name.c_str(), 0, 0, NULL, TVI_LAST);
		// set item data value : userDbId
		dataView->SetItemData(hDbItem, item.id);
		
		HTREEITEM hTablesFolderItem = dataView->InsertItem(S(L"tables").c_str(), 1, 1, hDbItem, TVI_LAST);
		HTREEITEM hViewsFolderItem = dataView->InsertItem(S(L"views").c_str(), 1, 1, hDbItem, TVI_LAST);
		HTREEITEM hTriggersFolderItem = dataView->InsertItem(S(L"triggers").c_str(), 1, 1, hDbItem, TVI_LAST);

		loadTablesForTreeView(hTablesFolderItem, item);
		loadViewsForTreeView(hViewsFolderItem, item);
		loadTriggersForTreeView(hTriggersFolderItem, item);

		if (item.isActive) {
			hSelDbItem = hDbItem;
			databaseSupplier->setSeletedUserDbId(item.id);
		}
		dataView->Expand(hTablesFolderItem);
	}

	// Expand the Selected DB item
	if (hSelDbItem) {
		dataView->Expand(hSelDbItem);
	}
	
}

CTreeItem LeftTreeViewAdapter::getSeletedItem()
{
	return dataView->GetSelectedItem();
}

uint64_t LeftTreeViewAdapter::getSeletedItemData()
{
	CTreeItem treeItem = dataView->GetSelectedItem();
	if (treeItem.IsNull()) {
		return 0;
	}
	DWORD_PTR data = dataView->GetItemData(HTREEITEM(treeItem));
	return static_cast<uint64_t>(data);
}

void LeftTreeViewAdapter::removeSeletedItem()
{
	CTreeItem treeItem = getSeletedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);
	if (!ret) {
		return ;
	}
	// if nImage == 0, seleted item is a item of user database
	if (nImage == 0 && QMessageBox::confirm(parentHwnd, S(L"delete-user-db")) == Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		uint64_t userDbId = getSeletedItemData();
		if (!userDbId) {
			return ;
		}
		
		try {
			// check user db is exists, then remove it.
			bool isHas = databaseService->hasUserDb(userDbId);
			if (!isHas) {
				return ;
			}
			databaseService->removeUserDb(userDbId); 
			QPopAnimate::success(parentHwnd, S(L"delete-db-success-text"));
			dataView->DeleteItem(HTREEITEM(treeItem));
			loadDbs(); // reload dblist
		}
		catch (QRuntimeException &ex) {
			Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
			QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
		}
	}
}

void LeftTreeViewAdapter::selectItem(uint64_t userDbId)
{
	if (userDbId <= 0) {
		return ;
	}

	CTreeItem treeItem = dataView->GetFirstVisibleItem();
	
	while (!treeItem.IsNull()) {
		uint64_t itemDataId = static_cast<uint64_t>(dataView->GetItemData(treeItem.m_hTreeItem));
		if (itemDataId == userDbId) {
			dataView->SelectItem(treeItem.m_hTreeItem);
			dataView->SetFocus();
			break;
		}
		treeItem = treeItem.GetNextSibling();
	}
}

/**
 * Get the selected item to find it's parent db item and return userDbId.
 * 
 * @return parent db item userDbId
 */
uint64_t LeftTreeViewAdapter::getSeletedUserDbId()
{
	CTreeItem seletedItem = dataView->GetSelectedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = false;
	CTreeItem parentItem = seletedItem;
	do  {
		ret = parentItem.GetImage(nImage, nSeletedImage);
		if (!ret) {
			continue;
		}

		// if nImage == 0, seleted item is a item of user database
		if (nImage == 0) {
			uint64_t userDbId = static_cast<uint64_t>(dataView->GetItemData(parentItem));
			databaseSupplier->setSeletedUserDbId(userDbId);
			return userDbId;
		}		
	} while (!(parentItem = parentItem.GetParent()).IsNull());
	return 0;
}

UserDbList LeftTreeViewAdapter::getDbs()
{
	return dbs;
}

void LeftTreeViewAdapter::loadDbs()
{
	try {
		dbs = databaseService->getAllUserDbs();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

void LeftTreeViewAdapter::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	databaseBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\database.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	folderBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\folder.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	tableBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\table.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	fieldBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\field.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	indexBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\index.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	viewBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\view.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	triggerBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\trigger.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 4);
	imageList.Add(databaseBitmap); // 0 - database
	imageList.Add(folderBitmap); // 1 - folder
	imageList.Add(tableBitmap); // 2 - table
	imageList.Add(fieldBitmap); // 3 - field
	imageList.Add(indexBitmap); // 4 - index
	imageList.Add(viewBitmap);// 5 - view
	imageList.Add(triggerBitmap);// 6 - trigger
}

/**
 * Load tables for TreeView.
 * 
 * @param hTablesFolderItem The tables folder item
 * @param userDb UserDb refrence
 */
void LeftTreeViewAdapter::loadTablesForTreeView(HTREEITEM hTablesFolderItem, UserDb & userDb)
{
	try {
		UserTableList tableList = databaseService->getUserTables(userDb.id);
		for (UserTable item : tableList) {
			HTREEITEM hTblItem = dataView->InsertItem(item.name.c_str(), 2, 2, hTablesFolderItem, TVI_LAST);

			HTREEITEM hFieldsFolderItem = dataView->InsertItem(S(L"fields").c_str(), 1, 1, hTblItem, TVI_LAST);
			HTREEITEM hIndexesFolderItem = dataView->InsertItem(S(L"indexes").c_str(), 1, 1, hTblItem, TVI_LAST);

			loadColumsForTreeView(hFieldsFolderItem, userDb.id, item);
			loadIndexesForTreeView(hIndexesFolderItem, userDb.id, item);
		}		
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
	
}

/**
* Load views for TreeView.
*
* @param hViewsFolderItem The views folder item
* @param userDb UserDb refrence
*/
void LeftTreeViewAdapter::loadViewsForTreeView(HTREEITEM hViewsFolderItem, UserDb & userDb)
{
	try {
		UserTableList list = databaseService->getUserViews(userDb.id);
		for (UserTable item : list) {
			dataView->InsertItem(item.name.c_str(), 5, 5, hViewsFolderItem, TVI_LAST);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

/**
* Load triggers for TreeView.
*
* @param hViewsFolderItem The triggers folder item
* @param userDb UserDb refrence
*/
void LeftTreeViewAdapter::loadTriggersForTreeView(HTREEITEM hTriggersFolderItem, UserDb & userDb)
{
	try {
		UserTableList list = databaseService->getUserTriggers(userDb.id);
		for (UserTable item : list) {
			dataView->InsertItem(item.name.c_str(), 6, 6, hTriggersFolderItem, TVI_LAST);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

void LeftTreeViewAdapter::loadColumsForTreeView(HTREEITEM hFieldsFolderItem, uint64_t userDbId, UserTable & userTable)
{
	try {
		ColumnInfoList list = databaseService->getUserColumns(userDbId, userTable.name);
		for (ColumnInfo item : list) {
			std::wstring field = item.name;
			field.append(L" [").append(item.type).append(L", ").append(item.notnull ? L"NOT NULL" : L"NULL").append(L"]");
			CTreeItem treeItem = dataView->InsertItem(field.c_str(), 3, 3, hFieldsFolderItem, TVI_LAST);
			treeItem.SetData((DWORD_PTR)item.cid);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

void LeftTreeViewAdapter::loadIndexesForTreeView(HTREEITEM hIndexesFolderItem, uint64_t userDbId, UserTable & userTable)
{
	try {
		UserIndexList list = databaseService->getUserIndexes(userDbId, userTable.name);
		for (UserTable item : list) {
			dataView->InsertItem(item.name.c_str(), 4, 4, hIndexesFolderItem, TVI_LAST);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}
