/*****************************************************************//**
 * @file   AppContext.h
 * @brief  全局上下文
 * @detail    $DETAIL
 * 
 * @author 覃学涵
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

	// mainFrm的窗口句柄
	void setMainFrmHwnd(HWND hwnd);
	HWND getMainFrmHwnd();

	std::wstring getMainFrmCaption();
	void appendMainFrmCaption(const std::wstring & text);

	// HomeView的窗口句柄
	void setHomeViewHwnd(HWND hwnd) { homeViewHwnd = hwnd; };
	HWND getHomeViewHwnd() { return homeViewHwnd; };

	std::wstring get(std::wstring k);
	int getInt(std::wstring k);
	void set(std::wstring k, std::wstring v);
	void set(std::wstring k, int v);

	// 消息分发
	void dispatch(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);

	// 消息订阅
	void subscribe(HWND hwnd, UINT msgId);
	void unsuscribe(HWND hwnd, UINT msgId);
	void unsuscribeAll(HWND hwnd);
private:
	AppContext();
	~AppContext();

	
	//singleton
	static AppContext * theInstance; 

	// mainFrm的窗口句柄
	HWND mainFrmHwnd = nullptr;
	// HomeView的窗口句柄
	HWND homeViewHwnd = nullptr;
	
	// 全局的配置
	std::unordered_map<std::wstring, std::wstring> settings;

	// 全局的消息调度器
	MsgDispatcher msgDispatcher;// 掌管全局消息的分发和订阅
};

