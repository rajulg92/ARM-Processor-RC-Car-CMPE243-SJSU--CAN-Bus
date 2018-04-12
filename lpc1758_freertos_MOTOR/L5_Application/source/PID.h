#ifndef PID_H
#define PID_H

#include <stdbool.h>
#include <stdint.h>


enum pid_control_directions {
    E_PID_DIRECT,
    E_PID_REVERSE,
};

struct pid_controller {
    // Input, output and setpoint
    float * input; //!< Current Process Value
    float * output; //!< Corrective Output from PID Controller
    float * setpoint; //!< Controller Setpoint
    // Tuning parameters
    float Kp; //!< Stores the gain for the Proportional term
    float Ki; //!< Stores the gain for the Integral term
    float Kd; //!< Stores the gain for the Derivative term
    // Output minimum and maximum values
    float omin; //!< Maximum value allowed at the output
    float omax; //!< Minimum value allowed at the output
    // Variables for PID algorithm
    float iterm; //!< Accumulator for integral term
    float lastin; //!< Last input value for differential term
    // Time related
    uint32_t lasttime; //!< Stores the time when the control loop ran last time
    uint32_t sampletime; //!< Defines the PID sample time
    // Operation mode
    uint8_t automode; //!< Defines if the PID controller is enabled or disabled
    enum pid_control_directions direction;
};

typedef struct pid_controller * pid_t;

#ifdef  __cplusplus
extern "C" {
#endif

pid_t pid_create(pid_t pid, float* in, float* out, float* set, float kp, float ki, float kd);
bool pid_need_compute(pid_t pid);
void pid_compute(pid_t pid);
void pid_tune(pid_t pid, float kp, float ki, float kd);
void pid_sample(pid_t pid, uint32_t time);
void pid_limits(pid_t pid, float min, float max);
void pid_auto(pid_t pid);
void pid_manual(pid_t pid);
void pid_direction(pid_t pid, enum pid_control_directions dir);

#ifdef  __cplusplus
}
#endif

#endif
