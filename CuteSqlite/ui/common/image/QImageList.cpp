#include "stdafx.h"
#include "QImageList.h"
#include <vector>
#include <algorithm>
#include <atlgdi.h>
#include "ui/common/QWinCreater.h"
#include "utils/FileUtil.h"

#define IMAGE_SPLIT_WIDTH 10
#define IMAGE_SPLIT_HEIGHT 10
#define PAGE_BUTTON_WIDTH 25
QImageList::QImageList()
{

}

/**
 * 构造函数.
 * 
 * @param imgPathList 图片路径
 * @param imageSize 图片的宽高
 * @param tips 图片上方显示的文字
 */
QImageList::QImageList(ImagePathList &imgPathList, SIZE & imgSize, TipList & tips) : 
	imagePathList(imgPathList) , imageSize(imgSize), tipList(tips)
{
	total = static_cast<int>(imagePathList.size());
}


QImageList::~QImageList()
{

}

void QImageList::load(ImagePathList &imgPathList, SIZE & imgSize, bool isRefresh)
{
	imagePathList = imgPathList;
	imageSize.cx = imgSize.cx;
	imageSize.cy = imgSize.cy;
	total = static_cast<int>(imagePathList.size()); // 总数
	isNeedReload = true;

	if (isRefresh) {
		loadWindow();
	}
}

/**
 * 增强版的加载，增加了tips参数.
 * 
 * @param imgPathList 图片列表
 * @param tips 图片上显示的文字列表，下标与imgPathList保持一致
 * @param imgSize 每张图片的大小
 * @param isRefresh 是否刷新
 */
void QImageList::load(ImagePathList &imgPathList,TipList &tips, SIZE & imgSize, bool isRefresh)
{
	tipList = tips;
	load(imgPathList, imgSize, isRefresh);	
}

int QImageList::prevPage()
{
	if (curPage <= 1) {
		return curPage;
	}
	curPage--;
	isNeedReload = true; // 重新加载
	loadWindow(); 
	return curPage;
}

int QImageList::nextPage()
{
	if (curPage >= pageCount) {
		return curPage;
	}
	curPage++;
	isNeedReload = true; // 重新加载
	loadWindow(); 
	return curPage;
}

int QImageList::getPageCount()
{
	return pageCount;
}

int QImageList::getPerpage()
{
	return perpage;
}

int QImageList::getCurPage()
{
	return curPage;
}

int QImageList::getSelItem()
{
	return nSelItem;
}

void QImageList::setSelItem(int index)
{
	if (index >= total) {
		return ;
	}
	nSelItem = index;

}

int QImageList::getTotal()
{
	return total;
}

void QImageList::setIsAllowPage(bool isAllow)
{
	this->isAllowPage = isAllow;
}

void QImageList::setBgkColor(COLORREF color)
{
	bkgColor = color;
	if (bkgBrush) DeleteObject(bkgBrush);
	bkgBrush = ::CreateSolidBrush(bkgColor);
}

/**
 * 设置选中项边框大小和颜色.
 * 
 * @param size
 * @param color
 */
void QImageList::setSelItemBoder(int size, COLORREF color /*= RGB(52, 139, 204)*/)
{
	selBorderSize = size;
	selborderColor = color;
}

/**
 * 判断是否存在.
 * 
 * @param hwnd 图片窗口句柄
 * @return 
 */
bool QImageList::contain(HWND hwnd)
{
	auto item = std::find_if(imagePtrs.begin(), imagePtrs.end(), [&hwnd](QStaticImage * image) {
		return image->m_hWnd == hwnd;
	});

	if (item == imagePtrs.end()) {
		return false;
	}
	return true;
}

void QImageList::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	// 显示图片
	createOrShowImages(0, clientRect);

	// 显示上下翻页按钮
	createOrShowButtons(clientRect);
}

/**
 * 指定当前页.
 * 
 * @param _curPage
 * @param clientRect
 */
void QImageList::createOrShowImages(int _curPage, CRect & clientRect)
{
	// 初始化每一个图片的位置
	auto imgRects = setupImagePageRects(clientRect, imageSize);
	curPage = _curPage> 0 ? _curPage : getCurPageBySelIndex(total, perpage, nSelItem);

	clearImagePtrs();
	int j = 0; // 选取图形位置
	for (int i = (curPage - 1) * perpage; (i < curPage * perpage) && (i < total); i++) {
		std::wstring imagePath = imagePathList.at(i); // 选取图形位置
		CRect rect = imgRects[j];
		std::wstring fileExt = FileUtil::getFileExt(imagePath);
		DWORD compress = BI_RGB;
		if (fileExt == L"png") {
			compress = BI_PNG;
		} else if (fileExt == L"jpg" || fileExt == L"jpeg") {
			compress = BI_JPEG;
		}
		QStaticImage * imagePtr = new QStaticImage(imagePath.c_str(), compress, true);
		imagePtr->setDisplayMode(QImage::kImageStretchFitTargetRect);// 显示模式
		if (!tipList.empty()) {
			imagePtr->setTip(tipList.at(i).c_str(), tipFont);
		}
		if (i == nSelItem) {
			imagePtr->setBorder(selBorderSize, selborderColor); // 选中项加边框
		}
		imagePtr->setBkgColor(bkgColor);
		UINT nID = Config::QIMAGE_LIST_ID_START + i;
		QWinCreater::createOrShowImage(m_hWnd, imagePtr, nID,  rect,  clientRect);
		
		imagePtrs.push_back(imagePtr);
		j++;
	}
}

/**
 * 翻页按钮.
 * 
 * @param clientRect
 */
void QImageList::createOrShowButtons(CRect & clientRect)
{
	if (!isAllowPage || pageCount <= 1) {
		return ;
	}

	int x = 0, y = 10, w = PAGE_BUTTON_WIDTH, h = clientRect.Height() - 20;
	CRect rect(x, y, x + w, y + h);
	prevPageButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, prevPageButton, Config::QIMAGE_PREV_PAGE_BUTTON_ID, L"《", rect, clientRect);

	x = clientRect.right - PAGE_BUTTON_WIDTH;
	rect = { x, y, x + w, y + h };
	nextPageButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, nextPageButton, Config::QIMAGE_NEXT_PAGE_BUTTON_ID, L"》", rect, clientRect);
}

void QImageList::loadWindow()
{
	if (isNeedReload) {
		isNeedReload = false; 

		CRect clientRect;
		GetClientRect(clientRect);
		createOrShowImages(curPage,clientRect);
	}
}

LRESULT QImageList::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush = (HBRUSH) ::CreateSolidBrush(bkgColor);
	tipFont = FontUtil::getFont(18);
	if (imageSize.cx == 0 || imageSize.cy == 0) {
		imageSize = { QIMAGE_LIST_ITEM_WIDTH, QIMAGE_LIST_ITEM_HEIGHT };
	}
	
	return 0;
}

LRESULT QImageList::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (tipFont) ::DeleteObject(tipFont);

	if (prevPageButton.IsWindow()) prevPageButton.DestroyWindow();
	if (nextPageButton.IsWindow()) nextPageButton.DestroyWindow();
	clearImagePtrs();
	return 0;
}

LRESULT QImageList::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT QImageList::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!wParam) {
		return 0;
	}
	loadWindow();
	return 0;
}

LRESULT QImageList::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);
	dc.FillRect(&(dc.m_ps.rcPaint), bkgBrush);

	return 0;
}

LRESULT QImageList::OnClickPrevPageButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CRect clientRect;
	GetClientRect(clientRect);
	curPage = curPage >= 2 ? curPage - 1 : 1;
	createOrShowImages(curPage, clientRect);
	return 0;
}

LRESULT QImageList::OnClickNextPageButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CRect clientRect;
	GetClientRect(clientRect);
	curPage = curPage < pageCount ? curPage + 1 : pageCount;
	createOrShowImages(curPage, clientRect);
	return 0;
}

/**
 * 单击列表中的图标，响应的事件.
 *   wParam带上被点击的窗口ID lParam带上窗口句柄hwnd
 * 
 * @param uMsg
 * @param wParam 被点击图片的窗口ID
 * @param lParam 被点击图片窗口句柄hwnd
 * @param bHandled
 * @return 
 */
LRESULT QImageList::OnClickImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwnd = (HWND)lParam;
	
	HWND hwndSelItem = nullptr;
	int n = static_cast<int>(imagePtrs.size());
	for (int i = 0; i < n; i++) {
		QStaticImage * ptr = imagePtrs.at(i);
		if (ptr->m_hWnd == hwnd) {
			ptr->setBorder(selBorderSize, selborderColor); // 加边框 
			ptr->Invalidate(TRUE);
			nSelItem = i;
			hwndSelItem = ptr->m_hWnd; // 选中图片的窗口句柄
		} else {
			ptr->setBorder(0, RGB(0, 0, 0)); // 去边框
			ptr->Invalidate(TRUE);
		}
	}
	
	
	// 父窗口发送点击消息： wParam为被选中项下标nSelItem，lParam带上被选中项的窗口句柄
	::PostMessage(GetParent().m_hWnd, Config::MSG_QIMAGELIST_CLICK_ID, (WPARAM)nSelItem, (LPARAM)hwndSelItem);
	return 0;
}

BOOL QImageList::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

std::vector<CRect> QImageList::setupImagePageRects(CRect & clientRect, CSize size)
{
	std::vector<CRect> result;
	int perWidth = size.cx + IMAGE_SPLIT_WIDTH; // 每张图片宽度 + 空格
	int perHeight = size.cy + IMAGE_SPLIT_HEIGHT; // 每张图片高度 + 空格
	
	int nCol = (clientRect.Width() - IMAGE_SPLIT_WIDTH )  / perWidth;  // 列数,不留左右翻页按钮的距离
	int nRow = (clientRect.Height() - IMAGE_SPLIT_HEIGHT) / perHeight; // 行数

	// 如果允许翻页
	if (isAllowPage) {
		nCol = (clientRect.Width() - IMAGE_SPLIT_WIDTH - PAGE_BUTTON_WIDTH )  / perWidth;  // 列数,留出左右翻页按钮的距离
	}

	// 计算每页的数量
	perpage = nCol * nRow;
	if (perpage == 0) {
		return result;
	}

	// 计算总页数
	pageCount = total % perpage == 0 ? total / perpage : total / perpage + 1; 

	// 如果页数大于1，则留出左右的距离
	int left = isAllowPage && pageCount > 1 ? PAGE_BUTTON_WIDTH + 5 : 0;
	
	int x = 0, y = 0, w = size.cx, h = size.cy;
	for (int i = 0; i < nRow; i++) {
		x = left; // 新起一行，x清零,重置成left
		y += 10;		
		for (int j = 0; j < nCol; j++) {
			x += 10;
			CRect rect(x, y, x+w, y+h);
			result.push_back(rect);
			x += w;
		}
		y += h;
	}
	return result;
}

/**
 * 计算当前页.
 * 
 * @param total 总数
 * @param perpage 每页条数
 * @param nSelItem 选中项
 * @return 
 */
int QImageList::getCurPageBySelIndex(int total, int perpage, int nSelItem)
{
	if (nSelItem < 0 || nSelItem >= total) {
		return 1;
	}
	for (int page = 1; page <= pageCount; page++) {
		int minIdx = (page - 1) * perpage;
		int maxIdx = page * perpage;
		if (nSelItem >= minIdx && nSelItem < maxIdx) {
			return page;
		}
	}
	return 1;
}

void QImageList::clearImagePtrs()
{
	std::for_each(imagePtrs.begin(), imagePtrs.end(), [](QImage * imagePtr) -> void {
		if (imagePtr && imagePtr->IsWindow()) {
			imagePtr->DestroyWindow();
			delete imagePtr;
			imagePtr = nullptr;
		}
	});
	
	imagePtrs.clear();
}
