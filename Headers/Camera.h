#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>


#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROLL = -90.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;
bool isOrbiting = false;
bool safeToRotate = false;
bool dontRotate = false;
bool wideViewMode = false;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position, Position_init, Position_orbit;
    glm::vec3 Front, Front_init;
    glm::vec3 Up, Up_orbit;
    glm::vec3 Right, Right_orbit;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;
    float Yaw2;
    float Pitch2;
    float Roll2;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;


    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        Position_init = position;
        Front_init = Front;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Roll = ROLL;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        Position_init = glm::vec3(posX, posY, posZ);
        Front_init = Front;
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    bool permissionToMove() {
        return !dontRotate;
    }

    void wideView() {
        if (!safeToRotate) {
            Front_init = Front;
            Position_init = Position;
            safeToRotate = true;
        }
        wideViewMode = true;
        Position.z = Position_init.z + 6.0f;
        Position.y = Position_init.y + 0.6f;
        Position.x = Position_init.x + 0.5f;
    }

    void saveVectors()
    {
        if (!safeToRotate) {
            Position_init = Position;
            Front_init = Front;
            safeToRotate = true;
        }
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (dontRotate) return;
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            Position += Front * velocity;
            if (!safeToRotate) {
                Front_init = Front;
                Position_init = Position;
            }
            if (wideViewMode) {
                Position_init += Front_init * velocity;
            }
        }

        if (direction == BACKWARD) {
            Position -= Front * velocity;
            if (!safeToRotate) {
                Front_init = Front;
                Position_init = Position;
            }
            if (wideViewMode) {
                Position_init -= Front_init * velocity;
            }
        }

        if (direction == LEFT) {
            Position -= Right * velocity;
            if (!safeToRotate) {
                Front_init = Front;
                Position_init = Position;
            }
            if (wideViewMode) {
                Position_init -= Right * velocity;
            }
        }

        if (direction == RIGHT) {
            Position += Right * velocity;
            if (!safeToRotate) {
                Front_init = Front;
                Position_init = Position;
            }
            if (wideViewMode) {
                Position_init += Right * velocity;
            }
        }

        if (direction == UP)
            Position.y += 1.0f * velocity;
        if (direction == DOWN)
            Position.y -= 1.0f * velocity;



    }

    void RotateAroundAxis(int axis, float angle)
    {
        dontRotate = true;
        float factor = 3.0f;
        if (!safeToRotate) {
            Position_init = Position;
            Front_init = Front;
            safeToRotate = true;
        }
        glm::vec3 front;
        // along x-axis - pitch
        if (axis == 1) {
            Pitch2 += (angle * MouseSensitivity * factor);
            Yaw2 = YAW;
            Roll2 = ROLL;
        }
        else if (axis == 2) {
            Roll2 += (angle * MouseSensitivity * factor);
            Pitch2 = PITCH;
            Yaw2 = YAW;
        }
        else if (axis == 3) {
            Roll2 += (angle * MouseSensitivity * factor);
            Pitch2 += (angle * MouseSensitivity * factor);
            Yaw2 += (angle * MouseSensitivity * factor);
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;

        }
        front.x = -cos(glm::radians(Yaw2)) * sin(glm::radians(Pitch2)) * sin(glm::radians(Roll2)) - sin(glm::radians(Yaw2)) * cos(glm::radians(Roll2));
        front.y = -sin(glm::radians(Yaw2)) * sin(glm::radians(Pitch2)) * sin(glm::radians(Roll2)) + cos(glm::radians(Yaw2)) * cos(glm::radians(Roll2));
        front.z = cos(glm::radians(Pitch2)) * sin(glm::radians(Roll2));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void RotateInverseAroundAxis(int axis, float angle)
    {
        glm::vec3 front;
        // along x-axis - pitch
        if (axis == 1) {
            Pitch2 -= (angle * MouseSensitivity);
            Yaw2 = YAW;
            Roll2 = ROLL;
        }
        else if (axis == 2) {
            Roll2 -= (angle * MouseSensitivity);
            Pitch2 = PITCH;
            Yaw2 = YAW;
        }
        else if (axis == 3) {
            Roll2 -= (angle * MouseSensitivity);
            Pitch2 -= (angle * MouseSensitivity);
            Yaw2 -= (angle * MouseSensitivity);
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        front.x = -cos(glm::radians(Yaw2)) * sin(glm::radians(Pitch2)) * sin(glm::radians(Roll2)) - sin(glm::radians(Yaw2)) * cos(glm::radians(Roll2));
        front.y = -sin(glm::radians(Yaw2)) * sin(glm::radians(Pitch2)) * sin(glm::radians(Roll2)) + cos(glm::radians(Yaw2)) * cos(glm::radians(Roll2));
        front.z = cos(glm::radians(Pitch2)) * sin(glm::radians(Roll2));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void Orbit()
    {
        // glm::vec3 center = glm::vec3(Front.x, Front.y - 0.1f, Front.z);
        // float displacement = center.x * Front.x + center.y * Front.y + center.z * Front.z;
        // displacement *= cos(glm::radians(10.0f));
        //// Front.x *= sin(glm::radians(10.0f * MouseSensitivity));
        // //Front.z *= cos(glm::radians(10.0f * MouseSensitivity));
        // 
        // displacement = center.x * Position.x + center.y * Position.y + center.z * Position.z;
        // displacement *= cos(glm::radians(10.0f));
        // Position.x *= sin(glm::radians(10.0f * MouseSensitivity));
        // Position.z *= cos(glm::radians(10.0f * MouseSensitivity));

        // Right = glm::normalize(glm::cross(Front, WorldUp)); 
        // Up = glm::normalize(glm::cross(Right, Front));

    }

    void ResetPosition()
    {
        isOrbiting = false;
        Position = Position_init;
        // Position.z = zVal + 3.0f;
        Front = Front_init;
        // Front.z = zVal+1.0f;
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
        updateCameraVectors();
        safeToRotate = false;
        dontRotate = false;
        wideViewMode = false;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        Yaw2 = Yaw;
        Pitch2 = Pitch;
        Roll2 = Roll;
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
        if (!safeToRotate) {
            Front_init = Front;
            Position_init = Position;
        }
    }
};
#endif