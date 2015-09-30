////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>
#include <pdh.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize();
	void Shutdown();
	void Render();

	float GetTime();
	int GetFps();
	int GetCpuPercentage();

	bool SetFps(int);
	bool SetCpu(int);
	bool SetRenderCount(int);

private:
	// Timer
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;

private:
	// Fps
	int m_fps, m_count;
	unsigned long m_startTimeFps;

private:
	// Cpu
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

#endif