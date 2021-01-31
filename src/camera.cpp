#include "camera.h"
#include "block.h"
#include "bx/math.h"
#include "world.h"
#include <SDL2/SDL.h>

#include <iostream>

extern World world;

Camera::Camera()
{
    position = glm::vec3(3.0f, 15.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    scale = 0.01f;
    ys = 0.0f;
    width = 640;
    height = 480;
    worldup = glm::vec3(0.0f, 1.0f, 0.0f);
    this->front = glm::vec3(0.0f, 0.0f, 1.0f);
    this->movement_speed = 0.1f;
    this->mouse_sensitivity = 0.1f;
}

void Camera::update_camera_position(float deltaTime)
{
    // WIP: only work in one chunk
    Chunk& chunk = world.get_chunk(position.x, position.y, position.z);
    if (chunk
            .blocks[int(position.x) % 16][int(position.y) % 16 - 1]
                   [int(position.z) % 16]
            .type == "air") {
        ys -= 0.1;
    }
    if (chunk.blocks[int(position.x) % 16][int(position.y) % 16 - 1]
                    [int(position.z) % 16]
                        .type != "air" &&
        ys <= 0) {
        ys = 0;
    }
    position.y += ys * deltaTime * movement_speed;
    return;
}

void Camera::view()
{

    float view[16];
    bx::Vec3 Position = {position.x, position.y, position.z};
    glm::vec3 tmp = position + front;
    bx::Vec3 Tmp = {tmp.x, tmp.y, tmp.z};
    bx::Vec3 Up = {up.x, up.y, up.z};

    bx::mtxLookAt(view, Position, Tmp, Up);

    float proj[16];
    bx::mtxProj(
        proj, 60.0f, width / height, 0.1f, 100.0f,
        bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);
    return;
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
    Chunk& chunk = world.get_chunk(position.x, position.y, position.z);
    glm::vec3 lastpos = position;
    float velocity = movement_speed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position += right * velocity;
    if (direction == RIGHT)
        position -= right * velocity;
    position.y = lastpos.y;
    if (direction == JUMP && ys <= 0)
        if (chunk
                .blocks[int(position.x) % 16][int(position.y) % 16 - 1]
                       [int(position.z) % 16]
                .type != "air")
            ys += 1;

    if (chunk
            .blocks[int(position.x) % 16][int(position.y) % 16]
                   [int(position.z) % 16]
            .type != "air") {
        position = lastpos;
    }
    return;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::update_camera_vectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(
        this->front, worldup)); // normalize the vectors, because their length
                                // gets closer to 0 the more you look up or down
                                // which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
    return;
}

void Camera::process_mouse_movement(float xoffset, float yoffset)
{
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw -= xoffset;
    pitch += yoffset;
    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors();
    return;
}