/*****************************************************************//**
 * @file   QImageList.h
 * @brief  ͼƬ�б�ؼ�,���ݴ��ڴ�С����СͼƬ��ʾ������
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-03-19
 *********************************************************************/
#pragma once
#include <list>
#include <vector>
#include <atlwin.h>
#include <atlcrack.h>
#include "QStaticImage.h"
#include "ui/common/button/QImageButton.h"

typedef std::vector<std::wstring> ImagePathList, TipList; // ͼƬ��·���б�

typedef std::vector<QStaticImage *> ImagePtrList; // ͼƬ�ؼ���ָ������ (�����Ϊÿҳ�ļ�¼��)

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

	// ����ͼƬ·���б�
	void load(ImagePathList &imgPathList, SIZE & imgSize, bool isRefresh= false); 
	void load(ImagePathList &imgPathList, TipList &tips, SIZE & imgSize, bool isRefresh); //֧��������ʾ
	// ��ҳ����
	int prevPage();
	int nextPage();

	int getPageCount(); //  �����ҳ��
	int getPerpage(); // ���ÿҳ����
	int getCurPage(); // ��õ�ǰ�ڼ�ҳ����1��ʼ

	int getSelItem(); // ���ѡ�����0��ʼ
	void setSelItem(int index); // ����ѡ�����0��ʼ

	int getTotal(); //���ͼƬ����

	// �Ƿ�����ҳ
	void setIsAllowPage(bool isAllow);

	//=======================================
	void setBgkColor(COLORREF color);
	void setSelItemBoder(int size, COLORREF color = RGB(52, 139, 204)); //����ѡ����ı߿�

	bool contain(HWND hwnd); //�Ƿ�
private:
	ImagePathList imagePathList;
	TipList tipList;
	SIZE imageSize = {0, 0};
	ImagePtrList imagePtrs;
	
	int pageCount = 0; // ��ҳ��
	int curPage = 1;   // ��ǰ�ڼ�ҳ����1��ʼ
	int perpage = 0; // ÿҳ����

	int total = 0; // ͼƬ����
	int nSelItem = -1; // ��ǰѡ����,��0��ʼ

	int selBorderSize = 0; // ѡ����߿�Ĵ�С
	int selborderColor = 0; // ѡ����߿�Ĵ�С
	
	bool isNeedReload = true;
	bool isAllowPage = false;

	COLORREF bkgColor = RGB(255, 255, 255); // ������ɫ
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
