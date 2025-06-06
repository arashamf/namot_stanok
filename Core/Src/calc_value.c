
// Includes ---------------------------------------------------------------------------------------//
#include "calc_value.h"

// Functions --------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------//
 uint16_t calc_rotation_speed (uint16_t value_rotation_speed)
{
	uint16_t pulse_frequency = 0;
	pulse_frequency = value_rotation_speed*PULSE_IN_TURN/60;
	return pulse_frequency;
}