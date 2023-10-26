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

		uint64_t UserDbId = databaseService->createUserDb(dbPath);
		
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
		// open and select that
		databaseSupplier->selectedUserDbId = databaseService->openUserDb(dbPath);
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

		if (item.isActive) {
			hSelDbItem = hDbItem;
			databaseSupplier->setSeletedUserDbId(item.id);

			loadTablesForTreeView(hTablesFolderItem, item);
			loadViewsForTreeView(hViewsFolderItem, item);
			loadTriggersForTreeView(hTriggersFolderItem, item);
		}
		dataView->Expand(hTablesFolderItem);
	}

	// Expand the Selected DB item
	if (hSelDbItem) {
		dataView->Expand(hSelDbItem);
		dataView->SelectItem(hSelDbItem);
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
	if ((nImage == 0 || nImage == 1) && QMessageBox::confirm(parentHwnd, S(L"delete-user-db")) == Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		uint64_t userDbId = getSeletedItemData();
		userDbId = userDbId ? userDbId : databaseSupplier->getSelectedUserDbId();
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
	int nImage = -1, nSelImage = -1;
	bool ret = false;
	CTreeItem parentItem = seletedItem;
	do  {
		ret = parentItem.GetImage(nImage, nSelImage);
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

/**
 * Get the children folder item
 * 
 * @param hTreeItem - tree item
 * @param folderName - find child folder name: must be tables,  views,  triggers, feilds, indexes
 * @return 
 */
HTREEITEM LeftTreeViewAdapter::getChildFolderItem(HTREEITEM hTreeItem, const std::wstring & folderName)
{
	ATLASSERT(hTreeItem && !folderName.empty());
	if (!dataView->ItemHasChildren(hTreeItem)) {
		return nullptr;
	}

	HTREEITEM hChildItem = dataView->GetChildItem(hTreeItem);
	while (hChildItem) {
		int iImage = -1, iSelImage = -1;
		wchar_t * cch = NULL;
		bool ret = dataView->GetItemText(hChildItem, cch);
		if (!ret) {
			::SysFreeString(cch);
			continue;
		}
		std::wstring itemIext;
		itemIext.assign(cch);
		::SysFreeString(cch);
		if (folderName == itemIext) {
			return hChildItem;
		}
		
		hChildItem = dataView->GetNextItem(hChildItem, TVGN_NEXT); // 下一个同级项
	}
	return nullptr;
}


void LeftTreeViewAdapter::copyUserDatabase(const std::wstring & toDbPath)
{
	CTreeItem treeItem = getSeletedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);
	if (!ret) {
		return ;
	}
	// if nImage == 0, seleted item is a item of user database
	if (nImage == 0 || nImage == 1) { // 0 - database , 1 - table
		uint64_t userDbId = getSeletedItemData();
		userDbId = userDbId ? userDbId : databaseSupplier->getSelectedUserDbId();
		if (!userDbId) {
			return ;
		}
		
		try {
			// check user db is exists, then remove it.
			bool isHas = databaseService->hasUserDb(userDbId);
			if (!isHas) {
				return ;
			}
			databaseService->copyUserDb(userDbId, toDbPath);
			QPopAnimate::success(parentHwnd, S(L"copy-db-success-text"));
			
			loadTreeView(); // reload db list
		} catch (QRuntimeException &ex) {
			Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
			QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
		}
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
 * @param userDb UserDb reference
 */
void LeftTreeViewAdapter::loadTablesForTreeView(HTREEITEM hTablesFolderItem, UserDb & userDb, bool isLoadColumnsAndIndex)
{
	try {
		UserTableList tableList = databaseService->getUserTables(userDb.id);
		for (UserTable item : tableList) {
			HTREEITEM hTblItem = dataView->InsertItem(item.name.c_str(), 2, 2, hTablesFolderItem, TVI_LAST); 

			HTREEITEM hColumnsFolderItem = dataView->InsertItem(S(L"columns").c_str(), 1, 1, hTblItem, TVI_LAST);
			HTREEITEM hIndexesFolderItem = dataView->InsertItem(S(L"indexes").c_str(), 1, 1, hTblItem, TVI_LAST);

			if (isLoadColumnsAndIndex) {
				loadColumsForTreeView(hColumnsFolderItem, userDb.id, item);
				loadIndexesForTreeView(hIndexesFolderItem, userDb.id, item);
			}			
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
* @param userDb UserDb reference
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
* @param userDb UserDb reference
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

void LeftTreeViewAdapter::loadColumsForTreeView(HTREEITEM hColumnsFolderItem, uint64_t userDbId, UserTable & userTable)
{
	try {
		ColumnInfoList list = databaseService->getUserColumns(userDbId, userTable.name);
		for (ColumnInfo item : list) {
			std::wstring field = item.name;
			field.append(L" [").append(item.type).append(L", ").append(item.notnull ? L"NOT NULL" : L"NULL").append(L"]");
			CTreeItem treeItem = dataView->InsertItem(field.c_str(), 3, 3, hColumnsFolderItem, TVI_LAST);
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

/**
 * Call this function when tree item expanding 
 * 
 * @param ptr - NMTREEVIEW pointer for treeItem
 */
void LeftTreeViewAdapter::expandTreeItem(LPNMTREEVIEW ptr)
{
	HTREEITEM hSelTreeItem = ptr->itemNew.hItem;
	CTreeItem treeItem(hSelTreeItem, dataView);

	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);
	if (nImage == 0) { // 0 - database
		uint64_t userDbId = static_cast<uint64_t>(treeItem.GetData());
		UserDb userDb;
		userDb.id = userDbId;
		HTREEITEM hTablesFolderItem = getChildFolderItem(hSelTreeItem, S(L"tables"));
		HTREEITEM hViewsFolderItem = getChildFolderItem(hSelTreeItem, S(L"views"));
		HTREEITEM hTriggersFolderItem = getChildFolderItem(hSelTreeItem, S(L"triggers"));

		// if folder item has children, then it is loaded before
		if ((hTablesFolderItem && dataView->ItemHasChildren(hTablesFolderItem))
			|| (hViewsFolderItem && dataView->ItemHasChildren(hViewsFolderItem))
			|| (hTriggersFolderItem && dataView->ItemHasChildren(hTriggersFolderItem))) {
			return ;
		}

		// reload
		loadTablesForTreeView(hTablesFolderItem, userDb);
		loadViewsForTreeView(hViewsFolderItem, userDb);
		loadTriggersForTreeView(hTriggersFolderItem, userDb);

		dataView->Expand(hTablesFolderItem);
	} else if (nImage == 2) { // 2- tables
		UserTable userTable;
		wchar_t * cch = nullptr;
		treeItem.GetText(cch);
		if (!cch) {
			return;
		}
		userTable.name.assign(cch);
		::SysFreeString(cch);

		HTREEITEM hColumnsFolderItem = getChildFolderItem(hSelTreeItem, S(L"columns"));
		HTREEITEM hIndexesFolderItem = getChildFolderItem(hSelTreeItem, S(L"indexes"));
		// if folder item has children, then it is loaded before
		if ((hColumnsFolderItem && dataView->ItemHasChildren(hColumnsFolderItem))
			|| (hIndexesFolderItem && dataView->ItemHasChildren(hIndexesFolderItem))) {
			return ;
		}

		// Get the userDbId, tree item parent chain upward: columns/indexes(folder:1) -> table(2) -> tables(folder:1) -> database(0)	
		CTreeItem dbTreeItem = treeItem.GetParent().GetParent();
		int pImage = -1, pSelImage = -1;
		ATLASSERT(dbTreeItem.GetImage(pImage, pSelImage) && pImage == 0);// 0 - database
		uint64_t userDbId = static_cast<uint64_t>(dbTreeItem.GetData());
		ATLASSERT(userDbId);
		
		loadColumsForTreeView(hColumnsFolderItem, userDbId, userTable);		
		loadIndexesForTreeView(hIndexesFolderItem, userDbId, userTable);
		dataView->Expand(hColumnsFolderItem);
		dataView->Expand(hIndexesFolderItem);
	}
}
