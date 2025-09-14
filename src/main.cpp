#include "main.h"
#include "pros/misc.hpp"
#include <cmath>
#include <string>

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {

	// pros::MotorGroup left_mg({-5, -8, -9, -10});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	// pros::MotorGroup right_mg({1, 2, 3, 4});

	// left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	// pros::lcd::set_text(1, std::to_string(left_mg.get_actual_velocity()));

	// left_mg.move_velocity(75); //Go straight
	// right_mg.move_velocity(75);
	// pros::delay(2000);

	// left_mg.move_velocity(0); //Stop
	// right_mg.move_velocity(0);
	// pros::delay(1000);

	// left_mg.move_velocity(75); // Turn left
	// right_mg.move_velocity(-75);
	// pros::delay(3000);

	// left_mg.move_velocity(0); // Stop
	// right_mg.move_velocity(0);
	// pros::delay(1000);
}

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::MotorGroup left_mg({-5, -8, -9, -10});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::MotorGroup right_mg({1, 2, 3, 4});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6
	pros::Motor arm(15);

	left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	int t = 3; // Amount of time to reach full speed
	int count = 0; // Current step
	int steps = 100; // Number of increments within 't' seconds 
	int minCount = 0.15*steps; // CHANGE THIS WHEN NEEDED
	count = minCount;
	int delay_per_step = (t * 1000) / steps; // Number of milliseconds within each step

	while (true) {

		// SPATULA (ARM) //

		bool up = master.get_digital(DIGITAL_R1);
		bool down = master.get_digital(DIGITAL_L1);
		int armSpeed = 60;

		if(up){
			arm.move(armSpeed);
		} else if (down) {
			arm.move(-armSpeed);
		}

		// DRIVING //
		
		int dy = master.get_analog(ANALOG_LEFT_Y);
		int dx = master.get_analog(ANALOG_RIGHT_X);

		if(dy != 0 || dx != 0){
			if(count < steps){
				count++; //Increment 
			}
			double factor = count / (double)steps; 

			left_mg.move(static_cast<int>((dy*factor)-(dx*factor))); // Move the output
			right_mg.move(static_cast<int>((dy*factor)+(dx*factor)));

		} else {
        	// Reset when joystick released
        	count = minCount;
        	left_mg.move(0);
        	right_mg.move(0);
		}


		pros::delay(delay_per_step); // Delays to prevent 

	
	}
}