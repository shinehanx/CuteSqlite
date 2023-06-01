/*****************************************************************//**
 * @file   QImageList.h
 * @brief  图片列表控件,根据窗口大小控制小图片显示的条数
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-19
 *********************************************************************/
#pragma once
#include <list>
#include <vector>
#include <atlwin.h>
#include <atlcrack.h>
#include "QStaticImage.h"
#include "ui/common/button/QImageButton.h"

typedef std::vector<std::wstring> ImagePathList, TipList; // 图片的路径列表

typedef std::vector<QStaticImage *> ImagePtrList; // 图片控件的指针数组 (最大数为每页的记录数)

class QImageList : public CWindowImpl<QImageList> 
{
public:
	BEGIN_MSG_MAP_EX(QImageList)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_HANDLER_EX(Config::QIMAGE_PREV_PAGE_BUTTON_ID, BN_CLICKED, OnClickPrevPageButton)
		COMMAND_HANDLER_EX(Config::QIMAGE_NEXT_PAGE_BUTTON_ID, BN_CLICKED, OnClickNextPageButton)
		MESSAGE_HANDLER(Config::MSG_QIMAGE_CLICK_ID, OnClickImage)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	QImageList();
	QImageList(ImagePathList &imgPathList, SIZE & imgSize , TipList & tips = TipList());
	~QImageList();

	// 加载图片路径列表
	void load(ImagePathList &imgPathList, SIZE & imgSize, bool isRefresh= false); 
	void load(ImagePathList &imgPathList, TipList &tips, SIZE & imgSize, bool isRefresh); //支持文字显示
	// 翻页功能
	int prevPage();
	int nextPage();

	int getPageCount(); //  获得总页数
	int getPerpage(); // 获得每页条数
	int getCurPage(); // 获得当前第几页，从1开始

	int getSelItem(); // 获得选中项，从0开始
	void setSelItem(int index); // 设置选中项，从0开始

	int getTotal(); //获得图片总数

	// 是否允许翻页
	void setIsAllowPage(bool isAllow);

	//=======================================
	void setBgkColor(COLORREF color);
	void setSelItemBoder(int size, COLORREF color = RGB(52, 139, 204)); //设置选中项的边框

	bool contain(HWND hwnd); //是否
private:
	ImagePathList imagePathList;
	TipList tipList;
	SIZE imageSize = {0, 0};
	ImagePtrList imagePtrs;
	
	int pageCount = 0; // 总页数
	int curPage = 1;   // 当前第几页，从1开始
	int perpage = 0; // 每页条数

	int total = 0; // 图片总数
	int nSelItem = -1; // 当前选中项,从0开始

	int selBorderSize = 0; // 选中项边框的大小
	int selborderColor = 0; // 选中项边框的大小
	
	bool isNeedReload = true;
	bool isAllowPage = false;

	COLORREF bkgColor = RGB(255, 255, 255); // 背景颜色
	HBRUSH  bkgBrush = nullptr;
	HFONT tipFont = nullptr;

	QImageButton prevPageButton;
	QImageButton nextPageButton;

	void createOrShowUI();
	void loadWindow();

	void createOrShowImages(int _curPage, CRect & clientRect);
	void createOrShowButtons(CRect & clientRect);
	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickPrevPageButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickNextPageButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL OnEraseBkgnd(CDCHandle dc);

	std::vector<CRect> setupImagePageRects(CRect & clientRect, CSize size);
	int getCurPageBySelIndex(int total, int perpage, int nSelItem);

	void clearImagePtrs();
};
