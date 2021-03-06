/********************************************************************
	created:	2017/09/05
	created:	5:9:2017   9:26
	file base:	Timer
	file ext:	h
	author:		LYG
	
	purpose:	
*********************************************************************/
#ifndef		__TIMER_H__
#define		__TIMER_H__
#include <windows.h>
#include <time.h>
#include <xstring>
#include <vector>
#include "GameDef.h"

struct ITimerListener
{
	virtual void OnTimer(unsigned int EventID, void* UserData) = 0;
};

struct TimerObject
{
	ITimerListener* Listener;
	unsigned int EventID;
	unsigned int Delta;
	unsigned int RemainTime;
	void* UserData;
	TimerObject()
	{
		memset(this, 0, sizeof(TimerObject));
	}
};

class Timer
{
	friend class GameLogicManager;
protected:
	Timer();
	~Timer();

	static Timer* ThisInstance;
public:
	// ��ʼ��
	bool Initialise();
	static Timer* GetInstance();
	// ����
	bool Update();
	unsigned long GetDelta() const;
	unsigned long GetFrameIndex() const;
	float GetDeltaFloat() const;
	/** Returns milliseconds since initialisation or last reset */
	unsigned long getMilliseconds();
	/** Returns microseconds since initialisation or last reset */
	unsigned long getMicroseconds();
	void AddTimer(ITimerListener* Listener, unsigned int EventID, unsigned int Delta, void* UserData = nullptr);
	void RemoveTimer(ITimerListener* Listener, unsigned int EventID);
	// Add a once callback timer, which been auto remove.
	void AddOnceTimer(ITimerListener* Listener, unsigned int EventID, unsigned int Delta, void* UserData = nullptr);
private:
	bool setOption(const std::string& strKey, const void* pValue);
	/** Resets timer */
	void reset();
	/** Returns milliseconds since initialisation or last reset, only CPU time measured */
	unsigned long getMillisecondsCPU();
	/** Returns microseconds since initialisation or last reset, only CPU time measured */
	unsigned long getMicrosecondsCPU();
	void SetPauseTimer(BOOL Pause);
	BOOL GetPauseTimer() const;
	DWORD GetTickCount() { return (DWORD)GetTickCount64(); }

	clock_t mZeroClock;
	DWORD mStartTick;
	LONGLONG mLastTime;
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
	unsigned long mDelta;
	unsigned long mFrameIndex;
	BOOL mPauseTimer;
	DWORD_PTR mTimerMask;
	std::vector<TimerObject*> mTimerEventList;
	std::vector<TimerObject*> mTimerEventOnceList;
	CLEAR_VECTOR2(mTimerEventList, mTimerEventOnceList);
};

#endif // !__TIMER_H__
