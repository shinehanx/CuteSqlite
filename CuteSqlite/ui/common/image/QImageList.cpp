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
 * ���캯��.
 * 
 * @param imgPathList ͼƬ·��
 * @param imageSize ͼƬ�Ŀ��
 * @param tips ͼƬ�Ϸ���ʾ������
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
	total = static_cast<int>(imagePathList.size()); // ����
	isNeedReload = true;

	if (isRefresh) {
		loadWindow();
	}
}

/**
 * ��ǿ��ļ��أ�������tips����.
 * 
 * @param imgPathList ͼƬ�б�
 * @param tips ͼƬ����ʾ�������б��±���imgPathList����һ��
 * @param imgSize ÿ��ͼƬ�Ĵ�С
 * @param isRefresh �Ƿ�ˢ��
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
	isNeedReload = true; // ���¼���
	loadWindow(); 
	return curPage;
}

int QImageList::nextPage()
{
	if (curPage >= pageCount) {
		return curPage;
	}
	curPage++;
	isNeedReload = true; // ���¼���
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
 * ����ѡ����߿��С����ɫ.
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
 * �ж��Ƿ����.
 * 
 * @param hwnd ͼƬ���ھ��
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

	// ��ʾͼƬ
	createOrShowImages(0, clientRect);

	// ��ʾ���·�ҳ��ť
	createOrShowButtons(clientRect);
}

/**
 * ָ����ǰҳ.
 * 
 * @param _curPage
 * @param clientRect
 */
void QImageList::createOrShowImages(int _curPage, CRect & clientRect)
{
	// ��ʼ��ÿһ��ͼƬ��λ��
	auto imgRects = setupImagePageRects(clientRect, imageSize);
	curPage = _curPage> 0 ? _curPage : getCurPageBySelIndex(total, perpage, nSelItem);

	clearImagePtrs();
	int j = 0; // ѡȡͼ��λ��
	for (int i = (curPage - 1) * perpage; (i < curPage * perpage) && (i < total); i++) {
		std::wstring imagePath = imagePathList.at(i); // ѡȡͼ��λ��
		CRect rect = imgRects[j];
		std::wstring fileExt = FileUtil::getFileExt(imagePath);
		DWORD compress = BI_RGB;
		if (fileExt == L"png") {
			compress = BI_PNG;
		} else if (fileExt == L"jpg" || fileExt == L"jpeg") {
			compress = BI_JPEG;
		}
		QStaticImage * imagePtr = new QStaticImage(imagePath.c_str(), compress, true);
		imagePtr->setDisplayMode(QImage::kImageStretchFitTargetRect);// ��ʾģʽ
		if (!tipList.empty()) {
			imagePtr->setTip(tipList.at(i).c_str(), tipFont);
		}
		if (i == nSelItem) {
			imagePtr->setBorder(selBorderSize, selborderColor); // ѡ����ӱ߿�
		}
		imagePtr->setBkgColor(bkgColor);
		UINT nID = Config::QIMAGE_LIST_ID_START + i;
		QWinCreater::createOrShowImage(m_hWnd, imagePtr, nID,  rect,  clientRect);
		
		imagePtrs.push_back(imagePtr);
		j++;
	}
}

/**
 * ��ҳ��ť.
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
	QWinCreater::createOrShowButton(m_hWnd, prevPageButton, Config::QIMAGE_PREV_PAGE_BUTTON_ID, L"��", rect, clientRect);

	x = clientRect.right - PAGE_BUTTON_WIDTH;
	rect = { x, y, x + w, y + h };
	nextPageButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, nextPageButton, Config::QIMAGE_NEXT_PAGE_BUTTON_ID, L"��", rect, clientRect);
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
 * �����б��е�ͼ�꣬��Ӧ���¼�.
 *   wParam���ϱ�����Ĵ���ID lParam���ϴ��ھ��hwnd
 * 
 * @param uMsg
 * @param wParam �����ͼƬ�Ĵ���ID
 * @param lParam �����ͼƬ���ھ��hwnd
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
			ptr->setBorder(selBorderSize, selborderColor); // �ӱ߿� 
			ptr->Invalidate(TRUE);
			nSelItem = i;
			hwndSelItem = ptr->m_hWnd; // ѡ��ͼƬ�Ĵ��ھ��
		} else {
			ptr->setBorder(0, RGB(0, 0, 0)); // ȥ�߿�
			ptr->Invalidate(TRUE);
		}
	}
	
	
	// �����ڷ��͵����Ϣ�� wParamΪ��ѡ�����±�nSelItem��lParam���ϱ�ѡ����Ĵ��ھ��
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
	int perWidth = size.cx + IMAGE_SPLIT_WIDTH; // ÿ��ͼƬ��� + �ո�
	int perHeight = size.cy + IMAGE_SPLIT_HEIGHT; // ÿ��ͼƬ�߶� + �ո�
	
	int nCol = (clientRect.Width() - IMAGE_SPLIT_WIDTH )  / perWidth;  // ����,�������ҷ�ҳ��ť�ľ���
	int nRow = (clientRect.Height() - IMAGE_SPLIT_HEIGHT) / perHeight; // ����

	// �������ҳ
	if (isAllowPage) {
		nCol = (clientRect.Width() - IMAGE_SPLIT_WIDTH - PAGE_BUTTON_WIDTH )  / perWidth;  // ����,�������ҷ�ҳ��ť�ľ���
	}

	// ����ÿҳ������
	perpage = nCol * nRow;
	if (perpage == 0) {
		return result;
	}

	// ������ҳ��
	pageCount = total % perpage == 0 ? total / perpage : total / perpage + 1; 

	// ���ҳ������1�����������ҵľ���
	int left = isAllowPage && pageCount > 1 ? PAGE_BUTTON_WIDTH + 5 : 0;
	
	int x = 0, y = 0, w = size.cx, h = size.cy;
	for (int i = 0; i < nRow; i++) {
		x = left; // ����һ�У�x����,���ó�left
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
 * ���㵱ǰҳ.
 * 
 * @param total ����
 * @param perpage ÿҳ����
 * @param nSelItem ѡ����
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
