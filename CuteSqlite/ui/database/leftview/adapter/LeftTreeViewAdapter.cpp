#include "stdafx.h"
#include "LeftTreeViewAdapter.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include <common/AppContext.h>

LeftTreeViewAdapter::LeftTreeViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
	this->dataView->SetImageList(imageList);
}


LeftTreeViewAdapter::~LeftTreeViewAdapter()
{
	if (databaseIcon) ::DeleteObject(databaseIcon);
	if (folderIcon) ::DeleteObject(folderIcon);
	if (tableIcon) ::DeleteObject(tableIcon);
	if (fieldIcon) ::DeleteObject(fieldIcon);
	if (indexIcon) ::DeleteObject(indexIcon);
	if (viewIcon) ::DeleteObject(viewIcon);
	if (triggerIcon) ::DeleteObject(triggerIcon);
	if (imageList.IsNull()) imageList.Destroy();
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
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
		HTREEITEM hIndexesFolderItem = dataView->InsertItem(S(L"indexes").c_str(), 1, 1, hDbItem, TVI_LAST);
		HTREEITEM hViewsFolderItem = dataView->InsertItem(S(L"views").c_str(), 1, 1, hDbItem, TVI_LAST);
		HTREEITEM hTriggersFolderItem = dataView->InsertItem(S(L"triggers").c_str(), 1, 1, hDbItem, TVI_LAST);

		if (item.isActive) {
			hSelDbItem = hDbItem;
			databaseSupplier->setSeletedUserDbId(item.id);

			loadTablesForTreeView(hTablesFolderItem, item);
			loadDbIndexesForTreeView(hIndexesFolderItem, item);
			loadViewsForTreeView(hViewsFolderItem, item);
			loadTriggersForTreeView(hTriggersFolderItem, item);
		}
		dataView->Expand(hTablesFolderItem);
	}

	// Expand the Selected DB item
	if (hSelDbItem) {
		dataView->Expand(hSelDbItem);
		dataView->SelectItem(hSelDbItem);
		// init the supplier
		initDatabaseSupplier();
		AppContext::getInstance()->dispatch(Config::MSG_TREEVIEW_CLICK_ID, WPARAM(this), (LPARAM)hSelDbItem);
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

void LeftTreeViewAdapter::removeSeletedDbTreeItem()
{
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	if (!userDbId) {
		return ;
	}
	UserDb userDb;
	try {
		userDb = databaseService->getUserDb(userDbId);
		if (!userDb.id) {
			return;
		}
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}

	// if nImage == 0, seleted item is a item of user database
	std::wstring msg = S(L"delete-user-db");
	msg = StringUtil::replace(msg, L"{dbName}", userDb.name);
	if (QMessageBox::confirm(parentHwnd, msg) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return;
	}
	// Send Config::MSG_DELETE_DATABASE_ID to other window (database-RightWorkView, analysis-LeftNavigation, analysis-RightAnalysisView)
	AppContext::getInstance()->dispatchForResponse(Config::MSG_DELETE_DATABASE_ID, WPARAM(databaseSupplier->getSelectedUserDbId()));

	try {
		databaseService->removeUserDb(userDbId); 
		CTreeItem treeItem =  dataView->GetFirstVisibleItem();
		bool found = false;
		int iImage = -1, iSelImage = -1;
		while (!treeItem.IsNull()) {
			treeItem.GetImage(iImage, iSelImage); 
			uint64_t selectUserDbId = static_cast<uint64_t>(treeItem.GetData());
			if (selectUserDbId == userDbId && iImage == 0) { // 0 - database
				found = true;
				break;
			}
			treeItem = dataView->GetNextSiblingItem(treeItem);
		}

		if (found) {
			databaseSupplier->clearSelectedData(); // clear the selected runtime data
			dataView->DeleteItem(HTREEITEM(treeItem));
		}
		QPopAnimate::success(parentHwnd, S(L"delete-db-success-text"));
			
		loadDbs(); // reload dblist
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
}

void LeftTreeViewAdapter::selectDbTreeItem(uint64_t userDbId)
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

void LeftTreeViewAdapter::selectTableTreeItem(uint64_t userDbId, const std::wstring tableName)
{
	if (userDbId <= 0) {
		return ;
	}

	// find path : db tree item --> Tables folder tree item --> table tree item

	// 1.find db tree item
	CTreeItem treeItem = dataView->GetFirstVisibleItem();
	int nImage = -1, nSelImage = -1;
	while (!treeItem.IsNull()) {
		uint64_t itemDataId = static_cast<uint64_t>(dataView->GetItemData(treeItem.m_hTreeItem));
		if (itemDataId != userDbId) {
			treeItem = treeItem.GetNextSibling();
			continue;
		}
		// found db true, select the db item, then expand the item
		dataView->SelectItem(treeItem.m_hTreeItem);
		dataView->SetFocus();
		dataView->Expand(treeItem.m_hTreeItem);

		// 2.find "Tables" folder
		CTreeItem folderTreeItem = treeItem.GetChild();
		while (!folderTreeItem.IsNull()) {
			bool ret = folderTreeItem.GetImage(nImage, nSelImage);
			if (!ret || nImage != 1) {// 1 - folder
				folderTreeItem = folderTreeItem.GetNextSibling();
				continue;
			}
			wchar_t * cch = nullptr;
			std::wstring folderName;
			folderTreeItem.GetText(cch);
			if (!cch) {
				folderTreeItem = folderTreeItem.GetNextSibling();
				continue;
			}
			folderName.assign(cch);
			::SysFreeString(cch);
			if (folderName != S(L"tables")) {
				folderTreeItem = folderTreeItem.GetNextSibling();
				continue;
			}
			break;
		}
		if (folderTreeItem.IsNull()) {
			continue;
		}

		// 3.find table tree item
		CTreeItem tblTreeItem = folderTreeItem.GetChild();
		while (!tblTreeItem.IsNull()) {
			bool ret = tblTreeItem.GetImage(nImage, nSelImage);
			if (!ret || nImage != 2) { // 2 - table
				folderTreeItem = folderTreeItem.GetNextSibling();
				continue;
			}

			wchar_t * cch = nullptr;
			std::wstring theTblName;
			tblTreeItem.GetText(cch);
			if (!cch) {
				tblTreeItem = tblTreeItem.GetNextSibling();
				continue;
			}
			theTblName.assign(cch);
			::SysFreeString(cch);

			if (theTblName != tableName) {
				tblTreeItem = tblTreeItem.GetNextSibling();
				continue;
			}
			break;
		}
		if (tblTreeItem.IsNull()) {
			continue;
		}
		// 4.found it, then select it
		tblTreeItem.Select();
		break;		
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
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
		} catch (QSqlExecuteException &ex) {
			Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
			QPopAnimate::report(ex);
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
	databaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	folderIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\folder.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	tableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	fieldIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\field.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE); 
	indexIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\index.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	viewIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\view.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	triggerIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tree\\trigger.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(databaseIcon); // 0 - database
	imageList.AddIcon(folderIcon); // 1 - folder 
	imageList.AddIcon(tableIcon); // 2 - table 
	imageList.AddIcon(fieldIcon); // 3 - field/column
	imageList.AddIcon(indexIcon); // 4 - table index 
	imageList.AddIcon(viewIcon);// 5 - view
	imageList.AddIcon(triggerIcon);// 6 - trigger
	imageList.AddIcon(indexIcon);// 7 - db index
}

/**
 * When tree item changing, init the runtime data of DatabaseSupplier object .
 * 
 */
void LeftTreeViewAdapter::initDatabaseSupplier()
{
	CTreeItem treeItem = dataView->GetSelectedItem();
	ATLASSERT(!treeItem.IsNull());

	databaseSupplier->selectedUserDbId = 0;
	databaseSupplier->selectedSchema.clear();
	databaseSupplier->selectedTable.clear();
	databaseSupplier->selectedColumn.clear();
	databaseSupplier->selectedIndexName.clear();
	databaseSupplier->selectedViewName.clear();
	databaseSupplier->selectedTriggerName.clear();

	int nImage = -1, nSelImage = -1;
	treeItem.GetImage(nImage, nSelImage);

	wchar_t * cch = nullptr;
	treeItem.GetText(cch);
	ATLASSERT(cch);
	if (nImage == 0) { // 0 - database
		databaseSupplier->selectedUserDbId = (uint64_t)treeItem.GetData();
	} else if (nImage == 1) { // 1 - folder
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 2) { // 2 - table
		databaseSupplier->selectedTable.assign(cch);
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 3) { // 3 - field /column
		std::wstring columns;
		columns.assign(cch);
		size_t pos1 = columns.find_first_of(L'[');
		size_t pos2 = columns.find_last_of(L']');
		if (pos1 != std::wstring::npos && pos2 != std::wstring::npos) {
			columns = columns.substr(0, pos1 - 1);
			StringUtil::trim(columns);
			databaseSupplier->selectedColumn = columns;
		}		
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 4) { // 4 - table index
		databaseSupplier->selectedIndexName.assign(cch);
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 5) { // 5 - view		
		databaseSupplier->selectedViewName.assign(cch);
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 6) { // 6 - trigger
		databaseSupplier->selectedTriggerName.assign(cch);
		doTrackParentTreeItemForSupplier(treeItem);
	} else if (nImage == 7) { // 7 - db index
		databaseSupplier->selectedIndexName.assign(cch);
		doTrackParentTreeItemForSupplier(treeItem);
	}
	::SysFreeString(cch);
}

void LeftTreeViewAdapter::doTrackParentTreeItemForSupplier(CTreeItem &treeItem)
{
	CTreeItem pTreeItem = treeItem.GetParent();
	while (!pTreeItem.IsNull()) {
		int nImage = -1, nSelImage = -1;
		pTreeItem.GetImage(nImage, nSelImage);
		if (nImage == 2) { // 2 - table
			wchar_t * cch = nullptr;
			pTreeItem.GetText(cch);
			if (cch) {
				databaseSupplier->selectedTable.assign(cch);
			}
			::SysFreeString(cch);
		} else if (nImage == 0) {// 0- database
			databaseSupplier->selectedUserDbId = (uint64_t)pTreeItem.GetData();

			int nCurImage = -1, nCurSelImage = -1;
			treeItem.GetImage(nCurImage, nCurSelImage);
			if (nCurImage == 7) {// 7 - db index
				if (treeItem.GetData() == 0) {
					return;
				}
				uint64_t rowId = static_cast<uint64_t>(treeItem.GetData());
				try {
					UserIndex userIndex;
					userIndex = tableService->getUserIndexByRowId(databaseSupplier->selectedUserDbId, rowId);
					databaseSupplier->selectedTable = userIndex.tblName;
				} catch (QSqlExecuteException &ex) {
					Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
					QPopAnimate::report(ex);
				}				
			}

			break;
		}
		pTreeItem = pTreeItem.GetParent();
	}
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
		UserTableList tableList = tableService->getUserTables(userDb.id);
		for (UserTable item : tableList) {
			HTREEITEM hTblItem = dataView->InsertItem(item.name.c_str(), 2, 2, hTablesFolderItem, TVI_LAST); 

			HTREEITEM hColumnsFolderItem = dataView->InsertItem(S(L"columns").c_str(), 1, 1, hTblItem, TVI_LAST);
			HTREEITEM hIndexesFolderItem = dataView->InsertItem(S(L"indexes").c_str(), 1, 1, hTblItem, TVI_LAST);

			if (isLoadColumnsAndIndex) {
				loadTblColumsForTreeView(hColumnsFolderItem, userDb.id, item);
				loadTblIndexesForTreeView(hIndexesFolderItem, userDb.id, item);
			}			
		}		
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
	
}

void LeftTreeViewAdapter::loadDbIndexesForTreeView(HTREEITEM hIndexesFolderItem, UserDb & userDb)
{
	try {
		UserIndexList indexList = databaseService->getUserIndexes(userDb.id);
		for (UserTable & item : indexList) {
			CTreeItem indexItem  = dataView->InsertItem(item.name.c_str(), 7, 7, hIndexesFolderItem, TVI_LAST);
			indexItem.SetData((DWORD_PTR)item.rowId);
						
		}		
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
		for (UserTable & item : list) {
			dataView->InsertItem(item.name.c_str(), 5, 5, hViewsFolderItem, TVI_LAST);
		}
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
		for (UserTable & item : list) {
			dataView->InsertItem(item.name.c_str(), 6, 6, hTriggersFolderItem, TVI_LAST);
		}
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
}

void LeftTreeViewAdapter::loadTblColumsForTreeView(HTREEITEM hColumnsFolderItem, uint64_t userDbId, UserTable & userTable)
{
	try {
		ColumnInfoList list = tableService->getUserColumns(userDbId, userTable.name);
		for (ColumnInfo & item : list) {
			std::wstring field = item.name;
			field.append(L" [").append(item.type).append(L", ").append(item.notnull ? L"NOT NULL" : L"NULL").append(L"]");
			CTreeItem treeItem = dataView->InsertItem(field.c_str(), 3, 3, hColumnsFolderItem, TVI_LAST);
			treeItem.SetData((DWORD_PTR)item.cid);
		}
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
}

void LeftTreeViewAdapter::loadTblIndexesForTreeView(HTREEITEM hIndexesFolderItem, uint64_t userDbId, UserTable & userTable)
{
	try {
		IndexInfoList list = tableService->getIndexInfoList(userDbId, userTable.name);
		for (IndexInfo & item : list) {
			std::wstring name = !item.name.empty() ? item.name
				: item.type + L"(" + item.columns + L")";
			dataView->InsertItem(name.c_str(), 4, 4, hIndexesFolderItem, TVI_LAST);
		}
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
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
		HTREEITEM hIndexesFolderItem = getChildFolderItem(hSelTreeItem, S(L"indexes"));
		HTREEITEM hViewsFolderItem = getChildFolderItem(hSelTreeItem, S(L"views"));
		HTREEITEM hTriggersFolderItem = getChildFolderItem(hSelTreeItem, S(L"triggers"));

		// if folder item has children, then it is loaded before
		if ((hTablesFolderItem && dataView->ItemHasChildren(hTablesFolderItem))
			|| (hIndexesFolderItem && dataView->ItemHasChildren(hIndexesFolderItem))
			|| (hViewsFolderItem && dataView->ItemHasChildren(hViewsFolderItem))
			|| (hTriggersFolderItem && dataView->ItemHasChildren(hTriggersFolderItem))) {
			return ;
		}

		// reload
		loadTablesForTreeView(hTablesFolderItem, userDb);
		loadDbIndexesForTreeView(hIndexesFolderItem, userDb);
		loadViewsForTreeView(hViewsFolderItem, userDb);
		loadTriggersForTreeView(hTriggersFolderItem, userDb);
				
		dataView->Expand(hTablesFolderItem);
		dataView->SelectItem(hSelTreeItem);
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
		
		loadTblColumsForTreeView(hColumnsFolderItem, userDbId, userTable);		
		loadTblIndexesForTreeView(hIndexesFolderItem, userDbId, userTable);
		
		dataView->Expand(hColumnsFolderItem);
		dataView->Expand(hIndexesFolderItem);
		dataView->SelectItem(hSelTreeItem);
	}
}
