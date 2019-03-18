#pragma once
#ifndef WYRD_OS_MONITORDESC_H
#define WYRD_OS_MONITORDESC_H

#include "os/rect.h"

typedef struct Resolution
{
	uint32_t mWidth;
	uint32_t mHeight;
} Resolution;

// Monitor data
//
typedef struct MonitorDesc
{
	RectDesc monitorRect;
	RectDesc workRect;
	// This size matches the static size of DISPLAY_DEVICE.DeviceName
#ifdef _WIN32
	WCHAR adapterName[32];
	WCHAR displayName[32];
	WCHAR publicAdapterName[64];
	WCHAR publicDisplayName[64];
#else
	char adapterName[32];
	char displayName[32];
	char publicAdapterName[64];
	char publicDisplayName[64];
#endif
	bool modesPruned;
	bool modeChanged;

	Resolution defaultResolution;
	Resolution* resolutions;
	uint32_t resolutionCount;
} MonitorDesc;

void getRecommendedResolution(RectDesc* rect);
// Sets video mode for specified display
void setResolution(const MonitorDesc* pMonitor, const Resolution* pRes);

MonitorDesc* getMonitor(uint32_t index);
vec2 getDpiScale();

bool getResolutionSupport(const MonitorDesc* pMonitor, const Resolution* pRes);

#endif