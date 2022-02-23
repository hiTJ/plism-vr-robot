/*
 * @file    Common.h
 * @brief	マクロや定数の定義
 * @date	2011-2
 * @autor   Brays
 */

#ifndef _Common_H_
#define _Common_H_

// 参照カウンタのデクリメント
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#endif // _Common_H_