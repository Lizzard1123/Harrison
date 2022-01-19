#include "custom/global.h"

void initialize() {
    RightLift.set_brake_mode(E_MOTOR_BRAKE_HOLD);
    LeftLift.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}
void disabled() {}
void competition_initialize() {}


double toRPM(bool reverse, double speed, int gear) {
  int check     = (reverse) ? -1 : 1;
  int gearRatio = 10;
  switch (gear) {
  case E_MOTOR_GEARSET_36:
    gearRatio = 100;
    break;
  case E_MOTOR_GEARSET_18:
    gearRatio = 200;
    break;
  case E_MOTOR_GEARSET_06:
    gearRatio = 600;
    break;
  }
  return check * (speed / 100) * gearRatio;
}


//AUTON

bool recording = false;
bool ready = false;
int dataSize = 0;
//skills
const int dataLength = 600;
const int segmentLength = 16;
double replayData[dataLength][segmentLength]; //16 should stay 3500 should be a lil lower and = to dataSize
/*
0 - LX joystick
1 - LY joystick
2 - RX joystick
3 - RY joystick

4 - upArrow (0 nothing, 1 trigger)
5 - rightArrow (0 nothing, 1 trigger)
6 - downArrow (0 nothing, 1 trigger)
7 - leftArrow (0 nothing, 1 trigger)

8 - X (0 nothing, 1 trigger)
9 - A (0 nothing, 1 trigger)
10 - B (0 nothing, 1 trigger)
11 - Y (0 nothing, 1 trigger)

12 - L1 (0 nothing, 1 trigger)
13 - L2 (0 nothing, 1 trigger)
14 - R1 (0 nothing, 1 trigger)
15 - R2 (0 nothing, 1 trigger)
*/

void fillEmpty(){ //set all to 0
    printf("attempting to fill array \n");
    if(!ready){
        for(int i = 0; i < dataLength; i++){
            for(int j = 0; j < segmentLength; j++){
                replayData[i][j] = 0;
            }
        }
    }
    ready = true;
}

void setData(int num, double val){
    if(recording && !ready){
        fillEmpty();
    } 
    if(recording){
        replayData[dataSize][num] = val;
    }
}

void printData(){
    printf("Printing Data\n");
    recording = false;
    printf("{");
    for(int i = 0; i < dataLength; i++){ //every segment
    printf("{");
        for(int j = 0; j < segmentLength; j++){ //every input
        if(j == 15){
            printf("%f", replayData[i][j]);
        } else {
            printf("%f,", replayData[i][j]);
        }
            delay(10);
        }
    printf("},\n");
    }
    printf("}\n");
}

void finalizeData(){
    if(recording){
        dataSize++;
        //printf("Filling data line: %f\n", dataSize);
        if(dataSize == dataLength){
            printData();
        }
    }
}

void runSegment(int line){

	int forward = replayData[line][3];
    int sideways = replayData[line][2];
    int Turn = replayData[line][0];

	FrontRight.move_velocity(toRPM(false, forward - sideways + Turn, FrontRight.get_gearing()));//.spin(vex::forward, forward - sideways + Turn, percent); // driver controls
    FrontLeft.move_velocity(toRPM(false, forward + sideways - Turn, FrontLeft.get_gearing())); //spin(vex::forward, forward + sideways - Turn, percent);
    BackRight1.move_velocity(toRPM(false, forward + sideways + Turn, BackRight1.get_gearing()));//.spin(vex::forward, forward + sideways + Turn, percent);
    BackLeft1.move_velocity(toRPM(false, forward - sideways - Turn, BackLeft1.get_gearing()));//.spin(vex::forward, forward - sideways - Turn, percent);
    BackRight2.move_velocity(toRPM(false, forward + sideways + Turn, BackRight2.get_gearing()));//.spin(vex::forward, forward + sideways + Turn, percent);
    BackLeft2.move_velocity(toRPM(false, forward - sideways - Turn, BackLeft2.get_gearing()));//.spin(vex::forward, forward - sideways - Turn, percent);

    if (replayData[line][4]) { // operate front arm
      RightLift.move_velocity(toRPM(false, 100, RightLift.get_gearing()));//.spin(vex::forward, 100, percent);
      LeftLift.move_velocity(toRPM(false, 100, LeftLift.get_gearing()));//.spin(vex::forward, 100, percent);
    } else if (replayData[line][6]) {
      RightLift.move_velocity(toRPM(true, 100, RightLift.get_gearing()));//.spin(vex::forward, 100, percent);
      LeftLift.move_velocity(toRPM(true, 100, LeftLift.get_gearing()));//.spin(vex::forward, 100, percent);
    } else {
      RightLift.move_velocity(0);
      LeftLift.move_velocity(0);
    }	
}

void executeSkillsData(){
    printf("Executing Skills Data");
    for(int i = 0; i < dataLength; i++){
       runSegment(i); //similate inputs 
       delay(100); // NEEDS to be the same as driver collected replayData
    }
}

bool isRecording(){
  return recording;
}

void setRecording(bool val){
  recording = val;
}

void autonomous() {
	executeSkillsData();
}

/*

END AUTON 

*/
 

void opcontrol() {
	while (true) {
		if(master.get_digital_new_press(DIGITAL_LEFT)){
            printf("Starting recording\n");
            setRecording(true);
            fillEmpty();
            for(int i = 5; i > 0; i--){
                master.print(1, 8, "start: " + i);
            }
            master.print(1, 8, "RECORDING");
        }
    	int forward = -(master.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y)) / 127; //-Controller1.Axis2.position(percent);
    	int sideways = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X) / 127; //Controller1.Axis1.position(percent);
    	int Turn = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X) / 127;

        setData(3, forward);
        setData(2, sideways);
        setData(0, Turn);

    	FrontRight.move_velocity(toRPM(false, forward - sideways + Turn, FrontRight.get_gearing()));//.spin(vex::forward, forward - sideways + Turn, percent); // driver controls
    	FrontLeft.move_velocity(toRPM(false, forward + sideways - Turn, FrontLeft.get_gearing())); //spin(vex::forward, forward + sideways - Turn, percent);
    	BackRight1.move_velocity(toRPM(false, forward + sideways + Turn, BackRight1.get_gearing()));//.spin(vex::forward, forward + sideways + Turn, percent);
    	BackLeft1.move_velocity(toRPM(false, forward - sideways - Turn, BackLeft1.get_gearing()));//.spin(vex::forward, forward - sideways - Turn, percent);
    	BackRight2.move_velocity(toRPM(false, forward + sideways + Turn, BackRight2.get_gearing()));//.spin(vex::forward, forward + sideways + Turn, percent);
    	BackLeft2.move_velocity(toRPM(false, forward - sideways - Turn, BackLeft2.get_gearing()));//.spin(vex::forward, forward - sideways - Turn, percent);

    	if (master.get_digital(DIGITAL_UP)) { // operate front arm
        	setData(4, 1);
    	    RightLift.move_velocity(toRPM(false, 100, RightLift.get_gearing()));//.spin(vex::forward, 100, percent);
    	    LeftLift.move_velocity(toRPM(false, 100, LeftLift.get_gearing()));//.spin(vex::forward, 100, percent);
    	} else if (master.get_digital(DIGITAL_DOWN)) {
        	setData(6, 1);
    	    RightLift.move_velocity(toRPM(true, 100, RightLift.get_gearing()));//.spin(vex::forward, 100, percent);
    	    LeftLift.move_velocity(toRPM(true, 100, LeftLift.get_gearing()));//.spin(vex::forward, 100, percent);
    	} else {
    	    RightLift.move_velocity(0);
    	    LeftLift.move_velocity(0);
    	}
		
        finalizeData();
		delay(100);
	}
}
