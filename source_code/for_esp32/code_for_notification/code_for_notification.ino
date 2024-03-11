#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "time.h"

#define RED_GPIO 42
#define YELLOW_GPIO 41
#define GREEN_GPIO 40
#define LDR_GPIO 4
#define SW_GPIO 2
#define BUZZER_GPIO 8
#define TOPIC_LIGHT TOPIC_PREFIX "/light"
#define TOPIC_LED_RED TOPIC_PREFIX "/led/red"
#define TOPIC_LED_YELLOW TOPIC_PREFIX "/led/yellow"
#define TOPIC_LED_GREEN TOPIC_PREFIX "/led/green"
#define TOPIC_SW TOPIC_PREFIX "/sw"
#define TOPIC_INPUT_AGE TOPIC_PREFIX "/input/age"
#define TOPIC_INPUT_SEX TOPIC_PREFIX "/input/sex"
#define TOPIC_INPUT_GOAL TOPIC_PREFIX "/input/goal"
#define TOPIC_INPUT_START TOPIC_PREFIX "/input/start"
#define TOPIC_INPUT_WATER_INCREASE TOPIC_PREFIX "/input/water/increase"
#define TOPIC_OUTPUT_PROGRESS TOPIC_PREFIX "/output/progress"
#define TOPIC_OUTPUT_PROGRESS_PERCENT TOPIC_PREFIX "/output/progress_percent"
#define TOPIC_OUTPUT_WATER_RECOM TOPIC_PREFIX "/output/water/recom"
#define TOPIC_OUTPUT_GOAL TOPIC_PREFIX "/output/goal"
#define TOPIC_NOTIFICATION TOPIC_PREFIX "/noti"
#define TOPIC_TIME TOPIC_PREFIX "/time/time"
#define TOPIC_DATE TOPIC_PREFIX "/time/date"
#define TOPIC_SW TOPIC_PREFIX "/sw"
#define TOPIC_NOTI_STOP TOPIC_PREFIX "/noti/stop"
#define TOPIC_RESULT_SUCCESS TOPIC_PREFIX "/result/success"
#define TOPIC_RESULT_FAILURE TOPIC_PREFIX "/result/failure"

WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;
uint32_t last_publish;
uint32_t last_publish_2;
uint8_t age_range;
bool is_male;
uint8_t old_age_range;
bool old_is_male;
uint16_t my_goal;
bool sw;
uint16_t my_progress;
uint16_t old_my_progress;
uint32_t next_notification;
struct tm next_notification_time;
bool in_mission;
uint32_t buzzer_last_buzzed;
bool buzzing;
uint16_t buzzer_duration;
bool can_press_water_increase;
bool goal_reached;

void connect_wifi() {
    printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
    printf("Connecting to WiFi %s.\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        printf(".");
        fflush(stdout);
        delay(500);
    }
    printf("\nWiFi connected.\n");
}

void connect_mqtt() {
    printf("Connecting to MQTT broker at %s.\n", MQTT_BROKER);
    if (!mqtt.connect("", MQTT_USER, MQTT_PASS)) {
        printf("Failed to connect to MQTT broker.\n");
        for (;;) {
        }
    }
    mqtt.setCallback(mqtt_callback);
    mqtt.subscribe(TOPIC_LED_RED);
    mqtt.subscribe(TOPIC_LED_YELLOW);
    mqtt.subscribe(TOPIC_LED_GREEN);
    mqtt.subscribe(TOPIC_INPUT_AGE);
    mqtt.subscribe(TOPIC_INPUT_SEX);
    mqtt.subscribe(TOPIC_INPUT_GOAL);
    mqtt.subscribe(TOPIC_INPUT_WATER_INCREASE);
    mqtt.subscribe(TOPIC_INPUT_START);
    printf("MQTT broker connected.\n");
}

void next_time(bool hourly = false);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, TOPIC_NOTI_STOP) == 0) {
        sw = true;
    }
    if (strcmp(topic, TOPIC_INPUT_AGE) == 0) {
        payload[length] = 0;
        age_range = (uint8_t)((atoi((char*)payload)));
    }
    if (strcmp(topic, TOPIC_INPUT_SEX) == 0) {
        payload[length] = 0;
        is_male = (bool)(atoi((char*)payload));  // 0=>female, 1=>male
    }
    if (strcmp(topic, TOPIC_INPUT_GOAL) == 0) {
        payload[length] = 0;
        my_goal = (uint16_t)(atoi((char*)payload));
    }
    if (strcmp(topic, TOPIC_INPUT_WATER_INCREASE) == 0) {
        increase_water();
    }
    if (strcmp(topic, TOPIC_INPUT_START) == 0) {
        if (in_mission) {
            return;
        }
        in_mission = true;
        next_time();
    }
}

void increase_water() {
    if (!can_press_water_increase) {
        return;
    }
    if (my_progress + 240 >= my_goal) {
        my_progress = my_goal;
    } else {
        my_progress += 240;
    }
    can_press_water_increase = false;
}

void next_time(bool hourly) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    double diff;
    if (hourly) {
        update_next_hour();

        diff = 3600.0;
    } else {
        struct tm next_timeinfo;
        next_timeinfo = timeinfo;

        round_to_halves(next_timeinfo);
        next_notification_time = next_timeinfo;

        time_t t2 = mktime(&next_notification_time);
        time_t t1 = mktime(&timeinfo);

        diff = difftime(t2, t1);
    }

    uint32_t now = millis();
    next_notification = now + (uint32_t)(diff * 1000);
    last_publish_2 = now + 10000;
}

char* recommended_amount(uint8_t age_range, bool is_male) {
    switch (age_range) {
        case 0:
            return "5 cups (~1200 ml)";
        case 1:
            return "7-8 cups (~1800 ml)";
        case 2:
            return "8-11 cups (~2280 ml)";
        case 3:
            if (is_male) {
                return "13+ cups (~3120ml)";
            }
            return "9+ cups (~2160 ml)";
        default:
            return "";
    }
}

void round_to_halves(struct tm& time) {
    time.tm_sec = 0;
    if (time.tm_min >= 30) {
        time.tm_hour++;
        time.tm_min = 0;
    } else {
        time.tm_min = 30;
    }
}

void update_next_hour() { next_notification_time.tm_hour += 1; }

bool is_during_break(struct tm& time) {
    int tm_hour = time.tm_hour;
    return (tm_hour <= 8) || (11 <= tm_hour && tm_hour <= 13) ||
           (18 <= tm_hour);
}

void start_buzzer() {
    buzzing = true;
    digitalWrite(BUZZER_GPIO, 1);
}

void stop_buzzer() {
    buzzing = false;
    digitalWrite(BUZZER_GPIO, 0);
}

void reset_mission() {
    last_publish = 0;
    last_publish_2 = 0;
    sw = false;
    old_age_range = 0;
    old_is_male = false;
    my_progress = 0;
    old_my_progress = 0;
    my_goal = 0;
    in_mission = false;
    can_press_water_increase = false;
    next_notification = 0;
    buzzer_last_buzzed = 0;
    buzzing = false;
    buzzer_duration = 0;
    goal_reached = false;
}

void setup() {
    Serial.begin(115200);
    pinMode(SW_GPIO, INPUT_PULLDOWN);
    pinMode(BUZZER_GPIO, OUTPUT);
    connect_wifi();
    connect_mqtt();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    reset_mission();
    age_range = 0;
    is_male = false;
}

void loop() {
    mqtt.loop();
    if (sw) {
        stop_buzzer();
        increase_water();
        sw = false;
    }

    if (in_mission && my_progress >= my_goal && !goal_reached) {
        mqtt.publish(TOPIC_RESULT_SUCCESS,
                     "Congratulations, you have successfully achieved your "
                     "goal for today!");
        goal_reached = true;
    }

    if (old_age_range != age_range || old_is_male != is_male) {
        char* water_recom = recommended_amount(age_range, is_male);
        mqtt.publish(TOPIC_OUTPUT_WATER_RECOM, water_recom);
        old_age_range = age_range;
        old_is_male = is_male;
    }
    if (old_my_progress != my_progress) {
        mqtt.publish(TOPIC_OUTPUT_PROGRESS, String(my_progress).c_str());
        mqtt.publish(
            TOPIC_OUTPUT_PROGRESS_PERCENT,
            String(((double)my_progress / (double)my_goal) * 100).c_str());
        old_my_progress = my_progress;
    }

    uint32_t now = millis();
    if (now - last_publish >= 20000) {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }

        if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
            if (in_mission) {
                if (my_progress >= my_goal) {
                    mqtt.publish(
                        TOPIC_RESULT_SUCCESS,
                        "Congratulations, you have successfully achieved "
                        "your goal for yesterday!");
                } else {
                    mqtt.publish(
                        TOPIC_RESULT_FAILURE,
                        "You have failed to achieve your goal for yesterday.");
                }
            }

            reset_mission();
        }

        if (is_during_break(timeinfo)) {
            buzzer_duration = 2000;
        } else {
            buzzer_duration = 10000;
        }

        char time_now[6];
        strftime(time_now, 6, "%R", &timeinfo);
        mqtt.publish(TOPIC_TIME, time_now);
        char time_date[11];
        strftime(time_date, 11, "%d/%m/%Y", &timeinfo);
        mqtt.publish(TOPIC_DATE, time_date);
        last_publish = now;
    }

    int32_t diff = ((int32_t)next_notification - (int32_t)now) / 1000;
    if (diff <= 0 && in_mission) {
        if (!buzzing) {
            buzzer_last_buzzed = now;
            can_press_water_increase = true;
            next_time(true);
            start_buzzer();
        }
    }
    if (now - buzzer_last_buzzed >= 600000) {
        can_press_water_increase = false;
    }
    if (buzzing && now - buzzer_last_buzzed >= buzzer_duration) {
        stop_buzzer();
    }
    if (now - last_publish_2 >= 10000) {
        char time_diff[6];
        if (in_mission) {
            int minutes = diff / 60;
            int seconds = diff - (minutes * 60);

            sprintf(time_diff, "%02d:%02d", minutes, seconds);
        } else {
            strncpy(time_diff, "--:--", sizeof(time_diff) - 1);
            time_diff[sizeof(time_diff) - 1] = '\0';
        }
        mqtt.publish(TOPIC_NOTIFICATION, time_diff);
        last_publish_2 = now;
    }

    int s = digitalRead(SW_GPIO);
    if (s) {
        sw = true;
    }
}
