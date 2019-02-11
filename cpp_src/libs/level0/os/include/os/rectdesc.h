#pragma once
#ifndef WYRD_OS_RECTDESC_H
#define WYRD_OS_RECTDESC_H

typedef struct RectDesc
{
	int left;
	int top;
	int right;
	int bottom;
} RectDesc;

#if defined(__cplusplus)
inline int getRectWidth(const RectDesc& rect)
{
	return rect.right - rect.left;
}

inline int getRectHeight(const RectDesc& rect)
{
	return rect.bottom - rect.top;
}
#endif

#endif //WYRD_OS_RECTDESC_H
