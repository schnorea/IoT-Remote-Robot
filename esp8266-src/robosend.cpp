#include <ESP8266WiFi.h>
#include "robosend.h"

#define LED_PIN D2

#define TIMER1_WRITE_VALUE 867

bool isLEDOn = false;

#define PIN_HIGH false
#define PIN_LOW true

#define START_WAIT 520
#define ONE_WAIT 260
#define ZERO_WAIT 65
#define BIT_WAIT 65

#define TX_IDLE 0x00
#define TX_START 0x80
#define TX_START_BIT 0x40
#define TX_BIT_HIGH 0x20
#define TX_BIT_LOW 0x10

volatile u_char tx_state = TX_IDLE;
volatile u_char tx_byte = 0;
volatile u_char tx_mask = 0x80;
volatile int tx_count = 0x00;

bool tx_pin_state = PIN_HIGH;

void tx_send(u_char byte_to_send)
{
    while (tx_state != TX_IDLE);
    tx_byte = byte_to_send;
    tx_mask = 0x80;
    tx_count = START_WAIT;
    tx_state = TX_START;
}

void ICACHE_RAM_ATTR tx_start_bit()
{
    tx_pin_state = PIN_HIGH;
    if (!tx_mask)
    {
        tx_state = TX_IDLE;
        return;
    }
    if (tx_byte & tx_mask) {
        tx_count = ONE_WAIT;
    }
    else {
        tx_count = ZERO_WAIT;
    }
    tx_state = TX_BIT_HIGH;
}

void ICACHE_RAM_ATTR tx_bit()
{
    tx_count -= 1;
    if (tx_state == TX_BIT_HIGH && tx_count < 0)
    {
        tx_pin_state = PIN_LOW;
        tx_count = BIT_WAIT;
        tx_state = TX_BIT_LOW;
    }
    if (tx_state == TX_BIT_LOW && tx_count < 0)
    {
        tx_pin_state = PIN_HIGH;
        tx_mask >>=1; 
        tx_state = TX_START_BIT;
    }
}

void ICACHE_RAM_ATTR tx_start() 
{
    tx_pin_state = PIN_LOW;
    tx_count -= 1;
    if (tx_count < 0)
    {
        tx_state = TX_START_BIT;
    }
}

void ICACHE_RAM_ATTR tx_state_machine()
{
    if (tx_state == TX_IDLE)
    {
      
    }
    if (tx_state == TX_START)
    {
        tx_start();
    }
    if (tx_state == TX_START_BIT)
    {
        tx_start_bit();
    }
    if (tx_state == TX_BIT_HIGH || tx_state == TX_BIT_LOW)
    {
        tx_bit();
    } 
}

void robo_send(u_char robo_cmd)
{
    u_char cmd_check = robo_cmd;
    cmd_check >>= 4;
    if (cmd_check == 0x08 || cmd_check == 0x09 || cmd_check == 0x0A || 
        cmd_check == 0x0B || cmd_check == 0x0C || cmd_check == 0x0D )
    {
        tx_send(robo_cmd);
    }
    else 
    {
        Serial.println("Fail, command has to be in the range of 0x80 - 0xD0");
    }
    Serial.println(robo_cmd);
}

void robo_callback()
{    
    isLEDOn = !isLEDOn;
    tx_state_machine();
}

void ICACHE_RAM_ATTR onTimerISR(){
    // Interrupt service routine
    digitalWrite(LED_PIN, isLEDOn && tx_pin_state);

    isLEDOn = !isLEDOn;
    tx_state_machine();
    timer1_write(TIMER1_WRITE_VALUE); 
}

void robo_setup()
{
    pinMode(LED_PIN, OUTPUT);
    timer1_attachInterrupt(onTimerISR);
    timer1_write(TIMER1_WRITE_VALUE); 
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
}

void print_info()
{
    Serial.print("ESP.getBootMode(); ");
    Serial.println(ESP.getBootMode());
    Serial.print("ESP.getSdkVersion(); ");
    Serial.println(ESP.getSdkVersion());
    Serial.print("ESP.getBootVersion(); ");
    Serial.println(ESP.getBootVersion());
    Serial.print("ESP.getChipId(); ");
    Serial.println(ESP.getChipId());
    Serial.print("ESP.getFlashChipSize(); ");
    Serial.println(ESP.getFlashChipSize());
    Serial.print("ESP.getFlashChipRealSize(); ");
    Serial.println(ESP.getFlashChipRealSize());
    Serial.print("ESP.getFlashChipSizeByChipId(); ");
    Serial.println(ESP.getFlashChipSizeByChipId());
    Serial.print("ESP.getFlashChipId(); ");
    Serial.println(ESP.getFlashChipId());
    Serial.print("ESP.getFreeHeap(); ");
    Serial.println(ESP.getFreeHeap());
}
