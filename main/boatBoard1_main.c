/* Hello World Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "max11612Jonas.h"
#include "esp_wifi.h"
#include "jonas_proj.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "apps/sntp/sntp.h"
#include "apps/sntp/sntp_opts.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "mqtt_client.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"


#define u16_t uint16_t

//PROTOTYPES where I think i needed them
void post_variables_task(void *);
void mqtt_app_start(void);
//GLOBALS........

esp_mqtt_client_handle_t client; //client for mqtt messages
static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;
extern int public_heap; // project variables
extern double public_tempC;
time_t now;
unsigned int esp32ID; //for my chipid
char char_esp32ID[10]; //string for chipid //string for chipid
EventGroupHandle_t  group_task_watchdog;
/*wifi_config_t sta_config = { .sta =
		{ .ssid = "vodafone-doing",
		.password =	"wallyb26",
		.bssid_set = false
		}*/
wifi_config_t sta_config = { .sta =
		{ .ssid = "fatherjack",
		.password =	"batmanN9",
		.bssid_set = false
		}
	};



enum task_shift {
	blink_task_shift, bmp180_shift, post_variables_shift
};
//END GLOBALS

void getMacID(void) {
	uint8_t chipid[6];
	esp_efuse_mac_get_default(chipid);

	for (int var = 0; var < sizeof(chipid); ++var) {
		printf("%02x   ", chipid[var]);
	}
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", chipid[0], chipid[1], chipid[2],
			chipid[3], chipid[4], chipid[5]);
	//transfer 3 lowest bytes of chipid array of chars to an int esp32ID by bit shifting each char element
	for (int var = 5, varShift = 0; var > 2 && var < 6; var--, varShift++) {
		esp32ID = esp32ID | (chipid[var] << (varShift * 8));//esp32ID = esp32ID | (chipid[var] <<(8 * varShift));
	}
	printf("sizeof esp32id %d   ,", sizeof(esp32ID));
	printf("\n\nhex value:%08x, decimal value:%d    \n", esp32ID, esp32ID);
printf("esp32id is %u\n", esp32ID);
printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^                                 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
sprintf(char_esp32ID,"%u",esp32ID); //convert int array to string array
				}  //close getmacid function


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
        	xTaskCreate(&post_variables_task, "post_variables_task", 2048, NULL, 2,	NULL);
        	flash_rate = connectedMQTT;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
           // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

           // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
           // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

          //  msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
          //  ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
        	flash_rate = connectingMQTT;

            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
          //  msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
           // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}





void mqtt_app_start(void)
{
	const esp_mqtt_client_config_t mqtt_cfg= {
        .event_handle = mqtt_event_handler,
		.host = "192.168.1.71",
					#if defined(CONFIG_MQTT_SECURITY_ON)
							.port = 8883, // encrypted
					#else
							//.port = 1883, // unencrypted
							.port = 10002, // unencrypted
					#endif
							//.client_id = "(const char)esp32ID",
							.username = "user",
							.password = "pass",
							.disable_clean_session = 0, .keepalive = 120, .lwt_topic = "/lwt", .lwt_msg =
									"offline_GONE_GONE_GONE", .lwt_retain = 0,
        // .user_context = (void *)your_context
    };

	memcpy(mqtt_cfg.client_id, char_esp32ID, 7);
	client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}


//set up sntp ####################################
static void initialize_sntp(void) {
	ESP_LOGI(TAG, "Initializing SNTP");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}
// end set up sntp##################################

void watchdog_task(void *pvParameter) {
	/* Get flags from all tasks to say alive then kick the dog
	 * task watchdog timeout is 5s- set in menuconfig near bottom of esp specific section
	 */
	//esp_task_wdt_feed();
	while (1) {
		//esp_task_wdt_feed();




		vTaskDelay(3 * 1000 / portTICK_PERIOD_MS); //check every 10 secs

	}
}
void blink_task(void *pvParameter) {
	/* Configure the IOMUX register for pad BLINK_GPIO (some pads are
	 muxed to GPIO on reset already, but some default to other
	 functions and need to be switched to GPIO. Consult the
	 Technical Reference for a list of pads and their default
	 functions.)
	 */
	gpio_pad_select_gpio(BLINK_GPIO);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	while (1) {
		/* Blink off (output low) */
		gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay(250 / portTICK_PERIOD_MS);
		/* Blink on (output high) */
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay(flash_rate / portTICK_PERIOD_MS);

	}
}

void post_variables_task(void *pvParameter) {

	while (1) {

		//if (Flag_wifi_connected) {
		xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
		printf("########################################\n");
			time(&now);
			// INFO("[jonas]  test publish msg\n");
			int heap, timestamp;
			heap = 456;
			public_heap = system_get_free_heap_size();

			if (public_heap)
				heap = public_heap;
			float volts;
			volts = 12.7;
			if (public_tempC)
				volts = public_tempC;
			char message[100];
			char message2[100];
			int len = 3;


			timestamp = 1477606545;
			if (now > 12000)
				timestamp = now;

			len = sprintf(message,
					"{\"HEAP\":%d,\"Volts\":%f,\"timestamp\":%ld}", heap, volts,
					(long) now); //json
			//have to escape the " ; e.g \"

			//must publish to "/mcu/14056893/heap,volts,stamp/" pahoListenPostSQLLAN.jar in the toshiba lappy home/
			//directory is only subscribed to that- currently
			printf("message=%s; message length=%d\n", message, len);
			//get ready to publish.....
			sprintf(message2,"/mcu/%d/heap,volts,stamp/", esp32ID); //put mac id from device as device id
			printf("message2 = %s", message2);
			esp_mqtt_client_publish(client, message2,	message, len, 0, 0);


		//} //close if wifi connected
		vTaskDelay(1 * 60 * 1000 / portTICK_PERIOD_MS);
	}//close while(1)
}
void vTimerCallback(TimerHandle_t xTimer) {

	ESP_LOGI(TAG,"the freertos timer in MAIN has been kicked \n                          ========\n");
}
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            flash_rate = connectingMQTT;

            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            flash_rate = connecting;
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void wifi_init(void)
{
    tcpip_adapter_init();
    flash_rate = connecting;
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "fatherJack",
            .password = "batmanN9",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", "fatherjack", "******");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Waiting for wifi");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}


void app_main() {
	nvs_flash_init();
	//esp_task_wdt_init();
	//esp_int_wdt_init(); // initiate watchdog- set in menuconfig 300ms timeout Task w'dog is 5s
	// set up wifi ##############################################
	xTaskCreate(&blink_task, "blink_task", 512, NULL, 7, NULL);
	wifi_init();
	mqtt_app_start();
	initialize_sntp();
	getMacID();
	//MY timer lest start line
	TimerHandle_t MainTestTimer = xTimerCreate("Timer", 3 * 1000 / portTICK_PERIOD_MS, pdTRUE,
	/* The ID is used to store a count of the
	 number of times the timer has expired, which
	 is initialised to 0. */
	(void *) 0,
	/* Each timer calls the same callback when
	 it expires. */
	vTimerCallback);
	if (MainTestTimer == NULL) {
		printf(" The timer was not created.");
	}
	//End of test timer setup
	//xTimerStart(MainTestTimer,0);// start that timer
	init_max11612Jonas(); //initialise the offboard ADC


	xTaskCreate(&watchdog_task, "watchdog_task", 2048, NULL, 8, NULL);
	xTaskCreate(&task_max11612Jonas, "bmp180_task", 2048, NULL, 1, NULL);
	}
