/*****************************************************************//**
 * @file   AppContext.h
 * @brief  ȫ��������
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-03-14
 *********************************************************************/
#pragma once
#include <string>
#include <unordered_map>
#include "MsgDispatcher.h"
class AppContext
{
public:
	static AppContext * getInstance();

	// mainFrm�Ĵ��ھ��
	void setMainFrmHwnd(HWND hwnd);
	HWND getMainFrmHwnd();

	std::wstring getMainFrmCaption();
	void appendMainFrmCaption(const std::wstring & text);

	// HomeView�Ĵ��ھ��
	void setHomeViewHwnd(HWND hwnd) { homeViewHwnd = hwnd; };
	HWND getHomeViewHwnd() { return homeViewHwnd; };

	std::wstring get(std::wstring k);
	int getInt(std::wstring k);
	void set(std::wstring k, std::wstring v);
	void set(std::wstring k, int v);

	// ��Ϣ�ַ�
	void dispatch(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);

	// ��Ϣ����
	void subscribe(HWND hwnd, UINT msgId);
	void unsuscribe(HWND hwnd, UINT msgId);
	void unsuscribeAll(HWND hwnd);
private:
	AppContext();
	~AppContext();

	
	//singleton
	static AppContext * theInstance; 

	// mainFrm�Ĵ��ھ��
	HWND mainFrmHwnd = nullptr;
	// HomeView�Ĵ��ھ��
	HWND homeViewHwnd = nullptr;
	
	// ȫ�ֵ�����
	std::unordered_map<std::wstring, std::wstring> settings;

	// ȫ�ֵ���Ϣ������
	MsgDispatcher msgDispatcher;// �ƹ�ȫ����Ϣ�ķַ��Ͷ���
};

