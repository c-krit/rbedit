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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

/* | `screen` 모듈 매크로 정의... | */

#define GUI_MENU_ELEMENT_COUNT          3
#define GUI_MENU_ELEMENT_WIDTH          (GUI_GROUP_BOX_WIDTH - 24.0f)
#define GUI_MENU_ELEMENT_PADDING        10.0f

#define GUI_WINDOW_BOX_ELEMENT_COUNT    3
#define GUI_WINDOW_BOX_ELEMENT_WIDTH    (GUI_WINDOW_BOX_WIDTH - 24.0f)
#define GUI_WINDOW_BOX_ELEMENT_PADDING  8.0f

#define GUI_CANVAS_ELEMENT_COUNT        8
#define GUI_CANVAS_ELEMENT_WIDTH        (GUI_GROUP_BOX_WIDTH - 24.0f)
#define GUI_CANVAS_ELEMENT_PADDING      8.0f

#define GUI_WORLD_ELEMENT_COUNT         7
#define GUI_WORLD_ELEMENT_WIDTH         (GUI_GROUP_BOX_WIDTH - 24.0f)
#define GUI_WORLD_ELEMENT_PADDING       10.0f

#define GUI_SHAPES_ELEMENT_COUNT        14
#define GUI_SHAPES_ELEMENT_WIDTH        (GUI_GROUP_BOX_WIDTH - 24.0f)
#define GUI_SHAPES_ELEMENT_PADDING      10.0f

#define GUI_BODIES_ELEMENT_COUNT        14
#define GUI_BODIES_ELEMENT_WIDTH        (GUI_GROUP_BOX_WIDTH - 24.0f)
#define GUI_BODIES_ELEMENT_PADDING      12.0f

#define GUI_BODIES_DEFAULT_POSITION  ((Vector2) { \
    .x = 0.5f * GUI_CANVAS_INITIAL_WIDTH,         \
    .y = 0.5f * GUI_CANVAS_INITIAL_HEIGHT         \
})

#define GUI_DROPDOWN_TEXT_LENGTH        64
#define GUI_TEXTBOX_TEXT_LENGTH         12

/* | `screen` 모듈 상수... | */

const float HALF_PI = (0.5f * PI);

const Rectangle SCREEN_BOUNDS = { 
    .width = SCREEN_WIDTH, 
    .height = SCREEN_HEIGHT
};

const Rectangle GUI_PANEL_BOUNDS = {
    .width = GUI_PANEL_WIDTH,
    .height = GUI_PANEL_HEIGHT
};

const Rectangle GUI_STATUS_BAR_BOUNDS = {
    .x = GUI_PANEL_WIDTH,
    .y = SCREEN_HEIGHT - GUI_STATUS_BAR_HEIGHT,
    .width = GUI_STATUS_BAR_WIDTH - GUI_PANEL_WIDTH,
    .height = GUI_STATUS_BAR_HEIGHT
};

const Rectangle GUI_DRAW_AREA_BOUNDS = {
    .x = GUI_PANEL_WIDTH,
    .y = 0.0f,
    .width = SCREEN_WIDTH - GUI_PANEL_WIDTH,
    .height = SCREEN_HEIGHT - GUI_STATUS_BAR_HEIGHT
};

const Rectangle GUI_MESSAGE_BOX_BOUNDS = {
    (0.5f * SCREEN_WIDTH) - 150.0f, 
    (0.5f * SCREEN_HEIGHT) - 60.0f, 
    GUI_MESSAGE_BOX_WIDTH, 
    GUI_MESSAGE_BOX_HEIGHT
};

const Rectangle GUI_WINDOW_BOX_BOUNDS = {
    (0.5f * SCREEN_WIDTH) - 180.0f, 
    (0.5f * SCREEN_HEIGHT) - 80.0f, 
    GUI_WINDOW_BOX_WIDTH,
    GUI_WINDOW_BOX_HEIGHT
};

const float GUI_CANVAS_ZOOM_MULTIPLIERS[GUI_CANVAS_ZOOM_COUNT] = {
    0.25f,
    0.5f,
    0.75f,
    1.0f,
    2.0f,
    4.0f,
    8.0f
};

const frVertices GUI_SHAPES_POLYGON_VERTICES[GUI_SHAPES_MAX_VERTEX_COUNT - 2] = {
    {
        .data = {
            (Vector2) { 0.0f, -3.4f },
            (Vector2) { -3.5f, 2.6f },
            (Vector2) { 3.5f, 2.6f }
        },
        .count = 3
    },
    {
        .data = {
            (Vector2) { 0.0f, -3.2f },
            (Vector2) { -3.2f, 0.0f },
            (Vector2) { 0.0f, 3.2f },
            (Vector2) { 3.2f, 0.0f }
        },
        .count = 4
    },
    {
        .data = {
            (Vector2) { 0.0f, -3.6f },
            (Vector2) { -3.2f, -0.8f },
            (Vector2) { -1.6f, 3.2f },
            (Vector2) { 1.6f, 3.2f },
            (Vector2) { 3.2f, -0.8f }
        },
        .count = 5
    },
    {
        .data = {
            (Vector2) { -1.4f, -3.2f },
            (Vector2) { -3.2f, 0.0f },
            (Vector2) { -1.4f, 3.2f },
            (Vector2) { 1.4f, 3.2f },
            (Vector2) { 3.2f, 0.0f },
            (Vector2) { 1.4f, -3.2f }
        },
        .count = 6
    },
};

/* | `screen` 모듈 변수... | */

static Rectangle gui_main_group_box_bounds[GUI_MAIN_GROUP_BOX_COUNT] = {
    { .width = GUI_GROUP_BOX_WIDTH, .height = 138.0f },
    { .width = GUI_GROUP_BOX_WIDTH, .height = 132.0f },
    { .width = GUI_GROUP_BOX_WIDTH, .height = 264.0f }
};

static Rectangle gui_edit_group_box_bounds[GUI_EDIT_GROUP_BOX_COUNT] = {
    { .width = GUI_GROUP_BOX_WIDTH, .height = 248.0f },
    { .width = GUI_GROUP_BOX_WIDTH, .height = 324.0f },
    { .width = GUI_GROUP_BOX_WIDTH, .height = 32.0f }
};

static Rectangle gui_menu_element_bounds[GUI_MENU_ELEMENT_COUNT] = {
    { .width = GUI_MENU_ELEMENT_WIDTH, .height = 32.0f },
    { .width = GUI_MENU_ELEMENT_WIDTH, .height = 32.0f },
    { .width = GUI_MENU_ELEMENT_WIDTH, .height = 32.0f }
};

static Rectangle gui_canvas_element_bounds[GUI_CANVAS_ELEMENT_COUNT] = {
    { .width = 0.3f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.7f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.3f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.7f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.3f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.7f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.9f * GUI_CANVAS_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.1f * GUI_CANVAS_ELEMENT_WIDTH, .height = 20.0f }
};

static Rectangle gui_world_element_bounds[GUI_WORLD_ELEMENT_COUNT] = {
    { .width = 1.0f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 0.4f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 0.6f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 1.0f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 1.0f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 1.0f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f },
    { .width = 1.0f * GUI_WORLD_ELEMENT_WIDTH, .height = 32.0f }
};

static Rectangle gui_shapes_element_bounds[GUI_SHAPES_ELEMENT_COUNT] = {
    { .width = 0.3f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.7f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f }
};

static Rectangle gui_bodies_element_bounds[GUI_BODIES_ELEMENT_COUNT] = {
    { .width = 0.3f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.7f * GUI_SHAPES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.6f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.4f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.3f * GUI_BODIES_ELEMENT_WIDTH, .height = 84.0f },
    { .width = 0.6f * GUI_BODIES_ELEMENT_WIDTH, .height = 84.0f },
    { .width = 0.9f * GUI_BODIES_ELEMENT_WIDTH, .height = 24.0f },
    { .width = 0.1f * GUI_BODIES_ELEMENT_WIDTH, .height = 20.0f }
};

static Rectangle gui_window_box_element_bounds[GUI_WINDOW_BOX_ELEMENT_COUNT] = {
    { .width = GUI_WINDOW_BOX_ELEMENT_WIDTH, .height = 24.0f },
    { .width = GUI_WINDOW_BOX_ELEMENT_WIDTH, .height = 24.0f },
    { .width = GUI_WINDOW_BOX_ELEMENT_WIDTH, .height = 24.0f }
};

static Rectangle gui_canvas_bounds = { .width = 0.0f, .height = 0.0f };

static Camera2D gui_canvas_camera;
static RenderTexture gui_canvas_rtex;

static frMaterial gui_shapes_material = GUI_SHAPES_DEFAULT_MATERIAL;
static Vector2 gui_shapes_rec_dimensions = { .x = 8.0f, .y = 6.0f };

static frShape *gui_shapes_circle = NULL;
static frShape *gui_shapes_rectangle = NULL;
static frShape *gui_shapes_polygon = NULL;

static Vector2 gui_bodies_position = { 0.0f };

static frBodyFlags gui_bodies_preview_flags = FR_FLAG_NONE;
static frBody *gui_bodies_preview_body = NULL;
static frBody *gui_bodies_selected_body = NULL;

static ObjectData gui_bodies_preview_data = { .color = RED, .no_fill = true };

static bool gui_edit_panel_visible = false;

static bool gui_menu_load_message_box_visible = false;
static bool gui_menu_save_message_box_visible = false;

static bool gui_canvas_width_editable = false;
static bool gui_canvas_height_editable = false;
static bool gui_canvas_zoom_editable = false;
static bool gui_canvas_grid_visible = true;

static bool gui_world_gravity_multiplier_editable = false;

static bool gui_shapes_type_editable = false;
static bool gui_shapes_density_editable = false;
static bool gui_shapes_restitution_editable = false;
static bool gui_shapes_static_friction_editable = false;
static bool gui_shapes_dynamic_friction_editable = false;
static bool gui_shapes_radius_editable = false;
static bool gui_shapes_width_editable = false;
static bool gui_shapes_height_editable = false;
static bool gui_shapes_vertex_count_editable = false;

static bool gui_bodies_type_editable = false;
static bool gui_bodies_position_x_editable = false;
static bool gui_bodies_position_y_editable = false;
static bool gui_bodies_rotation_editable = false;
static bool gui_bodies_gravity_scale_editable = false;

static char gui_canvas_zoom_dropdown_text[GUI_DROPDOWN_TEXT_LENGTH];
static char gui_shapes_type_dropdown_text[GUI_DROPDOWN_TEXT_LENGTH];
static char gui_bodies_type_dropdown_text[GUI_DROPDOWN_TEXT_LENGTH];

static char gui_world_gravity_multiplier_text[GUI_TEXTBOX_TEXT_LENGTH];

static char gui_shapes_density_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_restitution_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_static_friction_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_dynamic_friction_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_radius_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_width_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_shapes_height_text[GUI_TEXTBOX_TEXT_LENGTH];

static char gui_bodies_position_x_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_bodies_position_y_text[GUI_TEXTBOX_TEXT_LENGTH];
static char gui_bodies_gravity_scale_text[GUI_TEXTBOX_TEXT_LENGTH];

static float gui_world_gravity_multiplier = 0.00001f;

static float gui_shapes_circle_radius = 3.2f;

static float gui_bodies_gravity_scale = 1.0f;

static int gui_canvas_new_width = GUI_CANVAS_INITIAL_WIDTH;
static int gui_canvas_new_height = GUI_CANVAS_INITIAL_HEIGHT;
static int gui_canvas_zoom_index = 3;

static int gui_shapes_type_index = 0;
static int gui_shapes_polygon_vertex_count = 3;

static int gui_bodies_type_index = 2;
static int gui_bodies_rotation = 0;

/* | `screen` 모듈 함수... | */

/* 현재 화면에 캔버스를 그린다. */
static void DrawCanvas(void);

/* 현재 화면에 마우스 커서를 그린다. */
static void DrawCursor(void);

/* 현재 화면에 패널을 그린다. */
static void DrawCurrentPanel(void);

/* 현재 화면에 메인 패널을 그린다. */
static void DrawMainPanel(void);

/* 현재 화면에 편집 패널을 그린다. */
static void DrawEditPanel(void);

/* 현재 화면에 상태 바를 그린다. */
static void DrawStatusBar(void);

/* 사용자의 마우스 스크롤 입력을 처리한다. */
static void HandleMouseWheel(void);

/* 사용자의 마우스와 키보드 입력을 처리한다. */
static void HandleUserInput(void);

/* 게임 세계에서 위치 `position`을 포함하는 개체를 선택한다. */
static void SelectPhysicsObject(Vector2 position);

/* 메인 패널의 그룹 박스 위치를 설정한다. */
static void SetMainGroupBoxPositions(void);

/* 편집 패널의 그룹 박스 위치를 설정한다. */
static void SetEditGroupBoxPositions(void);

/* `메뉴` 그룹 박스의 요소 위치를 설정한다. */
static void SetMenuElementPositions(void);

/* `캔버스` 그룹 박스의 요소 위치를 설정한다. */
static void SetCanvasElementPositions(void);

/* `세계` 그룹 박스의 요소 위치를 설정한다. */
static void SetWorldElementPositions(void);

/* `모양` 그룹 박스의 요소 위치를 설정한다. */
static void SetShapesElementPositions(void);

/* `강체` 그룹 박스의 요소 위치를 설정한다. */
static void SetBodiesElementPositions(void);

/* `정보` 메시지 박스의 요소 위치를 설정한다. */
static void SetWindowBoxElementPositions(void);

/* `캔버스` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetCanvasTextValues(void);

/* `세계` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetWorldTextValues(void);

/* `모양` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetShapesTextValues(void);

/* `강체` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetBodiesTextValues(void);

/* `열기` 메시지 상자를 그린다. */
static void ShowLoadMessageBox(void);

/* `저장` 메시지 상자를 그린다. */
static void ShowSaveMessageBox(void);

/* 캔버스 경계 범위를 반환한다. */
Rectangle GetCanvasBounds(void) {
    return gui_canvas_bounds;
}

/* 캔버스를 기준으로 한 마우스 좌표를 반환한다. */
Vector2 GetCanvasMousePosition(void) {
    return GetScreenToCanvasPosition(GetMousePosition());
}

/* 캔버스 기준 좌표 `position`를 화면 기준 좌표로 변환한다. */
Vector2 GetCanvasToScreenPosition(Vector2 position) {
    return GetWorldToScreen2D(
        frVec2Add(position, (Vector2) { gui_canvas_bounds.x, gui_canvas_bounds.y }),
        gui_canvas_camera
    );
}

/* 화면 기준 좌표 `position`를 캔버스 기준 좌표로 변환한다. */
Vector2 GetScreenToCanvasPosition(Vector2 position) {
    return frVec2Subtract(
        GetScreenToWorld2D(position, gui_canvas_camera),
        (Vector2) { gui_canvas_bounds.x, gui_canvas_bounds.y }
    );
}

/* 캔버스 예비 강체를 반환한다. */
frBody *GetSelectedPhysicsObject(void) {
    return gui_bodies_selected_body;
}

/* 캔버스 확대 배율을 반환한다. */
float GetCanvasZoomMultiplier(void) {
    return GUI_CANVAS_ZOOM_MULTIPLIERS[gui_canvas_zoom_index];
}

/* 캔버스 편집 패널이 활성화되어 있는지 확인한다. */
bool IsCanvasEditPanelVisible(void) {
    return gui_edit_panel_visible;
}

/* 캔버스 격자가 활성화되어 있는지 확인한다. */
bool IsCanvasGridVisible(void) {
    return gui_canvas_grid_visible;
}

/* 마우스 좌표가 캔버스 안에 있는지 확인한다. */
bool IsMousePositionValid(void) {
    return CheckCollisionPointRec(
        GetCanvasMousePosition(), 
        (Rectangle) { 
            .width = gui_canvas_bounds.width, 
            .height = gui_canvas_bounds.height 
        }
    );
}

/* 캔버스 경계 범위를 `bounds`로 설정한다. */
void SetCanvasBounds(Rectangle bounds) {
    gui_canvas_bounds.width = bounds.width;
    gui_canvas_bounds.height = bounds.height;

    gui_canvas_bounds.x = GUI_DRAW_AREA_BOUNDS.x
        + (0.5f * (GUI_DRAW_AREA_BOUNDS.width - gui_canvas_bounds.width));

    gui_canvas_bounds.y = GUI_DRAW_AREA_BOUNDS.y
        + (0.5f * (GUI_DRAW_AREA_BOUNDS.height - gui_canvas_bounds.height));
    
    SetPhysicsWorldBounds((Rectangle) { .width = bounds.width, .height = bounds.height });
}

/* 현재 화면을 초기화한다. */
void InitCurrentScreen(void) {
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    SetPhysicsGravityMultiplier(gui_world_gravity_multiplier);

    SetCanvasBounds(
        (Rectangle) { 
            .width = GUI_CANVAS_INITIAL_WIDTH, 
            .height = GUI_CANVAS_INITIAL_HEIGHT 
        }
    );

    SetMainGroupBoxPositions();
    SetEditGroupBoxPositions();
    
    SetMenuElementPositions();
    SetBodiesElementPositions();
    SetCanvasElementPositions();
    SetShapesElementPositions();
    SetWorldElementPositions();

    SetWindowBoxElementPositions();

    SetCanvasTextValues();
    SetWorldTextValues();
    SetShapesTextValues();
    SetBodiesTextValues();

    gui_canvas_camera.offset = (Vector2) {
        .x = GUI_DRAW_AREA_BOUNDS.x + (0.5f * GUI_DRAW_AREA_BOUNDS.width),
        .y = GUI_DRAW_AREA_BOUNDS.y + (0.5f * GUI_DRAW_AREA_BOUNDS.height)
    };

    gui_canvas_camera.target = gui_canvas_camera.offset;
    gui_canvas_camera.zoom = GUI_CANVAS_ZOOM_MULTIPLIERS[gui_canvas_zoom_index];

    gui_canvas_rtex = LoadRenderTexture(GUI_CANVAS_MAXIMUM_WIDTH, GUI_CANVAS_MAXIMUM_HEIGHT);

    gui_canvas_new_width = gui_canvas_bounds.width;
    gui_canvas_new_height = gui_canvas_bounds.height;

    gui_shapes_circle = frCreateCircle(gui_shapes_material, gui_shapes_circle_radius);

    gui_shapes_rectangle = frCreateRectangle(
        gui_shapes_material, 
        gui_shapes_rec_dimensions.x,
        gui_shapes_rec_dimensions.y
    );

    gui_shapes_polygon = frCreatePolygon(gui_shapes_material, GUI_SHAPES_POLYGON_VERTICES[0]);

    gui_bodies_position = frVec2PixelsToMeters(GUI_BODIES_DEFAULT_POSITION);

    gui_bodies_preview_body = frCreateBody(
        (frBodyType) gui_bodies_type_index,
        gui_bodies_preview_flags,
        gui_bodies_position
    );

    gui_bodies_preview_data.id = -1;

    frSetBodyRotation(gui_bodies_preview_body, gui_bodies_rotation);
    frSetBodyUserData(gui_bodies_preview_body, &gui_bodies_preview_data);
    frSetBodyGravityScale(gui_bodies_preview_body, gui_bodies_gravity_scale);

    frAttachShapeToBody(gui_bodies_preview_body, gui_shapes_circle);
}

/* 현재 화면을 업데이트한다. */
void UpdateCurrentScreen(void) {
    SimulatePhysicsWorld();

    BeginDrawing();

    ClearBackground(BLACK);

    HandleUserInput();

    DrawCanvas();
    DrawStatusBar();
    DrawCurrentPanel();

    ShowLoadMessageBox();
    ShowSaveMessageBox();

    DrawCursor();

    EndDrawing();
}

/* 현재 화면에 할당된 메모리를 해제한다. */
void UninitCurrentScreen(void) {
    UnloadRenderTexture(gui_canvas_rtex);

    frReleaseBody(gui_bodies_preview_body);

    frReleaseShape(gui_shapes_circle);
    frReleaseShape(gui_shapes_rectangle);
    frReleaseShape(gui_shapes_polygon);
}

/* 현재 화면에 캔버스를 그린다. */
static void DrawCanvas(void) {
    DrawRectangleRec(GUI_DRAW_AREA_BOUNDS, DARKGRAY);

    {
        BeginTextureMode(gui_canvas_rtex);

        DrawPhysicsWorld();

        EndTextureMode();
    }

    // 렌더 텍스처의 필터를 설정한다.
    SetTextureFilter(gui_canvas_rtex.texture, TEXTURE_FILTER_BILINEAR);

    {
        BeginMode2D(gui_canvas_camera);

        // 렌더 텍스처의 맨 왼쪽 윗부분만 자른다.
        DrawTexturePro(
            gui_canvas_rtex.texture,
            (Rectangle) {
                .y = GUI_CANVAS_MAXIMUM_HEIGHT - gui_canvas_bounds.height,
                .width = gui_canvas_bounds.width, 
                .height = -gui_canvas_bounds.height 
            },
            gui_canvas_bounds,
            FR_STRUCT_ZERO(Vector2),
            0.0f,
            WHITE
        );

        EndMode2D();
    }
}

/* 현재 화면에 마우스 커서를 그린다. */
static void DrawCursor(void) {
    if (!IsMousePositionValid()) {
        ShowCursor();

        return;
    }

    HideCursor();

    Vector2 position = GetMousePosition();

    DrawLineEx(
        (Vector2) { position.x - 6.0f, position.y },
        (Vector2) { position.x + 6.0f, position.y },
        2.0f,
        BLACK
    );
    
    DrawLineEx(
        (Vector2) { position.x, position.y - 6.0f },
        (Vector2) { position.x, position.y + 6.0f },
        2.0f,
        BLACK
    );
}

/* 현재 화면에 패널을 그린다. */
static void DrawCurrentPanel(void) {
    if (gui_edit_panel_visible) DrawEditPanel();
    else DrawMainPanel();
}

/* 현재 화면에 메인 패널을 그린다. */
static void DrawMainPanel(void) {
    GuiPanel(GUI_PANEL_BOUNDS);

    {
        GuiGroupBox(gui_main_group_box_bounds[0], "MENU");

        if (GuiButton(gui_menu_element_bounds[0], "#1#LOAD...")) {
            if (!gui_menu_load_message_box_visible)
                gui_menu_load_message_box_visible = true;
        }

        if (GuiButton(gui_menu_element_bounds[1], "#2#SAVE...")) {
            if (!gui_menu_save_message_box_visible)
                gui_menu_save_message_box_visible = true;
        }

        if (GuiButton(gui_menu_element_bounds[2], "#191#ABOUT..."))
            OpenURL(RBEDIT_REPOSITORY_URL);
    }

    {
        GuiGroupBox(gui_main_group_box_bounds[2], "WORLD");

        GuiLabel(
            gui_world_element_bounds[0], 
            TextFormat(
                "STATE: %s (%d BODIES)", 
                GetPhysicsWorldStateText(),
                frGetWorldBodyCount(GetPhysicsWorld())
            )
        );

        GuiLabel(gui_world_element_bounds[1], "GRAVITY: ");

        if (GuiTextBox(
            gui_world_element_bounds[2],
            gui_world_gravity_multiplier_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_world_gravity_multiplier_editable
        )) {
            gui_world_gravity_multiplier_editable = !gui_world_gravity_multiplier_editable;

            gui_world_gravity_multiplier = StringToFloat(
                gui_world_gravity_multiplier_text,
                0.000001f,
                GUI_SHAPES_TEXTBOX_MAX_VALUE
            );

            FloatToString(gui_world_gravity_multiplier, gui_world_gravity_multiplier_text);

            SetPhysicsGravityMultiplier(gui_world_gravity_multiplier);
        }

        if (GuiButton(gui_world_element_bounds[3], "#131#PLAY")) 
            SetPhysicsWorldState(PHYSICS_STATE_RUNNING);
        
        if (GuiButton(gui_world_element_bounds[4], "#132#PAUSE"))
            SetPhysicsWorldState(PHYSICS_STATE_PAUSED);
        
        if (GuiButton(gui_world_element_bounds[5], "#73#RESTART"))
            SetPhysicsWorldState(PHYSICS_STATE_RESTARTING);
        
        if (GuiButton(gui_world_element_bounds[6], "#22#SINGLE-STEP"))
            SetPhysicsWorldState(PHYSICS_STATE_STEPPING);
    }

    {
        GuiGroupBox(gui_main_group_box_bounds[1], "CANVAS");

        GuiLabel(gui_canvas_element_bounds[0], "WIDTH: ");

        if (GuiSpinner(
            gui_canvas_element_bounds[1],
            NULL, 
            &gui_canvas_new_width,
            0, 
            GUI_CANVAS_MAXIMUM_WIDTH, 
            gui_canvas_width_editable
        )) gui_canvas_width_editable = !gui_canvas_width_editable;
        
        GuiLabel(gui_canvas_element_bounds[2], "HEIGHT: ");

        if (GuiSpinner(
            gui_canvas_element_bounds[3], 
            NULL, 
            &gui_canvas_new_height,
            0, 
            GUI_CANVAS_MAXIMUM_HEIGHT, 
            gui_canvas_height_editable
        )) gui_canvas_height_editable = !gui_canvas_height_editable;

        if (gui_canvas_new_width != (int) gui_canvas_bounds.width 
            || gui_canvas_new_height != (int) gui_canvas_bounds.height)
            SetCanvasBounds((Rectangle) { 
                .width = gui_canvas_new_width, 
                .height = gui_canvas_new_height 
            });

        GuiLabel(gui_canvas_element_bounds[6], "SHOW WORLD BOUNDS (GRIDS): ");

        gui_canvas_grid_visible = GuiCheckBox(
            gui_canvas_element_bounds[7], 
            NULL, 
            gui_canvas_grid_visible
        );

        GuiLabel(gui_canvas_element_bounds[4], "ZOOM: ");

        if (GuiDropdownBox(
            gui_canvas_element_bounds[5], 
            gui_canvas_zoom_dropdown_text,
            &gui_canvas_zoom_index, 
            gui_canvas_zoom_editable
        )) {
            gui_canvas_camera.zoom = GUI_CANVAS_ZOOM_MULTIPLIERS[gui_canvas_zoom_index];

            gui_canvas_zoom_editable = !gui_canvas_zoom_editable;

            if (gui_canvas_zoom_editable) GuiLock();
            else GuiUnlock();
        }
    }
}

/* 현재 화면에 편집 패널을 그린다. */
static void DrawEditPanel(void) {
    GuiPanel(GUI_PANEL_BOUNDS);

    {
        GuiGroupBox(gui_edit_group_box_bounds[0], "SHAPES");

        GuiLabel(gui_shapes_element_bounds[2], "DENSITY: ");

        if (GuiTextBox(
            gui_shapes_element_bounds[3],
            gui_shapes_density_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_shapes_density_editable
        )) {
            gui_shapes_density_editable = !gui_shapes_density_editable;

            gui_shapes_material.density = StringToFloat(
                gui_shapes_density_text,
                0.0f,
                GUI_SHAPES_TEXTBOX_MAX_VALUE
            );

            FloatToString(gui_shapes_material.density, gui_shapes_density_text);

            frShape *shape = frGetBodyShape(gui_bodies_selected_body);

            frSetShapeMaterial(shape, gui_shapes_material);
            frAttachShapeToBody(gui_bodies_selected_body, shape);
        }

        GuiLabel(gui_shapes_element_bounds[4], "RESTITUTION: ");

        if (GuiTextBox(
            gui_shapes_element_bounds[5],
            gui_shapes_restitution_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_shapes_restitution_editable
        )) {
            gui_shapes_restitution_editable = !gui_shapes_restitution_editable;

            gui_shapes_material.restitution = StringToFloat(
                gui_shapes_restitution_text,
                0.0f,
                GUI_SHAPES_TEXTBOX_MAX_VALUE
            );

            FloatToString(gui_shapes_material.restitution, gui_shapes_restitution_text);

            frShape *shape = frGetBodyShape(gui_bodies_selected_body);

            frSetShapeMaterial(shape, gui_shapes_material);
            frAttachShapeToBody(gui_bodies_selected_body, shape);
        }

        GuiLabel(gui_shapes_element_bounds[6], "S. FRICTION: ");

        if (GuiTextBox(
            gui_shapes_element_bounds[7],
            gui_shapes_static_friction_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_shapes_static_friction_editable
        )) {
            gui_shapes_static_friction_editable = !gui_shapes_static_friction_editable;

            gui_shapes_material.static_friction = StringToFloat(
                gui_shapes_static_friction_text,
                0.0f,
                1.0f
            );

            FloatToString(gui_shapes_material.static_friction, gui_shapes_static_friction_text);

            frShape *shape = frGetBodyShape(gui_bodies_selected_body);

            frSetShapeMaterial(shape, gui_shapes_material);
            frAttachShapeToBody(gui_bodies_selected_body, shape);
        }

        GuiLabel(gui_shapes_element_bounds[8], "D. FRICTION: ");

        if (GuiTextBox(
            gui_shapes_element_bounds[9],
            gui_shapes_dynamic_friction_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_shapes_dynamic_friction_editable
        )) {
            gui_shapes_dynamic_friction_editable = !gui_shapes_dynamic_friction_editable;

            gui_shapes_material.dynamic_friction = StringToFloat(
                gui_shapes_dynamic_friction_text,
                gui_shapes_material.static_friction,
                1.0f
            );

            FloatToString(gui_shapes_material.dynamic_friction, gui_shapes_dynamic_friction_text);

            frShape *shape = frGetBodyShape(gui_bodies_selected_body);

            frSetShapeMaterial(shape, gui_shapes_material);
            frAttachShapeToBody(gui_bodies_selected_body, shape);
        }

        if (gui_shapes_type_index == 0) {
            GuiLabel(gui_shapes_element_bounds[10], "RADIUS: ");

            if (GuiTextBox(
                gui_shapes_element_bounds[11],
                gui_shapes_radius_text,
                GUI_TEXTBOX_TEXT_LENGTH,
                gui_shapes_radius_editable
            )) {
                gui_shapes_radius_editable = !gui_shapes_radius_editable;

                gui_shapes_circle_radius = StringToFloat(
                    gui_shapes_radius_text,
                    0.0f,
                    GUI_SHAPES_TEXTBOX_MAX_VALUE
                );

                FloatToString(gui_shapes_circle_radius, gui_shapes_radius_text);

                frSetCircleRadius(gui_shapes_circle, gui_shapes_circle_radius);
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_circle);
            }
        } else if (gui_shapes_type_index == 1) {
            GuiLabel(gui_shapes_element_bounds[10], "WIDTH: ");

            if (GuiTextBox(
                gui_shapes_element_bounds[11],
                gui_shapes_width_text,
                GUI_TEXTBOX_TEXT_LENGTH,
                gui_shapes_width_editable
            )) {
                gui_shapes_width_editable = !gui_shapes_width_editable;

                gui_shapes_rec_dimensions.x = StringToFloat(
                    gui_shapes_width_text,
                    0.1f,
                    GUI_SHAPES_TEXTBOX_MAX_VALUE
                );

                FloatToString(gui_shapes_rec_dimensions.x, gui_shapes_width_text);

                frSetRectangleDimensions(gui_shapes_rectangle, gui_shapes_rec_dimensions);
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_rectangle);
            }

            GuiLabel(gui_shapes_element_bounds[12], "HEIGHT: ");

            if (GuiTextBox(
                gui_shapes_element_bounds[13],
                gui_shapes_height_text,
                GUI_TEXTBOX_TEXT_LENGTH,
                gui_shapes_height_editable
            )) {
                gui_shapes_height_editable = !gui_shapes_height_editable;

                gui_shapes_rec_dimensions.y = StringToFloat(
                    gui_shapes_height_text,
                    0.1f,
                    GUI_SHAPES_TEXTBOX_MAX_VALUE
                );

                FloatToString(gui_shapes_rec_dimensions.y, gui_shapes_height_text);

                frSetRectangleDimensions(gui_shapes_rectangle, gui_shapes_rec_dimensions);
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_rectangle);
            }
        } else {
            GuiLabel(gui_shapes_element_bounds[10], "MAX VERTEX COUNT: ");

            if (GuiSpinner(
                gui_shapes_element_bounds[11],
                NULL,
                &gui_shapes_polygon_vertex_count,
                3,
                GUI_SHAPES_MAX_VERTEX_COUNT,
                gui_shapes_vertex_count_editable
            )) gui_shapes_vertex_count_editable = !gui_shapes_vertex_count_editable;

            frVertices vertices = frGetPolygonVertices(gui_shapes_polygon);

            if (vertices.count != gui_shapes_polygon_vertex_count) {
                frVertices new_vertices = GUI_SHAPES_POLYGON_VERTICES[gui_shapes_polygon_vertex_count - 3];

                for (int i = 0; i < gui_shapes_polygon_vertex_count; i++)
                    vertices.data[i] = new_vertices.data[i];

                vertices.count = gui_shapes_polygon_vertex_count;

                frSetPolygonVertices(gui_shapes_polygon, vertices);
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_polygon);
            }
        }

        GuiLabel(gui_shapes_element_bounds[0], "TYPE: ");

        // 드롭다운 박스는 맨 나중에 그리기
        if (GuiDropdownBox(
            gui_shapes_element_bounds[1], 
            gui_shapes_type_dropdown_text,
            &gui_shapes_type_index, 
            gui_shapes_type_editable
        )) {
            gui_shapes_type_editable = !gui_shapes_type_editable;

            if (gui_shapes_type_editable) GuiLock();
            else GuiUnlock();

            if (gui_shapes_type_index == 0) 
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_circle);
            else if (gui_shapes_type_index == 1)
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_rectangle);
            else
                frAttachShapeToBody(gui_bodies_selected_body, gui_shapes_polygon);
        }
    }

    {
        GuiGroupBox(gui_edit_group_box_bounds[1], "BODIES");

        GuiLabel(gui_bodies_element_bounds[2], "POSITION.X: ");

        if (GuiTextBox(
            gui_bodies_element_bounds[3],
            gui_bodies_position_x_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_bodies_position_x_editable
        )) {
            gui_bodies_position_x_editable = !gui_bodies_position_x_editable;

            gui_bodies_position.x = StringToFloat(
                gui_bodies_position_x_text,
                0.0f,
                gui_canvas_bounds.width
            );

            FloatToString(gui_bodies_position.x, gui_bodies_position_x_text);

            frSetBodyPosition(gui_bodies_selected_body, gui_bodies_position);
        }

        GuiLabel(gui_bodies_element_bounds[4], "POSITION.Y: ");

        if (GuiTextBox(
            gui_bodies_element_bounds[5],
            gui_bodies_position_y_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_bodies_position_y_editable
        )) {
            gui_bodies_position_y_editable = !gui_bodies_position_y_editable;

            gui_bodies_position.y = StringToFloat(
                gui_bodies_position_y_text,
                0.0f,
                gui_canvas_bounds.height
            );

            FloatToString(gui_bodies_position.y, gui_bodies_position_y_text);

            frSetBodyPosition(gui_bodies_selected_body, gui_bodies_position);
        }

        GuiLabel(gui_bodies_element_bounds[6], "ROTATION (DEG.): ");

        if (GuiSpinner(
            gui_bodies_element_bounds[7],
            NULL,
            &gui_bodies_rotation,
            0,
            359,
            gui_bodies_rotation_editable
        )) gui_bodies_rotation_editable = !gui_bodies_rotation_editable;

        frSetBodyRotation(gui_bodies_selected_body, DEG2RAD * gui_bodies_rotation);

        GuiLabel(gui_bodies_element_bounds[8], "GRAVITY SCALE: ");

        if (GuiTextBox(
            gui_bodies_element_bounds[9],
            gui_bodies_gravity_scale_text,
            GUI_TEXTBOX_TEXT_LENGTH,
            gui_bodies_gravity_scale_editable
        )) {
            gui_bodies_gravity_scale_editable = !gui_bodies_gravity_scale_editable;

            gui_bodies_gravity_scale = StringToFloat(
                gui_bodies_gravity_scale_text,
                0.0f,
                GUI_BODIES_TEXTBOX_MAX_VALUE
            );

            FloatToString(gui_bodies_gravity_scale, gui_bodies_gravity_scale_text);
            
            frSetBodyGravityScale(gui_bodies_selected_body, gui_bodies_gravity_scale);
        }

        GuiLabel(gui_bodies_element_bounds[10], "COLOR: ");

        ObjectData *data = (ObjectData *) frGetBodyUserData(gui_bodies_selected_body);

        data->color = GuiColorPicker(gui_bodies_element_bounds[11], data->color);

        GuiLabel(gui_bodies_element_bounds[12], "SHOW ONLY BORDER (NO FILL): ");

        data->no_fill = GuiCheckBox(gui_bodies_element_bounds[13], NULL, data->no_fill);

        frSetBodyUserData(gui_bodies_selected_body, data);

        GuiLabel(gui_bodies_element_bounds[0], "TYPE: ");
        
        // 드롭다운 박스는 맨 나중에 그리기
        if (GuiDropdownBox(
            gui_bodies_element_bounds[1], 
            gui_bodies_type_dropdown_text,
            &gui_bodies_type_index, 
            gui_bodies_type_editable
        )) {
            gui_bodies_type_editable = !gui_bodies_type_editable;

            if (gui_bodies_type_editable) GuiLock();
            else GuiUnlock();

            frSetBodyType(gui_bodies_selected_body, (frBodyType) gui_bodies_type_index);
            frSetBodyGravityScale(gui_bodies_selected_body, gui_bodies_gravity_scale);
        }
    }
    
    if (GetPhysicsWorldState() == PHYSICS_STATE_PAUSED) {
        if (gui_bodies_selected_body != gui_bodies_preview_body) {
            ObjectData *data = frGetBodyUserData(gui_bodies_selected_body);

            UpdatePhysicsObjectInWorld(data->id, gui_bodies_selected_body);
        }
    }

    if (gui_bodies_selected_body == gui_bodies_preview_body) {
        if (GuiButton(gui_edit_group_box_bounds[2], "CREATE A NEW BODY")) {
            frBody *body = ClonePhysicsObject(gui_bodies_preview_body);

            AddPhysicsObjectToWorld(body, gui_bodies_preview_data);

            frSetBodyPosition(
                gui_bodies_preview_body,
                (gui_bodies_position = frVec2PixelsToMeters(GUI_BODIES_DEFAULT_POSITION))
            );
        }
    } else {
        if (GuiButton(gui_edit_group_box_bounds[2], "REMOVE SELECTED BODY")) {
            frBody *body = gui_bodies_selected_body;

            ObjectData *data = (ObjectData *) frGetBodyUserData(body);

            RemovePhysicsObjectFromWorld(data->id);

            gui_bodies_selected_body = gui_bodies_preview_body;
        }
    }
}

/* 현재 화면에 상태 바를 그린다. */
static void DrawStatusBar(void) {
    Vector2 mouse_position = GetCanvasMousePosition();

    GuiStatusBar(
        GUI_STATUS_BAR_BOUNDS,
        TextFormat(
            IsMousePositionValid() ? "%d%% / (%.02f, %.02f)" : "%d%% / (-, -)", 
            (int) (100 * GUI_CANVAS_ZOOM_MULTIPLIERS[gui_canvas_zoom_index]),
            mouse_position.x,
            mouse_position.y
        )
    );
}

/* 사용자의 마우스 스크롤 입력을 처리한다. */
static void HandleMouseWheel(void) {
    float wheel = GetMouseWheelMove();

    // 마우스 컨트롤 키를 눌렀을 때만 확대 배율 조정
    if (!IsKeyDown(KEY_LEFT_CONTROL)) return;

    if (wheel != 0.0f) {
        if (wheel > 0.0f) {
            if (gui_canvas_zoom_index < GUI_CANVAS_ZOOM_COUNT - 1)
                gui_canvas_zoom_index++;
        } else if (wheel < 0.0f) {
            if (gui_canvas_zoom_index > 0)
                gui_canvas_zoom_index--;
        }

        gui_canvas_camera.zoom = GUI_CANVAS_ZOOM_MULTIPLIERS[gui_canvas_zoom_index];
    }
}

/* 사용자의 마우스와 키보드 입력을 처리한다. */
static void HandleUserInput(void) {
    const Vector2 mouse_position = GetMousePosition();
    const Vector2 canvas_mouse_position = GetScreenToCanvasPosition(mouse_position);

    if (gui_menu_load_message_box_visible 
        || gui_menu_save_message_box_visible) {
        if (!CheckCollisionPointRec(mouse_position, GUI_MESSAGE_BOX_BOUNDS)) GuiLock();
        else GuiUnlock();

        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // 캔버스를 클릭했을 때만 `편집` 패널 보여주기
        if (CheckCollisionPointRec(mouse_position, GUI_DRAW_AREA_BOUNDS)) {
            SelectPhysicsObject(canvas_mouse_position);
            
            gui_edit_panel_visible = IsMousePositionValid();
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(
            frVec2PixelsToMeters(canvas_mouse_position),
            frGetBodyAABB(gui_bodies_selected_body)
        )) {
            gui_bodies_position = frVec2Add(
                gui_bodies_position, 
                frVec2PixelsToMeters(GetMouseDelta())
            );

            FloatToString(gui_bodies_position.x, gui_bodies_position_x_text);
            FloatToString(gui_bodies_position.y, gui_bodies_position_y_text);

            frSetBodyPosition(gui_bodies_selected_body, gui_bodies_position);
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) gui_edit_panel_visible = false;

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)
        || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
        if (IsKeyDown(KEY_LEFT)) gui_bodies_position.x -= 0.1f;
        if (IsKeyDown(KEY_RIGHT)) gui_bodies_position.x += 0.1f;
        if (IsKeyDown(KEY_UP)) gui_bodies_position.y -= 0.1f;
        if (IsKeyDown(KEY_DOWN)) gui_bodies_position.y += 0.1f;

        FloatToString(gui_bodies_position.x, gui_bodies_position_x_text);
        FloatToString(gui_bodies_position.y, gui_bodies_position_y_text);

        frSetBodyPosition(gui_bodies_selected_body, gui_bodies_position);
    }

    HandleMouseWheel();
}

/* 게임 세계에서 위치 `position`을 포함하는 개체를 선택한다. */
static void SelectPhysicsObject(Vector2 position) {
    frBody *body = GetPhysicsObjectAt(position);

    if (body != NULL) gui_bodies_selected_body = body;
    else gui_bodies_selected_body = gui_bodies_preview_body;

    frShape *shape = frGetBodyShape(gui_bodies_selected_body);
    
    gui_shapes_type_index = (frGetShapeType(shape) == FR_SHAPE_CIRCLE) ? 0 : 2;

    if (frIsShapeRectangle(shape)) gui_shapes_type_index = 1;

    gui_shapes_material = frGetShapeMaterial(shape);

    gui_shapes_circle_radius = frGetCircleRadius(shape);
    
    gui_bodies_type_index = (int) frGetBodyType(gui_bodies_selected_body);
    gui_bodies_position = frGetBodyPosition(gui_bodies_selected_body);
    gui_bodies_rotation = (int) (RAD2DEG * frGetBodyRotation(gui_bodies_selected_body));
    
    gui_bodies_gravity_scale = frGetBodyGravityScale(gui_bodies_selected_body);

    SetShapesTextValues();
    SetBodiesTextValues();
}

/* 메인 패널의 그룹 박스 위치를 설정한다. */
static void SetMainGroupBoxPositions(void) {
    gui_main_group_box_bounds[0].x = 0.5f * (GUI_PANEL_WIDTH - GUI_GROUP_BOX_WIDTH);
    gui_main_group_box_bounds[0].y = gui_main_group_box_bounds[0].x;

    for (int i = 1; i < GUI_MAIN_GROUP_BOX_COUNT; i++) {
        gui_main_group_box_bounds[i].x = gui_main_group_box_bounds[0].x;
        gui_main_group_box_bounds[i].y += gui_main_group_box_bounds[i - 1].y
            + (gui_main_group_box_bounds[i - 1].height + GUI_GROUP_BOX_PADDING);
    }
}

/* 편집 패널의 그룹 박스 위치를 설정한다. */
static void SetEditGroupBoxPositions(void) {
    gui_edit_group_box_bounds[0].x = 0.5f * (GUI_PANEL_WIDTH - GUI_GROUP_BOX_WIDTH);
    gui_edit_group_box_bounds[0].y = gui_edit_group_box_bounds[0].x;

    for (int i = 1; i < GUI_EDIT_GROUP_BOX_COUNT; i++) {
        gui_edit_group_box_bounds[i].x = gui_edit_group_box_bounds[0].x;
        gui_edit_group_box_bounds[i].y += gui_edit_group_box_bounds[i - 1].y
            + (gui_edit_group_box_bounds[i - 1].height + GUI_GROUP_BOX_PADDING);
    }
}

/* `메뉴` 그룹 박스의 요소 위치를 설정한다. */
static void SetMenuElementPositions(void) {
    gui_menu_element_bounds[0].x = gui_main_group_box_bounds[0].x 
        + (0.5f * (GUI_GROUP_BOX_WIDTH - GUI_MENU_ELEMENT_WIDTH));

    gui_menu_element_bounds[0].y = gui_main_group_box_bounds[0].y + GUI_MENU_ELEMENT_PADDING;

    for (int i = 1; i < GUI_MENU_ELEMENT_COUNT; i++) {
        gui_menu_element_bounds[i].x = gui_menu_element_bounds[0].x;
        gui_menu_element_bounds[i].y += gui_menu_element_bounds[i - 1].y
            + (gui_menu_element_bounds[i - 1].height + GUI_MENU_ELEMENT_PADDING);
    }
}

/* `캔버스` 그룹 박스의 요소 위치를 설정한다. */
static void SetCanvasElementPositions(void) {
    gui_canvas_element_bounds[0].x = gui_main_group_box_bounds[1].x 
        + (0.5f * (GUI_GROUP_BOX_WIDTH - GUI_CANVAS_ELEMENT_WIDTH));

    gui_canvas_element_bounds[0].y = gui_main_group_box_bounds[1].y + GUI_CANVAS_ELEMENT_PADDING;

    for (int i = 1; i < GUI_CANVAS_ELEMENT_COUNT; i++) {
        if (i & 1) {
            gui_canvas_element_bounds[i].x += gui_canvas_element_bounds[i - 1].width
                + (GUI_GROUP_BOX_WIDTH - GUI_CANVAS_ELEMENT_WIDTH);
            gui_canvas_element_bounds[i].y = gui_canvas_element_bounds[i - 1].y;
        } else {
            gui_canvas_element_bounds[i].x = gui_canvas_element_bounds[0].x;
            gui_canvas_element_bounds[i].y += gui_canvas_element_bounds[i - 1].y
                + (gui_canvas_element_bounds[i - 1].height + GUI_CANVAS_ELEMENT_PADDING);
        }
    }
}

/* `세계` 그룹 박스의 요소 위치를 설정한다. */
static void SetWorldElementPositions(void) {
    gui_world_element_bounds[0].x = gui_main_group_box_bounds[2].x 
        + (0.5f * (GUI_GROUP_BOX_WIDTH - GUI_WORLD_ELEMENT_WIDTH));

    gui_world_element_bounds[0].y = gui_main_group_box_bounds[2].y 
        + GUI_WORLD_ELEMENT_PADDING;

    for (int i = 1; i < GUI_WORLD_ELEMENT_COUNT; i++) {
        if (i == 2) {
            gui_world_element_bounds[i].x += gui_world_element_bounds[i - 1].width
                + (GUI_GROUP_BOX_WIDTH - GUI_WORLD_ELEMENT_WIDTH);
            gui_world_element_bounds[i].y = gui_world_element_bounds[i - 1].y;
        } else {
            gui_world_element_bounds[i].x = gui_world_element_bounds[0].x;
            gui_world_element_bounds[i].y += gui_world_element_bounds[i - 1].y
                + (gui_world_element_bounds[i - 1].height + GUI_WORLD_ELEMENT_PADDING);
        }
    }
}

/* `모양` 그룹 박스의 요소 위치를 설정한다. */
static void SetShapesElementPositions(void) {
    gui_shapes_element_bounds[0].x = gui_edit_group_box_bounds[0].x 
        + (0.5f * (GUI_GROUP_BOX_WIDTH - GUI_SHAPES_ELEMENT_WIDTH));

    gui_shapes_element_bounds[0].y = gui_edit_group_box_bounds[0].y + GUI_SHAPES_ELEMENT_PADDING;

    for (int i = 1; i < GUI_SHAPES_ELEMENT_COUNT; i++) {
        if (i & 1) {
            gui_shapes_element_bounds[i].x += gui_shapes_element_bounds[i - 1].width
                + (GUI_GROUP_BOX_WIDTH - GUI_SHAPES_ELEMENT_WIDTH);
            gui_shapes_element_bounds[i].y = gui_shapes_element_bounds[i - 1].y;
        } else {
            gui_shapes_element_bounds[i].x = gui_shapes_element_bounds[0].x;
            gui_shapes_element_bounds[i].y += gui_shapes_element_bounds[i - 1].y
                + (gui_shapes_element_bounds[i - 1].height + GUI_SHAPES_ELEMENT_PADDING);
        }
    }
}

/* `강체` 그룹 박스의 요소 위치를 설정한다. */
static void SetBodiesElementPositions(void) {
    gui_bodies_element_bounds[0].x = gui_edit_group_box_bounds[1].x 
        + (0.5f * (GUI_GROUP_BOX_WIDTH - GUI_BODIES_ELEMENT_WIDTH));

    gui_bodies_element_bounds[0].y = gui_edit_group_box_bounds[1].y + GUI_BODIES_ELEMENT_PADDING;

    for (int i = 1; i < GUI_BODIES_ELEMENT_COUNT; i++) {
        if (i & 1) {
            gui_bodies_element_bounds[i].x += gui_bodies_element_bounds[i - 1].width
                + (GUI_GROUP_BOX_WIDTH - GUI_BODIES_ELEMENT_WIDTH);
            gui_bodies_element_bounds[i].y = gui_bodies_element_bounds[i - 1].y;
        } else {
            gui_bodies_element_bounds[i].x = gui_bodies_element_bounds[0].x;
            gui_bodies_element_bounds[i].y += gui_bodies_element_bounds[i - 1].y
                + (gui_bodies_element_bounds[i - 1].height + GUI_BODIES_ELEMENT_PADDING);
        }
    }
}

/* `정보` 메시지 박스의 요소 위치를 설정한다. */
static void SetWindowBoxElementPositions(void) {
    gui_window_box_element_bounds[0].x = GUI_WINDOW_BOX_BOUNDS.x
        + (0.5f * (GUI_WINDOW_BOX_WIDTH - GUI_WINDOW_BOX_ELEMENT_WIDTH));

    gui_window_box_element_bounds[0].y = GUI_WINDOW_BOX_BOUNDS.y + (4.0f * GUI_WINDOW_BOX_ELEMENT_PADDING);

    for (int i = 1; i < GUI_WINDOW_BOX_ELEMENT_COUNT; i++) {
        gui_window_box_element_bounds[i].x = gui_window_box_element_bounds[0].x;
        gui_window_box_element_bounds[i].y += gui_window_box_element_bounds[i - 1].y
            + (gui_window_box_element_bounds[i - 1].height + GUI_WINDOW_BOX_ELEMENT_PADDING);
    }
}

/* `캔버스` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetCanvasTextValues(void) {
    int text_cursor = 0;

    for (int i = 0; i < GUI_CANVAS_ZOOM_COUNT; i++) {
        TextAppend(
            gui_canvas_zoom_dropdown_text,
            TextFormat(
                (i < (GUI_CANVAS_ZOOM_COUNT - 1)) ? "%d%%;" : "%d%%",
                (int) (100 * GUI_CANVAS_ZOOM_MULTIPLIERS[i])
            ),
            &text_cursor
        );
    }
}

/* `세계` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetWorldTextValues(void) {
    FloatToString(gui_world_gravity_multiplier, gui_world_gravity_multiplier_text);
}

/* `모양` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetShapesTextValues(void) {
    TextCopy(gui_shapes_type_dropdown_text, "CIRCLE;RECTANGLE;POLYGON");

    FloatToString(gui_shapes_material.density, gui_shapes_density_text);
    FloatToString(gui_shapes_material.restitution, gui_shapes_restitution_text);
    FloatToString(gui_shapes_material.static_friction, gui_shapes_static_friction_text);
    FloatToString(gui_shapes_material.dynamic_friction, gui_shapes_dynamic_friction_text);

    FloatToString(gui_shapes_circle_radius, gui_shapes_radius_text);
    FloatToString(gui_shapes_rec_dimensions.x, gui_shapes_width_text);
    FloatToString(gui_shapes_rec_dimensions.y, gui_shapes_height_text);
}

/* `강체` 그룹 박스의 텍스트 필드 값을 설정한다. */
static void SetBodiesTextValues(void) {
    TextCopy(gui_bodies_type_dropdown_text, "STATIC;KINEMATIC;DYNAMIC");

    FloatToString(gui_bodies_position.x, gui_bodies_position_x_text);
    FloatToString(gui_bodies_position.y, gui_bodies_position_y_text);
    FloatToString(gui_bodies_gravity_scale, gui_bodies_gravity_scale_text);
}

/* `열기` 메시지 상자를 그린다. */
static void ShowLoadMessageBox(void) {
    if (!gui_menu_load_message_box_visible) return;

    DrawRectangleRec(SCREEN_BOUNDS, Fade(RAYWHITE, 0.5f));

    int result = GuiMessageBox(
        GUI_MESSAGE_BOX_BOUNDS,
        "#1#Load from a file...",
        "This feature is not implemented yet.",
        "#159#Close"
    );

    if (result == 0 || result == 1)
        gui_menu_load_message_box_visible = false;
}

/* `저장` 메시지 상자를 그린다. */
static void ShowSaveMessageBox(void) {
    if (!gui_menu_save_message_box_visible) return;

    DrawRectangleRec(SCREEN_BOUNDS, Fade(RAYWHITE, 0.5f));

    int result = GuiMessageBox(
        GUI_MESSAGE_BOX_BOUNDS,
        "#2#Save to a file...",
        "This feature is not implemented yet.",
        "#159#Close"
    );

    if (result == 0 || result == 1)
        gui_menu_save_message_box_visible = false;
}
