#include "Encoder.h"
#include "zf_qtimer.h"
#include "Motor.h"

void Encoder_init()
{
  qtimer_quad_init(LEFT_QTIMER, LEFT_ENCODER_PHASE_A, LEFT_ENCODER_PHASE_B);
  qtimer_quad_init(RIGHT_QTIMER, RIGHT_ENCODER_PHASE_A, RIGHT_ENCODER_PHASE_B);
}

void speed_get()
{
  motor.speed_act_left = -qtimer_quad_get(LEFT_QTIMER, LEFT_ENCODER_PHASE_A) * QTIME*5;
  motor.speed_act_right = qtimer_quad_get(RIGHT_QTIMER, RIGHT_ENCODER_PHASE_A) * QTIME*5;
  
  qtimer_quad_clear(LEFT_QTIMER, LEFT_ENCODER_PHASE_A);
  qtimer_quad_clear(RIGHT_QTIMER, RIGHT_ENCODER_PHASE_A);
}