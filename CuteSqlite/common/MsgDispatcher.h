/*****************************************************************//**
 * @file   MsgDispatcher.h
 * @brief  ��Ϣ������
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-04-02
 *********************************************************************/
#pragma once
#include <list>
#include <unordered_map>

typedef std::list<HWND> HWNDLIST; // ������Ϣ�Ĵ����б�

class MsgDispatcher
{
public:
	MsgDispatcher();
	~MsgDispatcher();

	// ��Ϣ�ַ�(�޷���)
	void dispatch(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);
	// ��Ϣ�ַ�(�з���)
	LRESULT dispatchForResponse(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);

	// ��Ϣ����
	void subscribe(HWND hwnd, UINT msgId);
	void unsuscribe(HWND hwnd, UINT msgId);
	void unsuscribeAll(HWND hwnd);
private:
	/**
	 * ������Ϣ��MAP,
	 * @param UINT msgId ��Ϣ��ID
	 * @param HWNDLIST hwnd list ����msgId��Ϣ�Ĵ����б�
	 */
	std::unordered_map<UINT, HWNDLIST> msgMap; 
};
