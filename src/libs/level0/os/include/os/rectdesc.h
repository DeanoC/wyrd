#pragma once
#ifndef WYRD_OS_RECTDESC_H
#define WYRD_OS_RECTDESC_H

typedef struct Os_RectDesc_t
{
	int left;
	int top;
	int right;
	int bottom;
} Os_RectDesc_t;

#if defined(__cplusplus)
inline int getRectWidth(const Os_RectDesc_t& rect)
{
	return rect.right - rect.left;
}

inline int getRectHeight(const Os_RectDesc_t& rect)
{
	return rect.bottom - rect.top;
}
#endif

#endif //WYRD_OS_RECTDESC_H
