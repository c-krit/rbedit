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
#include "stb_ds.h"

/* | `physics` 모듈 상수... | */

const float DELTA_TIME = (1.0f / TARGET_FPS) * 100.0f;

const float RING_INNER_RADIUS = 16.0f;

/* | `physics` 모듈 변수... | */

static frWorld *world = NULL;
static frBody **bodies = NULL, **temp_bodies = NULL;

static PhysicsState state = PHYSICS_STATE_PAUSED;

static float line_thick = 1.0f;

/* | `physics` 모듈 함수... | */

/* 주어진 개체의 꼭짓점을 그린다. */
static void DrawPolygonVertices(frBody *body);

/* 주어진 개체의 AABB 색상을 반환한다. */
static Color GetAABBColor(frBody *body);

/* 게임 세계를 재시작한다. */
static void RestartPhysicsWorld(void);

/* 게임 세계를 초기화한다. */
void InitPhysicsWorld(void) {
    const Rectangle bounds = GetCanvasBounds();

    world = frCreateWorld(
        frVec2ScalarMultiply(FR_WORLD_DEFAULT_GRAVITY, 0.0f),
        frRecPixelsToMeters(
            (Rectangle) { 
                .width = bounds.width, 
                .height = bounds.height 
            }
        )
    );

    arrsetcap(bodies, FR_WORLD_MAX_BODY_COUNT);
    arrsetcap(temp_bodies, FR_WORLD_MAX_BODY_COUNT);
}

/* 게임 세계에 할당된 메모리를 해제한다. */
void UninitPhysicsWorld(void) {
    frReleaseWorld(world);

    for (int i = 0; i < arrlen(temp_bodies); i++)
        frReleaseBody(temp_bodies[i]);

    for (int i = 0; i < arrlen(bodies); i++)
        frReleaseBody(bodies[i]);

    arrfree(temp_bodies);
    arrfree(bodies);
}

/* 게임 세계에 새로운 개체를 추가한다. */
void AddPhysicsObjectToWorld(frBody *body, ObjectData data) {
    ObjectData *new_data = RL_CALLOC(1, sizeof(ObjectData));
    
    new_data->id = arrlen(bodies);
    new_data->color = data.color;
    new_data->no_fill = data.no_fill;

    frSetBodyUserData(body, new_data);

    frAddToWorld(world, body);

    arrput(bodies, ClonePhysicsObject(body));
}

/* 게임 세계에서 고유 번호가 `id`인 개체를 제거한다. */
void RemovePhysicsObjectFromWorld(unsigned int id) {
    if (id < 0 || id > arrlen(bodies) - 1) return;

    frBody *body = frGetWorldBody(world, id);

    RL_FREE(frGetBodyUserData(body));
    
    frRemoveFromWorld(world, body);
    frReleaseBody(body);

    arrdel(bodies, id);

    for (int i = id; i < arrlen(bodies); i++) {
        ObjectData *data = frGetBodyUserData(bodies[i]);

        if (data != NULL) {
            data->id = i;
            
            frSetBodyUserData(bodies[i], data);
        }
    }
}

/* 게임 세계에서 고유 번호가 `id`인 개체의 정보를 업데이트한다. */
void UpdatePhysicsObjectInWorld(unsigned int id, frBody *src) {
    if (id < 0 || id > arrlen(bodies) - 1) return;

    frBody *dst = bodies[id];
    frShape *dst_shape = frGetBodyShape(dst);

    frAttachShapeToBody(dst, frCloneShape(frGetBodyShape(src)));
    frReleaseShape(dst_shape);

    frSetBodyPosition(dst, frGetBodyPosition(src));
    frSetBodyRotation(dst, frGetBodyRotation(src));
    frSetBodyGravityScale(dst, frGetBodyGravityScale(src));

    ObjectData *src_data = frGetBodyUserData(src);
    ObjectData *dst_data = frGetBodyUserData(dst);

    dst_data->color = src_data->color;
    dst_data->no_fill = src_data->no_fill;
}

/* 주어진 개체의 복사본을 생성한다. */ 
frBody *ClonePhysicsObject(frBody *body) {
    frBody *result = frCreateBodyFromShape(
        frGetBodyType(body),
        frGetBodyFlags(body),
        frGetBodyPosition(body),
        frCloneShape(frGetBodyShape(body))
    );

    frSetBodyRotation(result, frGetBodyRotation(body));
    frSetBodyGravityScale(result, frGetBodyGravityScale(body));
    frSetBodyUserData(result, frGetBodyUserData(body));

    return result;
}

/* 게임 세계에서 좌표 `position`을 포함하는 개체를 반환한다. */
frBody *GetPhysicsObjectAt(Vector2 position) {
    int count = frQueryWorldSpatialHash(
        world,
        frRecPixelsToMeters(
            (Rectangle) {
                position.x - 0.5f,
                position.y - 0.5f,
                1.0f,
                1.0f
            }
        ),
        temp_bodies
    );

    int j = -1;

    for (int i = 0; i < count; i++) {
        if (frShapeContainsPoint(
            frGetBodyShape(temp_bodies[i]),
            frGetBodyTransform(temp_bodies[i]),
            frVec2PixelsToMeters(position)
        )) { if (j < i) j = i; }
    }

    return (j >= 0) ? temp_bodies[j] : NULL;
}

/* 게임 세계의 메모리 주소를 반환한다. */
frWorld *GetPhysicsWorld(void) {
    return world;
}

/* 게임 세계의 현재 상태를 반환한다. */
PhysicsState GetPhysicsWorldState(void) {
    return state;
}

/* 게임 세계의 현재 상태를 나타내는 문자열을 반환한다. */
const char *GetPhysicsWorldStateText(void) {
    if (state == PHYSICS_STATE_PAUSED) return "PAUSED";
    else if (state == PHYSICS_STATE_RUNNING) return "RUNNING";
    else if (state == PHYSICS_STATE_STEPPING) return "STEPPING";
    else if (state == PHYSICS_STATE_RESTARTING) return "RESTARTING";
    else return "UNKNOWN";
}

/* 게임 세계의 중력 가속률을 `value`로 설정한다. */
void SetPhysicsGravityMultiplier(float value) {
    frSetWorldGravity(world, frVec2ScalarMultiply(FR_WORLD_DEFAULT_GRAVITY, value));
}

/* 게임 세계의 경계 범위를 `bounds`로 설정한다. */
void SetPhysicsWorldBounds(Rectangle bounds) {
    frSetWorldBounds(world, frRecPixelsToMeters(bounds));
}

/* 게임 세계의 현재 상태를 `state`로 설정한다. */
void SetPhysicsWorldState(PhysicsState value) {
    state = value;
}

/* 게임 세계의 시간을 흐르게 한다. */
void SimulatePhysicsWorld(void) {
    if (state != PHYSICS_STATE_PAUSED)
        frSimulateWorld(world, DELTA_TIME);

    if (state == PHYSICS_STATE_STEPPING) state = PHYSICS_STATE_PAUSED;
    else if (state == PHYSICS_STATE_RESTARTING) RestartPhysicsWorld();
}

/* 현재 화면에 게임 세계를 그린다. */
void DrawPhysicsWorld(void) {
    ClearBackground(RAYWHITE);

    line_thick = (GetCanvasZoomMultiplier() > 0.25f) ? 1.0f : 3.0f;

    if (IsCanvasGridVisible())
        frDrawSpatialHash(frGetWorldSpatialHash(world), line_thick, GRAY);

    for (int i = 0; i < frGetWorldBodyCount(world); i++) {
        frBody *body = frGetWorldBody(world, i);
        
        ObjectData *data = (ObjectData *) frGetBodyUserData(body);

        if (data == NULL) return;

        if (!data->no_fill) frDrawBody(body, data->color);
        else frDrawBodyLines(body, line_thick, data->color);
    }

    if (IsCanvasEditPanelVisible()) {
        frBody *body = GetSelectedPhysicsObject();
        frBodyType type = frGetBodyType(body);

        ObjectData *data = (ObjectData *) frGetBodyUserData(body);

        if (data == NULL) return;

        float new_line_thick = 2.0f * line_thick;

        if (!data->no_fill) frDrawBody(body, Fade(data->color, 0.5f));
        else frDrawBodyLines(body, new_line_thick, Fade(data->color, 0.5f));
        
        frDrawBodyAABB(body, new_line_thick, GetAABBColor(body));

        DrawPolygonVertices(body);
    }

    DrawFPS(8, 8);
}

/* 주어진 개체의 꼭짓점을 그린다. */
static void DrawPolygonVertices(frBody *body) {
    if (frGetBodyType(body) != FR_SHAPE_POLYGON
        || frIsShapeRectangle(frGetBodyShape(body))) return;

    frShape *shape = frGetBodyShape(body);

    frVertices vertices = frGetPolygonVertices(shape);
    frTransform tx = frGetBodyTransform(body);

    Vector2 position = GetCanvasMousePosition();

    const float radius = 3.0f * line_thick;

    for (int i = 0; i < vertices.count; i++) {
        Vector2 vertex = frVec2Transform(vertices.data[i], tx);

        if (CheckCollisionPointCircle(
            GetCanvasMousePosition(), 
            frVec2MetersToPixels(vertex), 
            RING_INNER_RADIUS
        )) {
            DrawTextEx(
                GetFontDefault(),
                TextFormat("(%.2f, %.2f)", vertex.x, vertex.y),
                frVec2Add(frVec2MetersToPixels(vertex), (Vector2) { 12.0f, -24.0f }),
                20,
                1,
                GetAABBColor(body)
            );

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 delta = frVec2PixelsToMeters(GetMouseDelta());

                vertex = frVec2Add(vertex, delta);
                vertices.data[i] = frVec2Add(vertices.data[i], delta);

                frSetPolygonVertices(shape, vertices);
            }
        }

        DrawRing(
            frVec2MetersToPixels(vertex),
            RING_INNER_RADIUS, 
            RING_INNER_RADIUS - radius,
            0.0f, 
            360.0f, 
            FR_DEBUG_CIRCLE_SEGMENT_COUNT,
            GetAABBColor(body)
        );
    }
}

/* 주어진 개체의 AABB 색상을 반환한다. */
static Color GetAABBColor(frBody *body) {
    frBodyType type = frGetBodyType(body);

    return (type == FR_BODY_DYNAMIC) 
        ? Fade(LIME, 0.5f)
        : ((type == FR_BODY_KINEMATIC) ? Fade(GOLD, 0.5f) : Fade(DARKGRAY, 0.5f));
}

/* 게임 세계를 재시작한다. */
static void RestartPhysicsWorld() {
    for (int i = 0; i < frGetWorldBodyCount(world); i++) {
        frBody *body = frGetWorldBody(world, i);

        frSetBodyTransform(body, frGetBodyTransform(bodies[i]));
        frSetBodyVelocity(body, frGetBodyVelocity(bodies[i]));
        frSetBodyAngularVelocity(body, frGetBodyAngularVelocity(bodies[i]));

        frClearBodyForces(body);
    }
    
    state = PHYSICS_STATE_PAUSED;
}