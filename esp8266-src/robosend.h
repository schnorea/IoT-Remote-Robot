#ifndef robosend_h
#define robosend_h

void robo_send(u_char robo_cmd);
void robo_callback();
void robo_setup();
void print_info();

wifi.setPins(8,7,4,2);
Serial.begin(115200);

#endif
