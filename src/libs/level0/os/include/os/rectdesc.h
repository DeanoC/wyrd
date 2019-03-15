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

EXTERN_C inline int Os_GetRectWidth(const Os_RectDesc_t* rect)
{
	return rect->right - rect->left;
}

EXTERN_C inline int Os_GetRectHeight(const Os_RectDesc_t* rect)
{
	return rect->bottom - rect->top;
}

#endif //WYRD_OS_RECTDESC_H
