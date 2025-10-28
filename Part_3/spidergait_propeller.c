#include "simpletools.h" 
#include "fdserial.h"  

#define FALSE 0
#define TRUE !(FALSE)         

//                      1A 2A 3A 4A 5A  6A  1B  2B  3B  4B  5B  6B
unsigned int pin[12] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
float servo_angle[12];
int servo_PWM[12];

// Walking Variables
float walk_height = 90;
float lift_height = 45;
float swing = 20;

float leg_offset_S[6] = {30, 0, -30, 30, 0, -30};
int swing_sign_SF[6] = {1, 1, -1, 1, -1, -1};
int swing_sign_SB[6] = {-1, -1, 1, -1, 1, 1};

float leg_offset_L[6] = {-30, 30, 0, -30, 30, 0};
int swing_sign_LF[6] = {-1, 1, 1, -1, 1, -1};
int swing_sign_LB[6] = {1, -1, -1, 1, -1, 1};

float leg_offset_R[6] = {0, -30, 30, 0, -30, 30};
int swing_sign_RF[6] = {1, -1, 1, -1, -1, 1};
int swing_sign_RB[6] = {-1, 1, -1, 1, 1, -1};

// Serial
fdserial *ESP;
int sequence = 0;

// Forward Declarations
void reset_servo_angles(void);
void squat_sequence(void);
void kill(void);
void walking_sequence(int facing);

// Cog Routines & Stacks

void set_servo_1AB(void *par);
void set_servo_2AB(void *par);
void set_servo_3AB(void *par);
void set_servo_4AB(void *par);
void set_servo_5AB(void *par);
void set_servo_6AB(void *par);

unsigned int servo_stack_1[256];
unsigned int servo_stack_2[256];
unsigned int servo_stack_3[256];
unsigned int servo_stack_4[256];
unsigned int servo_stack_5[256];
unsigned int servo_stack_6[256];

// MAIN

int main(){
  
// Start Subroutines
  cogstart(&set_servo_1AB, NULL, servo_stack_1, sizeof(servo_stack_1));
  cogstart(&set_servo_2AB, NULL, servo_stack_2, sizeof(servo_stack_2));
  cogstart(&set_servo_3AB, NULL, servo_stack_3, sizeof(servo_stack_3));
  cogstart(&set_servo_4AB, NULL, servo_stack_4, sizeof(servo_stack_4));
  cogstart(&set_servo_5AB, NULL, servo_stack_5, sizeof(servo_stack_5));
  cogstart(&set_servo_6AB, NULL, servo_stack_6, sizeof(servo_stack_6));
 
// Reset Positions
  reset_servo_angles(); 
  
// Game Loop
 int T = 0;
 int cap = 8; 
 
 pause(500);
 
 while(T < cap){
   walking_sequence(1);
   T++;
 }  
 
 pause(350);
 
 while(T < cap){
   walking_sequence(0);
   T++;
 } 
 
 pause(350);
 
 while(T < cap){
   squat_sequence();
   T++;
 }
 
 kill();    
}   

//  Servo Sequences
void set_leg_position(int leg_number, float theta_1, float theta_2){
  servo_angle[leg_number] = theta_2;
  servo_angle[leg_number + 6] = theta_1;   
}  

void reset_servo_angles(void){
  for (int i = 0; i < 12; i++){
    servo_angle[i] = 90;
  }    
} 

void walking_sequence(int facing){
  int t = 350;
  
  float leg_offset[6];
  int swing_sign[6];
  switch (facing){
    case 0:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_L[i];
        swing_sign[i] = swing_sign_LF[i]; 
      }        
      break;
    case 1:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_S[i];
        swing_sign[i] = swing_sign_SF[i]; 
      }
      break;
    case 2:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_R[i];
        swing_sign[i] = swing_sign_RF[i]; 
      }
      break;
    case 3:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_L[i];
        swing_sign[i] = swing_sign_LB[i]; 
      }
      break;
    case 4:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_S[i];
        swing_sign[i] = swing_sign_SB[i]; 
      }
      break;
    case 5:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_R[i];
        swing_sign[i] = swing_sign_RB[i]; 
      }
      break;
    default:
      for (int i = 0; i<6; i++){
        leg_offset[i] = leg_offset_S[i];
        swing_sign[i] = swing_sign_RF[i]; 
      }
  }  
  
  //
  for (int i = 0; i < 3; i++){
    int j = 2*i;
    set_leg_position(j, 90 - 0.5*swing_sign[j]*swing + leg_offset[j], lift_height);     
  }
  pause(t);
  
  for (int i = 0; i < 3; i++){
    int j = 2*i + 1;
    set_leg_position(j, 90 + leg_offset[j], walk_height);      
  }
  pause(t);  
  
  //
  for (int i = 0; i < 3; i++){
    int j = 2*i;
    set_leg_position(j, 90 - swing_sign[j]*swing + leg_offset[j], walk_height);     
  }
  pause(t);
  
  for (int i = 0; i < 3; i++){
    int j = 2*i + 1;
    set_leg_position(j, 90 - 0.5*swing_sign[j]*swing + leg_offset[j], lift_height);   
  }
  pause(t);
  
  //
  for (int i = 0; i < 3; i++){
    int j = 2*i;
    set_leg_position(j, 90 + leg_offset[j], walk_height);     
  }
  pause(t);
  
  for (int i = 0; i < 3; i++){
    int j = 2*i + 1;
    set_leg_position(j, 90 - swing_sign[j]*swing + leg_offset[j], walk_height);   
  }
  pause(t);  
} 

void squat_sequence(){
  int t = 250;
  pause(t);
  for (int i = 0; i < 6; i++){
    servo_angle[i] = 120;
  }
  pause(t);
  for (int i = 0; i < 6; i++){
    servo_angle[i] = 60;
  }   
}  

void kill(){
  while(TRUE){
    for (int i = 0; i < 6; i++){
      set_leg_position(i, 90, 50);
    } 
  }       
}  

//  Servo PWM subroutines   

void set_servo_1AB(void *par){
	while(TRUE){
    pulse_out(pin[0], 400 + (190*servo_angle[0])/18);
    pulse_out(pin[6], 400 + (190*servo_angle[6])/18);
    pause(20);
	}	
}	

void set_servo_2AB(void *par){
	while(TRUE){
    pulse_out(pin[1], 400 + (190*servo_angle[1])/18);
    pulse_out(pin[7], 400 + (190*servo_angle[7])/18);
    pause(20);
	}	
}

void set_servo_3AB(void *par){
	while(TRUE){
    pulse_out(pin[2], 400 + (190*servo_angle[2])/18);
    pulse_out(pin[8], 400 + (190*servo_angle[8])/18);
    pause(20);
	}	
}

void set_servo_4AB(void *par){
	while(TRUE){
    pulse_out(pin[3], 400 + (190*servo_angle[3])/18);
    pulse_out(pin[9], 400 + (190*servo_angle[9])/18); // Opposite for some reason?
    pause(20);
	}	
}

void set_servo_5AB(void *par){
	while(TRUE){
    pulse_out(pin[4], 400 + (190*servo_angle[4])/18);
    pulse_out(pin[10], 400 + (190*servo_angle[10])/18);
    pause(20);
	}	
}

void set_servo_6AB(void *par){
	while(TRUE){
    pulse_out(pin[5], 400 + (190*servo_angle[5])/18);
    pulse_out(pin[11], 400 + (190*servo_angle[11])/18);
    pause(20);
	}	
}  