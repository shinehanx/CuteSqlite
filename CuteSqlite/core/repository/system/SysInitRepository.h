/*****************************************************************//**
 * @file   SysInitRepository.h
 * @brief  系统配置加载的类
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-02
 *********************************************************************/
#pragma once
#include "core/entity/Entity.h"
#include "core/common/repository/BaseRepository.h"

class SysInitRepository : public BaseRepository<SysInitRepository>
{
public:
	SysInitRepository() {};

	std::wstring get(const std::wstring & name);
	void set(const std::wstring & name, const std::wstring & val);
	bool has(const std::wstring & name);
	
	SysInitList getAll();

	
private:	
	~SysInitRepository() {};

};

