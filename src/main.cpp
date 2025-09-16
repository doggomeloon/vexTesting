#include "main.h"
#include "pros/misc.hpp"
#include <cmath>
#include <string>

// Default functions
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

void disabled() {}

void competition_initialize() {}


// Autonomous Function
void autonomous() {

	pros::MotorGroup left_mg({-5, -8, -9, -10}); // Motors 5, 8, 9, and 10 are on the left side but need to be reversed
	pros::MotorGroup right_mg({1, 2, 3, 4}); // Motors 1, 2, 3, 4 are on the right side

	// left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // This may need to be removed later but Hannah and I like it
	// right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // It could damage motors

	left_mg.move_velocity(75); //Go straight
	right_mg.move_velocity(75);
	pros::delay(2000);

	left_mg.move_velocity(0); //Stop
	right_mg.move_velocity(0);
	pros::delay(1000);

	left_mg.move_velocity(75); // Turn left
	right_mg.move_velocity(-75);
	pros::delay(3000);

	left_mg.move_velocity(0); // Stop
	right_mg.move_velocity(0);
	pros::delay(1000);
}

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER); // Needed for controller input
	pros::MotorGroup left_mg({11, 13, 14, 15});
	pros::MotorGroup right_mg({-1, -2, -3, -4}); 
	pros::MotorGroup arm({-9, 10}); 

	// I always seem to forget this so this is how you print strings to the robot:
	// pros::lcd::set_text(1, std::to_string(integer_here));

	// left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	int t = 3; // The amount of seconds to reach joystick's max speed
	int count = 0; // Current step, this should remain 0
	
	// Number of increments within 't' seconds that the robot switches speeds
	int steps = 100; // Higher numbers will take require more memory but are more accurate

	// The percent of the joystick speed that the robot will not go below
	int minCount = 0.15*steps; // Change the decimal to whatever is needed
	count = minCount;

	int delay_per_step = (t * 1000) / steps; // Number of milliseconds in-between each step

	while (true) {

		// SPATULA (ARM) //

		bool up = master.get_digital(DIGITAL_R1); // Get input from triggers
		bool down = master.get_digital(DIGITAL_L1);
		int armSpeed = 20; // Speed that the arm moves

		// Arm program
		if(up){ 
			arm.move(armSpeed); 
		} else if (down) {
			arm.move(-armSpeed);
		} else {
			arm.move(0); // If no button pressed, then stop arm motion
		}

		// DRIVING //
		
		int dy = master.get_analog(ANALOG_LEFT_Y); // Get input from joysticks
		int dx = master.get_analog(ANALOG_RIGHT_X);

		if(dy != 0 || dx != 0){ // If there is robot movement
			if(count < steps){
				count++; //Increment count
			}
			double factor = count / (double)steps; // 1/100th of speed, 2/100th of speed, etc.

			left_mg.move(static_cast<int>(factor*(dy+dx)));  // Straight movement - Turning movement, all multiplied by factor
			right_mg.move(static_cast<int>(factor*(dy-dx)));

		} else {
        	// Reset when joystick released
        	count = minCount; // Sets minimum
        	left_mg.move(0);
        	right_mg.move(0);
		}


		pros::delay(delay_per_step); // Delays to prevent 
	}
}