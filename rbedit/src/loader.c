/*
    Copyright (c) 2021-2022 jdeokkim

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "rbedit.h"
#include "raygui.h"

/* | `loader` 모듈 매크로 정의... | */

#define GUI_STYLE_PROP_COUNT  14

/* | `loader` 모듈 상수... | */

const GuiStyleProp GUI_STYLE_PROPS[GUI_STYLE_PROP_COUNT] = {
    { 0,  0, 0x343333ff },    // DEFAULT_BORDER_COLOR_NORMAL 
    { 0,  1, 0x343333ff },    // DEFAULT_BASE_COLOR_NORMAL 
    { 0,  2, 0xb6b6b6ff },    // DEFAULT_TEXT_COLOR_NORMAL 
    { 0,  3, 0x676666ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
    { 0,  4, 0x2c2c2cff },    // DEFAULT_BASE_COLOR_FOCUSED 
    { 0,  5, 0x919191ff },    // DEFAULT_TEXT_COLOR_FOCUSED 
    { 0,  6, 0x9a9999ff },    // DEFAULT_BORDER_COLOR_PRESSED 
    { 0,  7, 0x494949ff },    // DEFAULT_BASE_COLOR_PRESSED 
    { 0,  8, 0xa8a8a8ff },    // DEFAULT_TEXT_COLOR_PRESSED 
    { 0,  9, 0x363636ff },    // DEFAULT_BORDER_COLOR_DISABLED 
    { 0, 10, 0x0f0f0fff },    // DEFAULT_BASE_COLOR_DISABLED 
    { 0, 11, 0x575757ff },    // DEFAULT_TEXT_COLOR_DISABLED 
    { 0, 18, 0x545454ff },    // DEFAULT_LINE_COLOR 
    { 0, 19, 0x212121ff },    // DEFAULT_BACKGROUND_COLOR
};

/* | `loader` 모듈 함수... | */

/* 리소스 파일을 불러온다. */
void LoadResources(void) {
    SetExitKey(KEY_NULL);

    for (int i = 0; i < GUI_STYLE_PROP_COUNT; i++) {
        GuiSetStyle(
            GUI_STYLE_PROPS[i].controlId, 
            GUI_STYLE_PROPS[i].propertyId, 
            GUI_STYLE_PROPS[i].propertyValue
        );
    }
    
    InitPhysicsWorld();
    InitCurrentScreen();
}

/* 리소스 파일에 할당된 메모리를 해제한다. */
void UnloadResources(void) {
    UninitCurrentScreen();
    UninitPhysicsWorld();
}