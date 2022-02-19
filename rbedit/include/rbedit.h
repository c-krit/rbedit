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

#ifndef RBEDIT_H
#define RBEDIT_H

#include "ferox.h"
#include "raylib.h"

/* | 일반 매크로 정의... | */

#define RBEDIT_WINDOW_TITLE           "c-krit/rbedit by @jdeokkim"
#define RBEDIT_REPOSITORY_URL         "https://github.com/c-krit/rbedit"

/* | UI 매크로 정의... | */

#define TARGET_FPS                    60

#define SCREEN_WIDTH                  1024
#define SCREEN_HEIGHT                 768

#define GUI_CANVAS_INITIAL_WIDTH      800
#define GUI_CANVAS_INITIAL_HEIGHT     600
#define GUI_CANVAS_MAXIMUM_WIDTH      4096
#define GUI_CANVAS_MAXIMUM_HEIGHT     4096
#define GUI_CANVAS_ZOOM_COUNT         7

#define GUI_SHAPES_DEFAULT_MATERIAL   ((frMaterial) { 1.0f, 0.01f, 0.75f, 0.5f })
#define GUI_SHAPES_TEXTBOX_MAX_VALUE  999.0f
#define GUI_SHAPES_MAX_VERTEX_COUNT   6

#define GUI_BODIES_TEXTBOX_MAX_VALUE  99.0f

#define GUI_PANEL_WIDTH               256.0f
#define GUI_PANEL_HEIGHT              (SCREEN_HEIGHT)

#define GUI_GROUP_BOX_WIDTH           (GUI_PANEL_WIDTH - 24.0f)
#define GUI_GROUP_BOX_PADDING         16.0f

#define GUI_MAIN_GROUP_BOX_COUNT      3
#define GUI_EDIT_GROUP_BOX_COUNT      3

#define GUI_STATUS_BAR_WIDTH          (SCREEN_WIDTH)
#define GUI_STATUS_BAR_HEIGHT         28.0f

#define GUI_MESSAGE_BOX_WIDTH         300.0f
#define GUI_MESSAGE_BOX_HEIGHT        120.0f

#define GUI_WINDOW_BOX_WIDTH          360.0f
#define GUI_WINDOW_BOX_HEIGHT         160.0f

/* | 자료형 정의... | */

/* 게임 세계의 상태를 나타내는 열거형. */
typedef enum PhysicsState {
    PHYSICS_STATE_PAUSED,
    PHYSICS_STATE_RUNNING,
    PHYSICS_STATE_STEPPING,
    PHYSICS_STATE_RESTARTING
} PhysicsState;

/* 게임 세계의 개체 정보를 나타내는 구조체. */
typedef struct ObjectData {
    unsigned int id;
    Color color;
    bool no_fill;
} ObjectData;

/* | `loader` 모듈 함수... | */

/* 리소스 파일을 불러온다. */
void LoadResources(void);

/* 리소스 파일에 할당된 메모리를 해제한다. */
void UnloadResources(void);

/* | `physics` 모듈 함수... | */

/* 게임 세계를 초기화한다. */
void InitPhysicsWorld(void);

/* 게임 세계에 할당된 메모리를 해제한다. */
void UninitPhysicsWorld(void);

/* 게임 세계에 새로운 개체를 추가한다. */
void AddPhysicsObjectToWorld(frBody *body, ObjectData data);

/* 게임 세계에서 고유 번호가 `id`인 개체를 제거한다. */
void RemovePhysicsObjectFromWorld(unsigned int id);

/* 게임 세계에서 고유 번호가 `id`인 개체의 정보를 업데이트한다. */
void UpdatePhysicsObjectInWorld(unsigned int id, frBody *src);

/* 주어진 개체의 복사본을 생성한다. */ 
frBody *ClonePhysicsObject(frBody *body);

/* 게임 세계에서 좌표 `position`을 포함하는 개체를 반환한다. */
frBody *GetPhysicsObjectAt(Vector2 position);

/* 게임 세계의 메모리 주소를 반환한다. */
frWorld *GetPhysicsWorld(void);

/* 게임 세계의 현재 상태를 반환한다. */
PhysicsState GetPhysicsWorldState(void);

/* 게임 세계의 현재 상태를 나타내는 문자열을 반환한다. */
const char *GetPhysicsWorldStateText(void);

/* 게임 세계의 중력 가속률을 `value`로 설정한다. */
void SetPhysicsGravityMultiplier(float value);

/* 게임 세계의 경계 범위를 `bounds`로 설정한다. */
void SetPhysicsWorldBounds(Rectangle bounds);

/* 게임 세계의 현재 상태를 `value`로 설정한다. */
void SetPhysicsWorldState(PhysicsState value);

/* 게임 세계의 시간을 흐르게 한다. */
void SimulatePhysicsWorld(void);

/* 현재 화면에 게임 세계를 그린다. */
void DrawPhysicsWorld(void);

/* | `screen` 모듈 함수... | */

/* 캔버스 경계 범위를 반환한다. */
Rectangle GetCanvasBounds(void);

/* 캔버스를 기준으로 한 마우스 좌표를 반환한다. */
Vector2 GetCanvasMousePosition(void);

/* 캔버스 기준 좌표 `position`를 화면 기준 좌표로 변환한다. */
Vector2 GetCanvasToScreenPosition(Vector2 position);

/* 화면 기준 좌표 `position`를 캔버스 기준 좌표로 변환한다. */
Vector2 GetScreenToCanvasPosition(Vector2 position);

/* 캔버스 예비 강체를 반환한다. */
frBody *GetSelectedPhysicsObject(void);

/* 캔버스 확대 배율을 반환한다. */
float GetCanvasZoomMultiplier(void);

/* 캔버스 편집 패널이 활성화되어 있는지 확인한다. */
bool IsCanvasEditPanelVisible(void);

/* 캔버스 격자가 활성화되어 있는지 확인한다. */
bool IsCanvasGridVisible(void);

/* 마우스 좌표가 현재 화면의 캔버스 안에 있는지 확인한다. */
bool IsMousePositionValid(void);

/* 캔버스 경계 범위를 `bounds`로 설정한다. */
void SetCanvasBounds(Rectangle bounds);

/* 현재 화면을 초기화한다. */
void InitCurrentScreen(void);

/* 현재 화면을 업데이트한다. */
void UpdateCurrentScreen(void);

/* 현재 화면에 할당된 메모리를 해제한다. */
void UninitCurrentScreen(void);

/* | `utils` 모듈 함수... | */

/* 주어진 부동 소수점 값 `value`를 문자열로 변환한다. */
void FloatToString(float value, char *result);

/* 주어진 문자열 `value`를 부동 소수점 값으로 변환한다. */
float StringToFloat(char *value, float min, float max);

#endif