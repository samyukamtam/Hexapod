#include "simpletools.h"      
#include "mx2125.h"           
#include "adcDCpropab.h"      

int gesture_to_final[36] = {
  8, -1, -1, -1, 1, -1, -1, -1, -1, -1, 
  6, -1, -1, 5, -1, -1, 7, -1, -1, 3, 
  -1, -1, 2, -1, -1, 4, -1, -1, -1, -1, 
  9, -1, -1, -1, -1
};

int main()
{
  pause(1000);  
  adc_init(21, 20, 19, 18); 
  
  while(1)
  {
    int y_acc = mx_accel(1);  
    int x_acc = mx_accel(0);  

    int x_tilt = 1; 
    if (x_acc < 4000) x_tilt = 0; 
    else if (x_acc > 5000) x_tilt = 2; 

    int y_tilt = 1; 
    if (y_acc < 4000) y_tilt = 0; 
    else if (y_acc > 5000) y_tilt = 2; 

    float flex1 = adc_volts(2);
    float flex2 = adc_volts(3);

    int f1_state = (flex1 < 1.0) ? 0 : 1;
    int f2_state = (flex2 < 1.0) ? 0 : 1;

    int gesture_code = f1_state * 18 + f2_state * 9 + x_tilt * 3 + y_tilt;
    int final_val = gesture_to_final[gesture_code];

    print("%cGesture Code: %d => Final Value: %d%c\n", HOME, gesture_code, final_val, CLREOL);
    pause(300);  
  }
}
