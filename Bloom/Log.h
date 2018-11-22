/***********************************************
 * File: Log.h
 *
 * Author: LYG
 * Date: Ê®ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include <stdio.h>
#include <xstring>
class Scene;
class LogImpl
{
	friend class GameLogicManager;
public:
	virtual void Log(std::string Msg);
	virtual void __cdecl LogFormat(const char* Format, ...);

	static LogImpl* GetInstance();
protected:
	LogImpl(const char* FilePath);
	~LogImpl();

	static LogImpl* ThisInstance;

	FILE* mLogFile;
};