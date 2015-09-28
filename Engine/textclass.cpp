///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{	
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize()
{
	bool result;
	PDH_STATUS status;
	// TIMER

	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	// FPS	
		
	// Initialize the counters and the start time.
	m_fps = 0;
	m_count = 0;
	m_startTimeFps = timeGetTime();	

	// CPU
	

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	m_canReadCpu = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// Initialize the start time and cpu usage.
	m_lastSampleTime = GetTickCount();
	m_cpuUsage = 0;

	return true;
}


void TextClass::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}

	return;
}


void TextClass::Render()
{
	INT64 currentTime;
	float timeDifference;

	PDH_FMT_COUNTERVALUE value;

	// Set timer


	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;


	// Set fps
	m_count++;

	// If one second has passed then update the frame per second speed.
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}

	// Set cpu


	if (m_canReadCpu)
	{
		// If it has been 1 second then update the current cpu usage and reset the 1 second timer again.
		if ((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}

	
	return;
}

float TextClass::GetTime()
{
	return true;
}

int TextClass::GetFps()
{
	return m_fps;
}


bool TextClass::SetFps(int fps)
{
	char tempString[16];
	char fpsString[16];
	bool result;




	// Truncate the fps to below 10,000.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcat_s(fpsString, tempString);

	
	return true;
}

int TextClass::GetCpuPercentage()
{
	int usage;
	

	// If the class can read the cpu from the operating system then return the current usage.  If not then return zero.
	if (m_canReadCpu)
	{
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}



bool TextClass::SetCpu(int cpu)
{
	char tempString[16];
	char cpuString[16];
	bool result;


	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcat_s(cpuString, tempString);


	return true;
}



bool TextClass::SetRenderCount(int count)
{
	char tempString[16];
	char renderString[32];
	bool result;


	// Truncate the render count if it gets to large to prevent a buffer overflow.
	if (count > 999999999)
	{
		count = 999999999;
	}

	// Convert the cpu integer to string format.
	_itoa_s(count, tempString, 10);

	// Setup the cpu string.
	strcat_s(renderString, tempString);

	return true;
}

