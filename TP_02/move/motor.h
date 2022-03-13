#ifndef MOTOR_H
#define MOTOR_H

#define NOT_MOVING	0
#define MOVING		1

void motor_init(void);

void motor_stop(void);

void motor_set_position_right(float position_r, float speed_r);
void motor_set_position_left(float position_l, float speed_l);
void motor_set_position(float position_r, float position_l, float speed_r, float speed_l);

void motor_set_speed_right(float speed_r);
void motor_set_speed_left(float speed_l);
void motor_set_speed(float speed_r, float speed_l);

uint8_t motor_right_move(void); //return NOT_MOVING or MOVING
uint8_t motor_left_move(void);	//return NOT_MOVING or MOVING
uint8_t motors_move(void);		//return NOT_MOVING or MOVING

void robot_rotation(float speed, float angle, uint8_t turn_direction); //if turn_direction = 1 -> right, if = -1 -> left
void robot_rotation_180(void);

#endif /* MOTOR_H */
