#pragma once
#include <string>
#include <vector>

#define USER_FROM_ID 250 

#define FORM_BUTTON_WIDTH	24
#define FORM_BUTTON_HEIGHT	24

#define QIMAGE_LIST_ITEM_WIDTH 224
#define QIMAGE_LIST_ITEM_HEIGHT 126

namespace Config {

//PANEL的ID
typedef enum {
	HOME_PANEL = WM_USER + 1,
	DATABASE_PANEL,
	ANALYSIS_PANEL,
	VIEW_PANEL,
	SETTING_PANEL,
	// 不列入菜单项的PANEL
	ANALYSIS_LIST_PANEL,
	TAKEN_IMAGE_PANEL,
	SAMPLE_SETTINGS_PANEL
} PanelId;


//框架用的工具栏上的按钮ID
typedef enum {
	UNSED_BUTTON_ID = 0,
	HOME_BUTTON_ID = WM_USER + 10,
	DATABASE_BUTTON_ID,
	ANALYSIS_BUTTON_ID,
	VIEW_BUTTON_ID,
	SETTING_BUTTON_ID,
} FrmButtonId ;

// button id
typedef enum {
	// YES/NO
	CUSTOMER_FORM_YES_BUTTON_ID = WM_USER + 40,
	CUSTOMER_FORM_NO_BUTTON_ID,
	QIMAGE_PREV_PAGE_BUTTON_ID,
	QIMAGE_NEXT_PAGE_BUTTON_ID,

	// TREE VIEW
	DATABASE_CREATE_BUTTON_ID,
	DATABASE_OPEN_BUTTON_ID,
	DATABASE_REFRESH_BUTTON_ID,
	DATABASE_DELETE_BUTTON_ID,
	DATABASE_COPY_BUTTON_ID,

	// RIGHT VIEW TOOLBAR - EXEC SQL
	DATABASE_EXEC_SQL_BUTTON_ID = WM_USER + 50,
	DATABASE_EXEC_ALL_BUTTON_ID,
	DATABASE_EXPLAIN_SQL_BUTTON_ID,
	DATABASE_QUERY_BUTTON_ID,
	DATABASE_HISTORY_BUTTON_ID,
	// RIGHT VIEW TOOLBAR - SAVE
	DATABASE_SAVE_BUTTON_ID,
	DATABASE_SAVE_ALL_BUTTON_ID,
	// RIGHT VIEW TOOLBAR - DATABASE
	DATABASE_EXPORT_BUTTON_ID,
	DATABASE_IMPORT_BUTTON_ID,
	// RIGHT VIEW TOOLBAR - TABLE
	DATABASE_CREATE_TABLE_BUTTON_ID,
	DATABASE_OPEN_TABLE_BUTTON_ID,
	DATABASE_ALTER_TABLE_BUTTON_ID,
	DATABASE_RENAME_TABLE_BUTTON_ID,
	DATABASE_TRUNCATE_TABLE_BUTTON_ID,
	DATABASE_DROP_TABLE_BUTTON_ID,
	DATABASE_COPY_TABLE_BUTTON_ID,
	DATABASE_SHARDING_TABLE_BUTTON_ID,
	DATABASE_EXPORT_TABLE_BUTTON_ID,
	DATABASE_IMPORT_TABLE_FROM_SQL_BUTTON_ID,
	DATABASE_IMPORT_TABLE_FROM_CSV_BUTTON_ID,

	//LIST VIEW TOOLBAR
	LISTVIEW_EXPORT_BUTTON_ID = WM_USER + 70,
	LISTVIEW_COPY_BUTTON_ID,
	LISTVIEW_FILTER_BUTTON_ID,
	LISTVIEW_REFRESH_BUTTON_ID,
	LISTVIEW_NEW_ROW_BUTTON_ID,
	LISTVIEW_COPY_ROW_BUTTON_ID,
	LISTVIEW_SAVE_BUTTON_ID,
	LISTVIEW_DELETE_BUTTON_ID,
	LISTVIEW_CANCEL_BUTTON_ID,

	QDIALOG_CLEAR_BUTTON_ID,
	QDIALOG_YES_BUTTON_ID = WM_USER + 81,
	QDIALOG_NO_BUTTON_ID,

	// EXPORT-FEILDS
	EXPORT_SELECT_ALL_FIELDS_BUTTON_ID,
	EXPORT_DESELECT_ALL_FIELDS_BUTTON_ID,
	EXPORT_OPEN_FILE_BUTTON_ID,

	// EXPORT DB AS SQL
	EXPORT_DB_AS_SQL_OPEN_FILE_BUTTON_ID,
	EXPORT_DB_AS_SQL_SELECT_ALL_OBJECTS_BUTTON_ID,
	EXPORT_DB_AS_SQL_DESELECT_ALL_OBJECTS_BUTTON_ID,

	// IMPORT DB FROM SQL
	IMPORT_OPEN_FILE_BUTTON_ID,

	// NEW TABLE PAGE
	TABLE_NEW_COLUMN_BUTTON_ID,
	TABLE_DEL_COLUMN_BUTTON_ID,
	TABLE_UP_COLUMN_BUTTON_ID,
	TABLE_DOWN_COLUMN_BUTTON_ID,
	TABLE_NEW_INDEX_BUTTON_ID,
	TABLE_DEL_INDEX_BUTTON_ID,
	TABLE_NEW_FOREIGNKEY_BUTTON_ID,
	TABLE_DEL_FOREIGNKEY_BUTTON_ID,
	TABLE_SAVE_BUTTON_ID,
	TABLE_REVERT_BUTTON_ID,

	// TABLE INDEX DIALOG
	TABLE_INDEX_MOVE_LEFT_BUTTON_ID,
	TABLE_INDEX_MOVE_RIGHT_BUTTON_ID,
	TABLE_INDEX_MOVE_UP_BUTTON_ID,
	TABLE_INDEX_MOVE_DOWN_BUTTON_ID,

	// QPOP ANIMATE
	CLOSE_IMAGE_BUTTON_ID,

	// COPY TABLE DIALOG
	COPYTABLE_SHARDING_STRATEGY_EXPRESS_BUTTON_ID,
	PREVIEW_SQL_BUTTON_ID,

	// TABLE PREOPERTY
	DATABSE_PROPERTIES_REFRESH_BUTTON_ID,
	TABLE_PROPERTIES_REFRESH_BUTTON_ID,

	//QUERY PAGE EDITOR
	EDITOR_TEMPLATES_BUTTON_ID,
	EDITOR_PRAGMAS_BUTTON_ID,
	EDITOR_SQL_LOG_BUTTON_ID,
	EDITOR_CLEAR_ALL_BUTTON_ID,

	// COMMON SEARCH EDIT
	COMMON_SEARCH_BUTTON_ID,
	
	// SQL LOG LIST ITEM
	SQL_LOG_ITEM_ANALYSIS_BUTTON_ID,
	SQL_LOG_ITEM_USE_BUTTON_ID,
	SQL_LOG_ITEM_EXPLAIN_BUTTON_ID,
	SQL_LOG_ITEM_TOP_BUTTON_ID,
	SQL_LOG_ITEM_COPY_BUTTON_ID,
	SQL_LOG_ITEM_DELELE_BUTTON_ID,

	// PAGE BUTTONS
	FIRST_PAGE_BUTTON_ID,
	PREV_PAGE_BUTTON_ID,
	NEXT_PAGE_BUTTON_ID,
	LAST_PAGE_BUTTON_ID,

	// HOME PANEL
	HOME_CREATE_DB_BUTTON_ID,
	HOME_MOD_DB_BUTTON_ID,

	// ANALYSIS PANEL
	ANALYSIS_SQL_LOG_BUTTON_ID = WM_USER + 150,
	ANALYSIS_ADD_SQL_TO_ANALYSIS_BUTTON_ID,
	ANALYSIS_SAVE_BUTTON_ID,
	ANALYSIS_SAVE_ALL_BUTTON_ID,
	ANALYSIS_CREATE_INDEX_BUTTON_ID,

	// SETTING PANEL
	SETTING_FEEDBACK_SUBMIT_BUTTON_ID,
	
} ButtonId;

// TabView id
typedef enum {
	DATABASE_WORK_TAB_VIEW_ID = WM_USER + 170,
	DATABASE_RESULT_TAB_VIEW_ID ,
	DATABASE_TABLE_TAB_VIEW_ID,
} TabViewId;

typedef enum {
	// TREE VIEW
	TREEVIEW_SELECTED_DB_COMBOBOX_ID = WM_USER + 180,
	//LIST VIEW TOOLBAR
	LISTVIEW_READ_WRITE_COMBOBOX_ID,

	// EXPORT DATABASE AS SQL
	EXPORT_DB_AS_SQL_SELECT_DB_COMBOBOX_ID,
	// IMPORT DATABASE 
	IMPORT_TARGET_DB_COMBOBOX_ID,
	IMPORT_TARGET_TBL_COMBOBOX_ID,

	//NEW TABLE 
	TABLE_DATABASE_COMBOBOX_ID,
	TABLE_SCHEMA_COMBOBOX_ID,

	// QLIST VIEW COMBOBOX
	QLISTVIEWCTRL_SUBITEM_COMBOBOX_ID,

	// IMPORT BY CSV DIALOG
	CSV_FIELD_TERMINAATED_BY_COMBOBOX_ID,
	CSV_LINE_TERMINAATED_BY_COMBOBOX_ID,
	CSV_NULL_AS_KEYWORD_COMBOBOX_ID,
	CSV_CHARSET_COMBOBOX_ID,

	// SETTING PANEL
	SETTING_LANGUAGE_COMBOBOX_ID,

	// ANALYSIS PANEL - STORE_ANALYSIS_PAGE
	ANALYSIS_DB_STORE_TBL_COMBOBOX_ID,
	// ANALYSIS PANEL - DB_PRAGMA_PARAMS_PAGE
	ANALYSIS_DB_PRAGMAS_COMBOBOX_ID,
} ComboBoxId;


typedef enum {
	DATABASE_TREEVIEW_ID = WM_USER + 220,
	DATABASE_OBJECTS_TREEVIEW_ID,
	ANALYSIS_NAVIGATION_TREEVIEW_ID,
} TreeViewId;


typedef enum {
	DATABASE_QUERY_EDITOR_ID = WM_USER + 230,
	// LIST VIEW 
	LISTVIEW_OFFSET_EDIT_ID,
	LISTVIEW_LIMIT_EDIT_ID,
	// EXPORT RESULT
	EXPORT_CSV_FIELD_TERMINAATED_BY_EDIT_ID,
	CSV_FIELD_ENCLOSED_BY_EDIT_ID,
	CSV_FIELD_ESCAPED_BY_EDIT_ID,
	EXPORT_CSV_LINE_TERMINAATED_BY_EDIT_ID,
	EXPORT_CSV_CHARSET_EDIT_ID,
	EXPORT_EXCEL_COLUMN_MAX_SIZE_EDIT_ID,
	EXPORT_EXCEL_DECIMAL_PLACES_EDIT_ID,
	EXPORT_PATH_EDIT_ID,
	// RESULT
	QLISTVIEWCTRL_SUBITEM_EDIT_ID,
	INFOPAGE_INFO_EDIT_ID,
	// EXPORT DATABASE AS SQL
	EXPORT_DB_AS_SQL_PATH_EDIT_ID,
	// IMPORT DATABASE FROM SQL
	IMPORT_SOURCE_PATH_EDIT_ID,

	// NEW TABLE
	TABLE_TBL_NAME_EDIT_ID,
	TABLE_SQL_PREVIEW_EDIT_ID,

	//VIEW DIALOG
	VIEWDIALOG_VIEW_NAME_EDIT_ID,
	VIEWDIALOG_TRIGGER_NAME_EDIT_ID,

	// COPY TABLE DIALOG
	COPYTABLE_FROM_DB_EDIT_ID,
	COPYTABLE_FROM_TBL_EDIT_ID,
	COPYTABLE_TARGET_TABLE_EDIT_ID,
	COPYTABLE_TBL_SUFFER_BEGIN_EDIT_ID,
	COPYTABLE_TBL_SUFFER_END_EDIT_ID,
	COPYTABLE_SHARDING_RANGE_EDIT_ID,
	COPYTABLE_SHARDING_STRATEGY_EXPRESS_EDIT_ID,

	// PREVIEW SQL DIALOG
	PREVIEW_SQL_EDITOR_ID,
	// COPY DATA - EXPRESS DIALOG
	EXPRESS_EDITOR_ID,

	// SEARCH EDIT
	SEARCH_EDIT_ID,

	// HOMEPANEL - DB LIST ITEM
	DBLIST_ITEM_PATH_EDIT_ID,

	//SETTINGPANEL 
	FEEDBACK_CONTENT_EDIT_ID,
	FEEDBACK_EMAIL_EDIT_ID
} EditorId;

typedef enum {
	// QUERY RESULT
	DATABASE_QUERY_LISTVIEW_ID = WM_USER + 260,
	// NEW TABLE/MODIFY TABLE
	DATABASE_TABLE_COLUMNS_LISTVIEW_ID,
	DATABASE_TABLE_INDEXES_LISTVIEW_ID,
	DATABASE_TABLE_FOREIGNKEYS_LISTVIEW_ID,

	// IMPORT FROM CSV
	IMPORT_COLUMN_LISTVIEW_ID,
	IMPORT_DATA_LISTVIEW_ID,
} ListViewId;

typedef enum {
	//LIST VIEW TOOLBAR
	LISTVIEW_FORMVIEW_CHECKBOX_ID = WM_USER + 270,
	LISTVIEW_LIMIT_CHECKBOX_ID,
	//EXPORT - CSV OPTIONS
	EXPORT_CSV_COLUMN_NAME_CHECKBOX_ID,
	CSV_COLUMN_NAME_CHECKBOX_ID,

	// EXPORT DATABASE AS SQL
	EXPORT_RETAIN_COLUMN_CHECKBOX_ID,
	EXPORT_CONTAIN_MULTI_ROWS_CHECKBOX_ID,

	//IMPORT DABASE FROM SQL
	IMPORT_ABORT_ON_ERROR_CHECKBOX_ID,

	// COPY TABLE DIALOG
	COPYTABLE_TARGET_DB_COMBOBOX_ID,
	COPYTABLE_ENABLE_TABLE_SHARDING_CHECKBOX_ID,
	COPYTABLE_ENABLE_SHARDING_STRATEGY_CHECKBOX_ID,
} CheckBoxId;

typedef enum {
	//LIST VIEW TOOLBAR
	EXPORT_TO_CSV_RADIO_ID = WM_USER + 290,
	EXPORT_TO_JSON_RADIO_ID,
	EXPORT_TO_XML_RADIO_ID,
	EXPORT_TO_HTML_RADIO_ID,
	EXPORT_TO_EXCEL_XML_RADIO_ID,
	EXPORT_TO_SQL_RADIO_ID,
	// SQL SETTING
	STRUCTURE_ONLY_RADIO_ID,
	DATA_ONLY_RADIO_ID,
	STRUCTURE_DATA_RADIO_ID,

	// EXPORT DATABASE AS SQL DIALOG
	EXPORT_RETAIN_TABLE_RADIO_ID,
	EXPORT_OVERRIDE_TABLE_RADIO_ID,
} RadioId;

typedef enum {
	//LIST VIEW TOOLBAR
	EXPORT_TO_CSV_GROUPBOX_ID = WM_USER + 300,
	EXPORT_TO_CSV_FEILD_SET_GROUPBOX_ID,
	EXPORT_TO_CSV_LINE_SET_GROUPBOX_ID,
	EXPORT_TO_CSV_CHARSET_SET_GROUPBOX_ID,
	EXPORT_TO_EXCEL_GROUPBOX_ID,
	EXPORT_TO_SQL_GROUPBOX_ID,
	EXPORT_FEILDS_GROUPBOX_ID,

	// EXPORT DATABASE AS SQL DIALOG
	EXPORT_AS_SQL_STRUCTURE_AND_DATA_GROUPBOX_ID,
	EXPORT_AS_SQL_INSERT_STATEMENT_GROUPBOX_ID,
	EXPORT_AS_SQL_TBL_STATEMENT_GROUPBOX_ID,

} GroupBoxId;

typedef enum {
	EXPORT_SELECT_FIELDS_LISTBOX_ID = WM_USER + 320,

	//TABLE INDEX SEL COLUMNS
	TABLE_INDEX_TO_BE_SELECTED_COLUMNS_LISTBOX_ID,
	TABLE_INDEX_SELECTED_COLUMNS_LISTBOX_ID,

	// COPY TABLE - EXPRESS DIALOG
	EXPRESS_FUNCTIONS_LISTBOX_ID,
	EXPRESS_COLUMNS_LISTBOX_ID,

	// SQL LOG DIALOG
	DIALOG_SQL_LOG_LIST_ID,

	// SETTING PANEL
	SETTING_MENU_LIST_ID,
} ListBoxId;

typedef enum {
	// list view copy menu
	COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID = WM_USER + 340,
	COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID,
	COPY_ALL_ROWS_AS_SQL_MEMU_ID,
	COPY_SEL_ROWS_AS_SQL_MEMU_ID,
	// treeview popup menu for database
	DATABASE_CREATE_MENU_ID = WM_USER + 360,
	DATABASE_OPEN_MENU_ID,
	DATABASE_REFRESH_MENU_ID,
	DATABASE_DELETE_MENU_ID,
	DATABASE_COPY_MENU_ID,
	DATABASE_EXPORT_AS_SQL_MENU_ID,
	DATABASE_IMPORT_FROM_SQL_MENU_ID,
	DATABASE_NEW_MENU_ID,
	DATABASE_NEW_TABLE_MENU_ID,
	DATABASE_NEW_VIEW_MENU_ID,
	DATABASE_NEW_TRIGGER_MENU_ID,

	// treeview popup menu for views
	DATABASE_OPEN_VIEW_MENU_ID,
	DATABASE_DROP_VIEW_MENU_ID,

	// treeview popup menu for views
	DATABASE_OPEN_TRIGGER_MENU_ID,
	DATABASE_DROP_TRIGGER_MENU_ID,

	// treeview popup menu for table
	TABLE_OPEN_MENU_ID = WM_USER + 380,
	TABLE_CREATE_MENU_ID,
	TABLE_ALTER_MENU_ID,
	TABLE_RENAME_MENU_ID,
	TABLE_TRUNCATE_MENU_ID,
	TABLE_DROP_MENU_ID,
	TABLE_COPY_MENU_ID,
	TABLE_SHARDING_MENU_ID,
	TABLE_EXPORT_MENU_ID,
	TABLE_IMPORT_SQL_MENU_ID,
	TABLE_IMPORT_CSV_MENU_ID,
	TABLE_MANAGE_INDEX_MENU_ID,
	TABLE_MANAGE_FOREIGNKEY_MENU_ID,
	TABLE_PROPERTIES_MENU_ID,
	TABLE_MANAGE_COLUMNS_MENU_ID,
	TABLE_DROP_COLUMN_MENU_ID,
	TABLE_DROP_INDEX_MENU_ID,

	// query page templates menu
	TEMPLATES_SELECT_STMT_MEMU_ID  = WM_USER + 480,
	TEMPLATES_INSERT_STMT_MEMU_ID = WM_USER + 481,
	TEMPLATES_UPDATE_STMT_MEMU_ID = WM_USER + 482,
	TEMPLATES_DELETE_STMT_MEMU_ID = WM_USER + 483,
	TEMPLATES_REPLACE_STMT_MEMU_ID = WM_USER + 484,

	TEMPLATES_CREATE_TABLE_STMT_MEMU_ID = WM_USER + 485,
	TEMPLATES_DROP_TABLE_STMT_MEMU_ID = WM_USER + 486,
	TEMPLATES_TRUNCATE_TABLE_STMT_MEMU_ID = WM_USER + 487,
	TEMPLATES_ALTER_TABLE_STMT_MEMU_ID = WM_USER + 488,
	TEMPLATES_RENAME_TABLE_STMT_MEMU_ID = WM_USER + 489,

	TEMPLATES_CREATE_INDEX_STMT_MEMU_ID = WM_USER + 490,
	TEMPLATES_DROP_INDEX_STMT_MEMU_ID = WM_USER + 491,

	TEMPLATES_CREATE_VIEW_STMT_MEMU_ID = WM_USER + 492,
	TEMPLATES_DROP_VIEW_STMT_MEMU_ID = WM_USER + 493,

	TEMPLATES_CREATE_TRIGGER_STMT_MEMU_ID = WM_USER + 494,
	TEMPLATES_DROP_TRIGGER_STMT_MEMU_ID = WM_USER + 495,

	TEMPLATES_WITH_STMT_MEMU_ID = WM_USER + 496,

	TABVIEW_SAVE_THIS_MENU_ID,
	TABVIEW_CLOSE_THIS_MENU_ID,
	TABVIEW_CLOSE_OTHERS_MENU_ID,
	TABVIEW_CLOSE_LEFT_MENU_ID,
	TABVIEW_CLOSE_RIGHT_MENU_ID,

	ANALYSIS_OPEN_PERF_REPORT_MENU_ID = WM_USER + 510,
	ANALYSIS_DROP_PERF_REPORT_MENU_ID,
} MenuId;

// PostMessage messageId
typedef enum {
	MSG_TIMER_MOVE_ID = WM_USER + 540, // QPopAnimate 触发的移动定时器
	MSG_TIMER_DELAY_ID, // QPopAnimate 触发的悬停定时器
	MSG_QIMAGE_CLICK_ID, // 点击QStaticImage,如果命中右下角小图标，则调用上一个消息ID， 否则，给父窗口发送此消息 wParam带上被点击的窗口ID lParam带上窗口句柄hwnd
	MSG_QIMAGE_CONERICON_CLICK_ID, // 点击QStaticImage图片右下角小图标（拍照等），给父窗口发送此消息 wParam带上被点击的窗口ID lParam带上窗口句柄hwnd
	MSG_QIMAGELIST_CLICK_ID, // 单击QImageList图片列表，给父窗口发送消息，wParam为被选中项下标nSelItem，lParam带上被选中项的窗口句柄
	MSG_TREEVIEW_CLICK_ID, // 单击LeftTreeView::treeView的选中项，发送该消息，接收方wParam为CTreeViewCtrlEx *指针, lParam 是HTREEITEM指针，接收方通过lParam获得需要的数据
	MSG_TREEVIEW_DBCLICK_ID, // 双击LeftTreeView::treeView的选中项，发送该消息，接收方wParam为CTreeViewCtrlEx *指针, lParam 是HTREEITEM指针，接收方通过lParam获得需要的数据
	MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, // QListViewCtrl的子项（subItem）的文本发生改变时，向父窗口发送该消息,wParam=iItem, lParam=iSubItem
	MSG_QLISTVIEW_SUBITEM_CHECKBOX_CHANGE_ID, // QListViewCtrl的子项（subItem）的CheckBox发生改变时，向父窗口发送该消息,wParam=iItem, lParam=iSubItem
	MSG_QLISTVIEW_ITEM_CHECKBOX_CHANGE_ID, // QListViewCtrl的项（item）的CheckBox发生改变时，向父窗口发送该消息,wParam=iItem, lParam=iSubItem
	MSG_QLISTVIEW_COLUMN_CLICK_ID, // QListViewCtrl的Column被点击时，向父窗口发送该消息,wParam=iItem, lParam=(LPNMHEADER)lParam
	MSG_EXEC_SQL_RESULT_MESSAGE_ID, // 执行SQL语句后，返回的消息，wParam- NULL，lParam - point of adapter.runtimeResultInfo
	MSG_EXPORT_DB_AS_SQL_PROCESS_ID, // 导出数据库为SQL对话框进度的消息,wParam是完成状态，lParam是完成百分比
	MSG_IMPORT_PROCESS_ID, // SQL导入数据库对话框进度的消息,wParam是完成状态，lParam是完成百分比
	MSG_MENU_ITEM_CLICK_ID, //菜单栏下某个菜单项被点击的消息ID，用来通知父窗口，指定ID的菜单项被点击了
	MSG_ADD_DATABASE_ID, // Send this msg when clicking "Add Database/Open Database" menu or toolbar button , wParam=userDbId, lParam=NULL
	MSG_DELETE_DATABASE_ID, // Send this msg when clicking "Delete Database" menu or toolbar button , wParam=userDbId, lParam=NULL
	MSG_NEW_TABLE_ID, // Send this msg when clicking "New table" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_NEW_VIEW_ID, // Send this msg when clicking "New view" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_OPEN_VIEW_ID, // Send this msg when clicking "Open view" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_DROP_VIEW_ID, // Send this msg when clicking "Drop view" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_NEW_TRIGGER_ID, // Send this msg when clicking "New trigger" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_OPEN_TRIGGER_ID, // Send this msg when clicking "Open trigger" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_DROP_TRIGGER_ID, // Send this msg when clicking "Drop trigger" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_TABLE_COLUMNS_CHANGE_PRIMARY_KEY_ID, // Send this msg when changing table columns index from the TableColumnsPage to TableIndexesPage, wParam=NULL, lParam=NULL
	MSG_TABLE_COLUMNS_CHANGE_COLUMN_NAME_ID, // Send this msg when changing column from the TableColumnsPage to TableIndexesPage, wParam=oldColumnName, lParam=newColumnName
	MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, // Send this msg when deleting column from the TableColumnsPage to TableIndexesPage, wParam=iItem, lParam=iSubItem
	MSG_TABLE_INDEX_CREATE_ID, // Send this msg when need create a new index use columns in the TableIndexesPage, wParam=Columns, lParam=indexType(index, primary key, unique)
	MSG_TABLE_PREVIEW_SQL_ID, // Send this msg when changing table columns/index from the TableColumnsPage/TableIndexesPage to TableStructurePage, wParam=NULL, lParam=NULL
	MSG_QTABVIEW_CHANGE_PAGE_TITLE, // Send this msg when changing tab view title caption, wParam=(page index), lParam=NULL
	MSG_LEFTVIEW_REFRESH_DATABASE_ID, // Send this msg when creating a table or altering a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_CREATE_DATABASE_ID, // Send this msg when creating a table, wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_RENAME_TABLE_ID, // Send this msg when creating a table or altering a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_TRUNCATE_TABLE_ID, // Send this msg when truncate a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_DROP_TABLE_ID, // Send this msg when drop a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_COPY_TABLE_ID, // Send this msg when copy a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_SHARDING_TABLE_ID, // Send this msg when sharding a table , wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_EXPORT_TABLE_ID, // Send this msg when export a table, wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_IMPORT_TABLE_SQL_ID, // Send this msg when import a table from sql file, wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_IMPORT_TABLE_CSV_ID, // Send this msg when import a table from csv file, wParam = NULL, lParam=NULL
	MSG_LEFTVIEW_SELECT_TABLE_ID, // Send this msg when need select table, wParam = userDbId, lParam=tableName string pointer
	MSG_SHOW_TABLE_DATA_ID, // Send this msg when clicking the table open menu , wParam = NULL, lParam=NULL
	MSG_ALTER_TABLE_ID, // Send this msg when clicking "Alter table" menu or toolbar button , wParam=TABLE_COLUMNS_PAGE/TABLE_INDEX_PAGE/TABLE_FOREIGN_KEYS_PAGE, lParam=NULL
	MSG_RENAME_TABLE_ID, // Send this msg when clicking "Rename table" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_REFRESH_SAME_TABLE_DATA_ID, // Send this msg when clicking "Truncate table" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_DROP_TABLE_ID, // Send this msg when clicking "Drop table" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_COPY_TABLE_ID, // Send this msg when clicking "Copy table" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_SHARDING_TABLE_ID, // Send this msg when clicking "Sharding" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_EXPORT_TABLE_ID, // Send this msg when clicking "Export table as..." menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_TABLE_IMPORT_SQL_ID, // Send this msg when clicking "Import From SQL File" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_TABLE_IMPORT_CSV_ID, // Send this msg when clicking "Import From CSV File" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_TABLE_MANAGE_INDEX_ID, // Send this msg when clicking "Manage Indexes" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_TABLE_PROPERTIES_ID, // Send this msg when clicking "Properties" menu or toolbar button , wParam=NULL, lParam=NULL
	MSG_COPY_TABLE_PROCESS_ID, // Table CopyTableDialog progress message, wParam is the completion status, lParam is the percent complete
	MSG_LOADING_SQL_PREVIEW_EDIT_ID, // When PreviewSqlDialog loading the sql editor,will be send this msg to his parent window, parent window handle this message, wParam=sqlEditorPtr, lParam=null  
	MSG_SEARCH_BUTTON_ID, // When search edit button has clicked, send this msg to parent window, wParam=NULL, lParam = NULL	
	
	MSG_USE_SQL_ID, // When use button has clicked in the SqlLogDialog/SqlLogPage, send this msg to SqlLogDialog/SqlLogPage, wParam=NULL, lParam = NULL
	MSG_TOP_SQL_LOG_ID, // When top button has clicked in the SqlLogDialog/SqlLogPage, send this msg to SqlLogDialog/SqlLogPage, wParam=SqlLog.id, lParam = NULL
	MSG_DEL_SQL_LOG_ID, // When DELETE button has clicked in the SqlLogDialog/SqlLogPage, send this msg to SqlLogDialog/SqlLogPage, wParam=SqlLog.id, lParam= NULL
	MSG_QUERY_PAGE_NEXT_PAGE_SQL_LOG_ID, // When SqlLogListBox v-scroll scroll down 70% in the SqlLogDialog/SqlLogPage, send this msg to SqlLogDialog/SqlLogPage, wParam=si.nPos, lParam=si.nMax
	MSG_DATA_DIRTY_ID, // When data has be changed in ResultTablePgae, send this msg to parent tabView to display dirty icon for QueryPage title,  wParam=resultTablePage.m_hWnd, lParam=isDirty
	MSG_TABLE_STRUCTURE_DIRTY_ID, // When table struct has be changed in TableColumnsPage/TableIndexesPage/TableForeignKeysPage, 
								  //  send this msg to parent tabView to display dirty icon for TableStruecturePage title icon, wParam=dirtyPage.m_hWnd, lParam=isDirty
	MSG_DATA_HAS_CHANGED_ID, // When the MainFrm close, send this msg to all the window that it has subscribed this msgId, to confirm whether data has changed, 
							 // wParam=NULL, lParam=null, and return 1 - No changed, 0 - Has Changed
	MSG_ACTIVE_PANEL_ID, // When need to display DatabasePanel in LeftPanel, send this msg to LeftPanel for displaying. wParam=panelId, lParam = NULL
	MSG_DBLIST_ITEM_CLICK_ID, // when database list item has been clicked in HomePanel, send this msg to HomePanel. wParam=item.nID, lParam = userDbId
	MSG_SAVE_ID, // Send the msg to the current active page need saved, wParam=NULL, lParam = NULL
	MSG_SAVE_ALL_ID,// Send the msg to all the pages need saved, wParam=NULL, lParam = NULL
	MSG_DROP_FILES_ID, // When user drag files to MainFrm window, send the message to LeftTreeView for opening databases by drag file paths, wParam=NULL , lParam=NULL
	MSG_SHOW_SQL_LOG_PAGE_ID, // Send the msg to show SqlLogPage in the RightAnalysisView::tabView, wParam=NULL , lParam=NULL
	MSG_ANALYSIS_ADD_PERF_REPORT_ID, // Send the msg to show AddSqlDialog in the RightAnalysisView, wParam=NULL , lParam=NULL
	MSG_SHOW_PERF_ANALYSIS_PAGE_ID, // Send the msg to show PerAnalysisPage in the RightAnalysisView::tabView, wParam=userDbId , lParam=sqlLogId
	MSG_HOME_REFRESH_DATABASE_ID, // Send this msg when created a table or altering a table , wParam = NULL, lParam=NULL
	MSG_ANALYSIS_CREATE_INDEX_ID, // Send this msg to parent when clicking WhereOrderClauseAnalysisElem::createIdxButton , wParam = WhereOrderClauseAnalysisElem::this, lParam =NULL
	MSG_CHANGE_LANGUAGE_ID, // Send this msg when select item in SettingPanel.GeneralSettingView.changeLanguageComboBox,

	MSG_ANALYSIS_SQL_ID, // When the tree item(iImage=5/6) has double clicked in the LeftNavigation, send this msg to RightAnalysisView for open PerfAnalysisPage, wParam=userDbId, lParam = sqlLogId
	MSG_ANALYSIS_SAVE_PERF_REPORT_ID, // When data has be changed in PerfAnalysisPage, send this msg to parent tabView to display dirty icon for QueryPage title,  wParam=resultTablePage.m_hWnd, lParam=sqlLogId
	MSG_ANALYSIS_DROP_PERF_REPORT_ID, // When drop report menu has clicked in the PerfAnalysisPage, send this msg to LeftNavigationView and RightAnalysisView, wParam=userDbId, lParam = sqlLogId
	MSG_ANALYSIS_DIRTY_DB_PRAGMAS_ID, // When data has be changed in DbPragmaParamsPage, send this msg to LeftNavigationView and RightAnalysisView, wParam=userDbId,lParam=isDirty
	MSG_ANALYSIS_SAVE_DB_PRAGMAS_ID, // When data has be changed in DbPragmaParamsPage, send this msg to LeftNavigationView and RightAnalysisView, wParam=userDbId,lParam=isDirty
	MSG_ANALYSIS_DIRTY_DB_QUICK_CONFIG_ID, // When data has be changed in DbQuickConfigPage, send this msg to LeftNavigationView and RightAnalysisView, wParam=userDbId,lParam=isDirty
	MSG_ANALYSIS_SAVE_DB_QUICK_CONFIG_ID, // When data has be changed in DbQuickConfigPage, send this msg to LeftNavigationView and RightAnalysisView, wParam=userDbId,lParam=isDirty
	MSG_DB_STORE_ANALYSIS_ID,  // When the tree item(iImage=7) has double clicked in the LeftNavigation, send this msg to RightAnalysisView for open StoreAnalysisPage, wParam=userDbId, lParam = NULL
	MSG_DB_PRAGMA_PARAMS_ID,  // When the tree item(iImage=9) has double clicked in the LeftNavigation, send this msg to RightAnalysisView for open DbPragmaParamsPage, wParam=userDbId, lParam = NULL
	MSG_DB_QUICK_CONFIG_PARAMS_ID,  // When the tree item(iImage=10) has double clicked in the LeftNavigation, send this msg to RightAnalysisView for open DbQuickConfigParamsPage, wParam=userDbId, lParam = NULL
	MSG_QPARAMELEM_VAL_CHANGE_ID, // When the QParamElem value has change, send this msg to parent window for setting data dirty. wParam=QParamElem.m_hWnd, lParam=NULL
	
}MessageId;

typedef enum {	
	// SETTING
	SETTING_GENERAL_SETTINGS_MENU_ID = WM_USER + 800,
	SETTING_ABOUT_MENU_ID
} ListMenuItemId;

typedef enum {
	// SETTING VIEW ID
	SETTING_GENERAL_SETTINGS_VIEW_ID = WM_USER + 830,
	SETTING_ABOUT_VIEW_ID,
} ViewId;

typedef enum {
	RESULT_STATUSBAR_SQL_PANE_ID = WM_USER + 860,
	RESULT_STATUSBAR_DATABASE_PANE_ID ,
	RESULT_STATUSBAR_ROWS_PANE_ID,
	RESULT_STATUSBAR_EXEC_TIME_PANE_ID,
}StatusBarPaneId;

typedef enum {
	QPARAMELEM_EDIT_ELEM_ID = WM_USER + 865,
	QPARAMELEM_READ_ELEM_ID ,
	QPARAMELEM_COMBO_EDIT_ID ,
	QPARAMELEM_COMBO_READ_ID 
} QParamElemId;

typedef enum {
	QIMAGE_LIST_ID_START = WM_USER + 1024,
	QIMAGE_LIST_ID_END = WM_USER + 1099,
} ImageIdRank;

typedef enum {
	FILTER_OP_BUTTON_ID_START = WM_USER + 1124,
	FILTER_OP_BUTTON_ID_END = WM_USER + 1199,
} ButtonIdRank;

typedef enum {
	FORMVIEW_EDIT_ID_START = WM_USER + 1200,
	FORMVIEW_EDIT_ID_END = WM_USER + 1712,
} EditIdRank;

typedef enum {
	PRAGMAS_MENU_ID_START = WM_USER + 1713,
	PRAGMAS_MENU_ID_END = WM_USER + 1900,
} MenuIdRank;

typedef enum {
	DB_LIST_ITEM_ID_START = WM_USER + 1901,
	DB_LIST_ITEM_ID_END = WM_USER + 2900,
} DbListItemIdRank;

typedef enum {
	ANALYSIS_INDEX_COLUMN_CHECKBOX_ID_START = WM_USER + 2901,
	ANALYSIS_INDEX_COLUMN_CHECKBOX_ID_END = WM_USER + 3900,
} AnalysisIndexColumnCheckboxIdRank;

typedef enum {
	ANALYSIS_SELECT_COLUMN_CHECKBOX_ID_START = WM_USER + 3901,
	ANALYSIS_SELECT_COLUMN_CHECKBOX_ID_END = WM_USER + 4900,
} AnalysissSelectColumnCheckboxIdRank;


};
