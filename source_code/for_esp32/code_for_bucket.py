import machine
from machine import Pin, ADC,I2C,PWM,Timer,UART
import uasyncio as asyncio
import time,utime
import ssd1306
import network,neopixel

from config import (
    WIFI_SSID, WIFI_PASS,
    MQTT_BROKER, MQTT_USER, MQTT_PASS,
    TOPIC_PREFIX
)
from umqtt.simple import MQTTClient

led=Pin(48,Pin.OUT)
np = neopixel.NeoPixel(led,8)
np[0] = (0, 0, 0)
np.write()
TRIG_GPIO=1
RX_GPIO=44
IR_GPIO=8
SERVO_GPIO=5
RED_GPIO = 42
YELLOW_GPIO = 41
GREEN_GPIO = 40
TOPIC_LIGHT = f'{TOPIC_PREFIX}/light'
TOPIC_LED_RED = f'{TOPIC_PREFIX}/led/red'
TOPIC_LED_YELLOW = f'{TOPIC_PREFIX}/led/yellow'
TOPIC_LED_GREEN = f'{TOPIC_PREFIX}/led/green'
TOPIC_MYBUCKET=f'{TOPIC_PREFIX}/mybucket'
TOPIC_DWATER_LV=f'{TOPIC_PREFIX}/output/progress'
JUG_AREA=242
JUG_HEIGHT=14.5



def connect_wifi():
    mac = ':'.join(f'{b:02X}' for b in wifi.config('mac'))
    print(f'WiFi MAC address is {mac}')
    wifi.active(True)
    print(f'Connecting to WiFi {WIFI_SSID}.')
    wifi.connect(WIFI_SSID, WIFI_PASS)
    while not wifi.isconnected():
        print('.', end='')
        time.sleep(0.5)
    print('\nWiFi connected.')


def connect_mqtt():
    print(f'Connecting to MQTT broker at {MQTT_BROKER}.')
    mqtt.connect()
    mqtt.set_callback(mqtt_callback)
    mqtt.subscribe(TOPIC_LED_RED)
    mqtt.subscribe(TOPIC_LED_YELLOW)
    mqtt.subscribe(TOPIC_LED_GREEN)
    
    
    print('MQTT broker connected.')




def mqtt_callback(topic, payload):
    
    if topic.decode() == TOPIC_LED_RED:
        try:
            red.value(int(payload))
        except ValueError:
            pass
    elif topic.decode() == TOPIC_LED_YELLOW:
        try:
            yellow.value(int(payload))
        except ValueError:
            pass
    elif topic.decode() == TOPIC_LED_GREEN:
        try:
            green.value(int(payload))
        except ValueError:
  
   
        
        
        
def servo_angle(angle):
    v=180/1.9
    angle=0.5+angle/v
    v=angle/20
    return round(v*1024)           

def distance_sr05():
    while uart.read(1):
        pass
    trig.value(0)
    while uart.any() <= 4: 
        pass
    trig.value(1)  
    distanceValue = uart.read(4)

    if distanceValue[0] == 0xFF:
        distance = (distanceValue[1] << 8) | distanceValue[2]
        checksum = distanceValue[1] + distanceValue[2] - 1
        if distanceValue[3] == checksum:
            return distance
    return -1
    
def distance():
    a=0
    while True:
        a=distance_sr05()
        if a==-1:
            pass
        else:break
    return a
def chk_wl(wl):
    def ryg():
        def set1led(n):
            if n==0:
                red.value(1)
                yellow.value(0)
                green.value(0)
            elif n ==1:
                red.value(0)
                yellow.value(1)
                green.value(0)
            else:
                red.value(0)
                yellow.value(0)
                green.value(1)
                
                
        if wl>13 or wl<3:
            set1led(0)
        elif wl>6.5:
            set1led(1)
        else:set1led(2)
    if not wl:
        wl=distance()/10
    else:pass
    ryg()
    
def servo_start():
    myservo.duty(servo_angle(160))
def servo_stop():
    myservo.duty(servo_angle(10))
############
# setup
############

ir=Pin(8, Pin.IN, Pin.PULL_DOWN)
uart = UART(1, baudrate=9600, rx=RX_GPIO )  
trig=Pin(TRIG_GPIO, mode=machine.Pin.OUT, value=1)


display = ssd1306.SSD1306_I2C(128, 64, i2c)
red = Pin(RED_GPIO, Pin.OUT)
yellow = Pin(YELLOW_GPIO, Pin.OUT)
green = Pin(GREEN_GPIO, Pin.OUT)
wifi = network.WLAN(network.STA_IF)
mqtt = MQTTClient(client_id='',
                  server=MQTT_BROKER,
                  user=MQTT_USER,
                  password=MQTT_PASS)
connect_wifi()
connect_mqtt()
last_publish = 0

mqtt.publish(TOPIC_SW,str(0))

myservo = PWM(Pin(SERVO_GPIO, mode=Pin.OUT))
myservo.freq(50)
servo_stop()
led=Pin(48,Pin.OUT)

np.write()

class State:
    WAIT_IR = 0
    ULTRA_INIT = 1
    SERVO_START = 2
    SERVO_STOP = 3
    ULTRA_END = 4
class nnum:
    def __init__(self,r):
        self.n=r
        self.r=r
    def up(self):
        if self.n<self.r :
            self.n+=1
        else:self.n=0
    def down(self):
        if self.n >0:
            self.n-=1
        else:self.n=self.r
def ddey(func,timer,time,v):
    print(1)
    print(ir.value())
    def gg(param):
        if v == ir.value():
            func(None)
            
    timer.init(period=time, callback=gg,mode=Timer.ONE_SHOT)
# Initial state
current_state = nnum(4)
current_state2 = nnum(3)
############ event driven
cms0 = 0
wl=0
previous_day=utime.localtime()[2]
task1_timer = Timer(0)

def task1_start():
    task1_wait_ir(None)

def task1_wait_ir(param):
    global current_state
   
    if current_state.n==4:
        ir.irq(trigger=Pin.IRQ_FALLING, handler=delay_ir)
        
        
def delay_ir(param):
    task1_timer.init(period=500, callback=afterdelay,mode=Timer.ONE_SHOT)
def afterdelay(param):
    if ir.value()==0:
        current_state.up()
        init_ultra(None)
    else:task1_wait_ir(None)
        
def init_ultra(param):
    
    global current_state
    
    if current_state.n==0:
        global cms0
        current_state.up()
        ir.irq(handler=None)
        print("222",ir.value())
   
        

        cms0=distance()/10
        print("cms0:",cms0)
        print(10_)
        task1_servo_start(None)

def task1_servo_start(param):
    global current_state
    if current_state.n==1:
        
        servo_start()
        current_state.up()
        ir.irq(trigger=Pin.IRQ_RISING, handler=task1_servo_stop)
        

def task1_servo_stop(param):
    global current_state
    if current_state.n==2:
        ir.irq(trigger=0,handler=None)
        print(4)
        servo_stop()
        current_state.up()
        init_ultra1(None)

def init_ultra1(param):
    global current_state
    global wl
    if current_state.n==3:
        cms1=distance()/10
        chk_wl(cms1)
        print("cms1",cms1)
        dcms = cms1 - cms0
        print("dcms:",dcms)
        current_state.up()
        if dcms>0:
            wl+=dcms*JUG_AREA
            mqtt.publish(TOPIC_DWATER_LV, str(round(wl)))
        
        mqtt.publish(TOPIC_MYBUCKET, str((JUG_HEIGHT-cms1)*JUG_AREA))
        task1_wait_ir(None)
    
def chk_day():
    today=utime.localtime()[2]
    if previous_day != today:
        wl=0

      
        
#task2
task2_timer = Timer(1)
def task2():
    task2_timer.init(period=1000, callback=mqtt_chk)
def mqtt_chk(param):
    np.write()
    chk_day()
    #mqtt.check_msg()
    
chk_wl(None)
task1_start()
task2()
############

############
# loop
############

mqtt.publish(TOPIC_SW,str(0))
while True:
    time.sleep_ms(1000)
    

   


