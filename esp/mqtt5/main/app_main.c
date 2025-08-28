/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp32s3/rom/crc.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "mqtt_client.h"
#include "freertos/task.h"

#include <fmt_esp_spi.h>
#include <fmt_esp_uart.h>
#include <pb.h>
#include <firment_msg.pb.h>


#define MAX_PAYLOAD_BYTES 64
#define MQTT_BUFFER_SIZE (100 * MAX_PAYLOAD_BYTES)
// #define START_KEY 0xABCDEF00
#define MSG_TIMEOUT_MS 2000
#define MQTT_PUB_PERIOD_US 500000U

static enum fmtTransport_e {
  FMT_UNKNOWN,
  FMT_SPI,
  FMT_UART,
} fmtTransport = FMT_UNKNOWN;

static uint8_t mqttBuffer[MQTT_BUFFER_SIZE];
static uint32_t mqttWritePos = 0;
static uint32_t lastResetTimeUs = 0;

static const char *TAG = "mqtt5_example";

static bool unpackAndSendToEdge(uint8_t *packed, int len);
static size_t appendCRC(uint8_t *packet);
static bool sendToActiveTransport(uint8_t *packet, size_t packetSize);
enum fmtTransport_e checkForTransportChange(esp_err_t *rxStatus, uint8_t *rxPacket);


static void log_error_if_nonzero(const char *message, int error_code)
{
  if (error_code != 0)
  {
    ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
  }
}

static esp_mqtt_client_handle_t client;

static esp_mqtt5_user_property_item_t user_property_arr[] = {
    {"board", "esp32"},
    {"u", "user"},
    {"p", "password"}};

#define USE_PROPERTY_ARR_SIZE sizeof(user_property_arr) / sizeof(esp_mqtt5_user_property_item_t)

static esp_mqtt5_publish_property_config_t publish_property = {
    .payload_format_indicator = 1,
    .message_expiry_interval = 1000,
    .topic_alias = 0,
    .response_topic = "/topic/test/response",
    .correlation_data = "123456",
    .correlation_data_len = 6,
};

static esp_mqtt5_subscribe_property_config_t subscribe_property = {
    .subscribe_id = 25555,
    .no_local_flag = false,
    .retain_as_published_flag = false,
    .retain_handle = 0,
    .is_share_subscribe = true,
    .share_name = "group1",
};

// static esp_mqtt5_subscribe_property_config_t subscribe1_property = {
//     .subscribe_id = 25555,
//     .no_local_flag = true,
//     .retain_as_published_flag = false,
//     .retain_handle = 0,
// };

static esp_mqtt5_unsubscribe_property_config_t unsubscribe_property = {
    .is_share_subscribe = true,
    .share_name = "group1",
};

static esp_mqtt5_disconnect_property_config_t disconnect_property = {
    .session_expiry_interval = 60,
    .disconnect_reason = 0,
};

static void print_user_property(mqtt5_user_property_handle_t user_property)
{
  if (user_property)
  {
    uint8_t count = esp_mqtt5_client_get_user_property_count(user_property);
    if (count)
    {
      esp_mqtt5_user_property_item_t *item = malloc(count * sizeof(esp_mqtt5_user_property_item_t));
      if (esp_mqtt5_client_get_user_property(user_property, item, &count) == ESP_OK)
      {
        for (int i = 0; i < count; i++)
        {
          esp_mqtt5_user_property_item_t *t = &item[i];
          ESP_LOGI(TAG, "property %d: (key,val) %s, %s", i, t->key, t->value);
          free((char *)t->key);
          free((char *)t->value);
        }
      }
      free(item);
    }
  }
}

static void print_data(esp_mqtt_event_t *event)
{
  // print_user_property(event->property->user_property);
  // ESP_LOGI(TAG, "payload_format_indicator is %d", event->property->payload_format_indicator);
  // ESP_LOGI(TAG, "response_topic is %.*s", event->property->response_topic_len, event->property->response_topic);
  // ESP_LOGI(TAG, "correlation_data is %.*s", event->property->correlation_data_len, event->property->correlation_data);
  // ESP_LOGI(TAG, "content_type is %.*s", event->property->content_type_len, event->property->content_type);
  // ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
  // ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
  char *transportMsg[] = {
    [FMT_UNKNOWN] = "UNKNOWN",
    [FMT_SPI] = "SPI", 
    [FMT_UART] = "UART", 
  };
  ESP_LOGI(TAG, "Edge-bound via %s, len=%d Dat=%02x %02x %02x %02x",
          transportMsg[fmtTransport],
           event->data_len,
           event->data[0], event->data[1], event->data[2], event->data[3]);
}

static void setup_users(esp_mqtt_client_handle_t client)
{
  esp_mqtt5_client_set_user_property(&publish_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
  esp_mqtt5_client_set_user_property(&subscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
  esp_mqtt5_client_set_user_property(&unsubscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
  esp_mqtt5_client_set_user_property(&disconnect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);

  esp_mqtt5_client_set_publish_property(client, &publish_property);
  esp_mqtt5_client_set_subscribe_property(client, &subscribe_property);
  esp_mqtt5_client_set_unsubscribe_property(client, &unsubscribe_property);
  esp_mqtt5_client_set_disconnect_property(client, &disconnect_property);
}

static void delete_users(void)
{
  esp_mqtt5_client_delete_user_property(publish_property.user_property);
  esp_mqtt5_client_delete_user_property(subscribe_property.user_property);
  esp_mqtt5_client_delete_user_property(unsubscribe_property.user_property);
  esp_mqtt5_client_delete_user_property(disconnect_property.user_property);
  publish_property.user_property = NULL;
  subscribe_property.user_property = NULL;
  unsubscribe_property.user_property = NULL;
  disconnect_property.user_property = NULL;
}

static int subscribe(esp_mqtt_client_handle_t client, char *topic, int qos)
{
  int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
  ESP_LOGI(TAG, "requested subscribe to '%s'", topic);
  return msg_id;
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;

  ESP_LOGD(TAG, "free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
  switch ((esp_mqtt_event_id_t)event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    setup_users(client);
    print_user_property(event->property->user_property);
    subscribe(client, "edge-bound", 0);
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "DISCONNECTED");
    delete_users();
    break;
  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "SUBSCRIBED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "UNSUBSCRIBED, msg_id=%d", event->msg_id);
    // esp_mqtt_client_disconnect(client);
    break;
  case MQTT_EVENT_PUBLISHED:
    // ESP_LOGI(TAG, "PUBLISHED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_DATA:
    print_data(event);
    unpackAndSendToEdge((uint8_t *)(event->data), event->data_len);
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR, return code %d", event->error_handle->connect_return_code);
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
    {
      log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
      log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
      log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
      ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
    }
    break;
  default:
    ESP_LOGI(TAG, "Other event id:%d", event->event_id);
    break;
  }
}

static bool unpackAndSendToEdge(uint8_t *packed, int len)
{
  bool success = true;
  size_t packetSize;
  uint8_t packet[MAX_PACKET_SIZE_BYTES];

  // needs revision if max message length set > 255.
  int msgPos = 0;
  uint8_t msgSize = packed[msgPos + LENGTH_POSITION] + LENGTH_SIZE_BYTES;

  while (
      msgPos + msgSize <= len && // prevent reading past buffer.
      msgSize > 1 &&             // quit if we encounter 0-length msg.
      success)                             // quit if send buffer fills up.
  {
    memcpy(packet, &packed[msgPos], msgSize);
    packetSize = appendCRC(packet);
    success = sendToActiveTransport(packet, packetSize);
    msgPos += msgSize;
    msgSize = packed[msgPos + LENGTH_POSITION] + LENGTH_SIZE_BYTES;
  }
  return success;
}

/** appendCRC
 * @brief calculates and appends a 16-bit CRC to the provided buffer.
 * @return the finished packet size, with CRC appended.
 */
static size_t appendCRC(uint8_t *packet)
{
  // Size must be a multiple of 2 (for 16-bit CRC).  Pad with byte if needed.
    uint32_t crcPosition = getCRCPosition(packet);
    uint16_t crc = crc16_le(0x00, packet, crcPosition);
    *(uint16_t *)(&packet[crcPosition]) = crc;
    return crcPosition + CRC_SIZE_BYTES;
}

static bool sendToActiveTransport(uint8_t *packet, size_t packetSize)
{
  if (fmtTransport == FMT_SPI)
    return sendPacketSpi(packet, packetSize);
  else if (fmtTransport == FMT_UART)
    return sendPacketUart(packet, packetSize);
  return false;
}

static void mqtt5_app_start(void)
{
  esp_mqtt5_connection_property_config_t connect_property = {
      .session_expiry_interval = 10,
      .maximum_packet_size = 1024,
      .receive_maximum = 65535,
      .topic_alias_maximum = 2,
      .request_resp_info = true,
      .request_problem_info = true,
      .will_delay_interval = 10,
      .payload_format_indicator = true,
      .message_expiry_interval = 10,
      .response_topic = "/test/response",
      .correlation_data = "123456",
      .correlation_data_len = 6,
  };

  esp_mqtt_client_config_t mqtt5_cfg = {
      .broker.address.uri = CONFIG_BROKER_URL,
      .session.protocol_ver = MQTT_PROTOCOL_V_5,
      .network.disable_auto_reconnect = true,
      .credentials.username = "123",
      .credentials.authentication.password = "456",
      .session.last_will.topic = "/topic/will",
      .session.last_will.msg = "i will leave",
      .session.last_will.msg_len = 12,
      .session.last_will.qos = 1,
      .session.last_will.retain = true,
  };

#if CONFIG_BROKER_URL_FROM_STDIN
  char line[128];

  if (strcmp(mqtt5_cfg.uri, "FROM_STDIN") == 0)
  {
    int count = 0;
    printf("Please enter url of mqtt broker\n");
    while (count < 128)
    {
      int c = fgetc(stdin);
      if (c == '\n')
      {
        line[count] = '\0';
        break;
      }
      else if (c > 0 && c < 127)
      {
        line[count] = c;
        ++count;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    mqtt5_cfg.broker.address.uri = line;
    printf("Broker url: %s\n", line);
  }
  else
  {
    ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
    abort();
  }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

  client = esp_mqtt_client_init(&mqtt5_cfg);

  /* Set connection properties and user properties */
  esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
  esp_mqtt5_client_set_user_property(&connect_property.will_user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
  esp_mqtt5_client_set_connect_property(client, &connect_property);

  /* If you call esp_mqtt5_client_set_user_property to set user properties, DO NOT forget to delete them.
   * esp_mqtt5_client_set_connect_property will malloc buffer to store the user_property and you can delete it after
   */
  esp_mqtt5_client_delete_user_property(connect_property.user_property);
  esp_mqtt5_client_delete_user_property(connect_property.will_user_property);

  /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
  esp_mqtt_client_start(client);
}

// Try creating a new telemetry task, but task-creation crashes.
// void telemetryTask(void *params)
// {
// }

/** Print each byte of a buffer as decimals, space delimited.
 * @param msg must be a length-prefixed buffer (first byte is the length).
 */
void logMsgContents(uint8_t msg[])
{
  static unsigned int msgNum = 0;
  msgNum++;
  uint8_t msgLength = msg[0];

  if (msgLength <= MAX_PACKET_SIZE_BYTES)
  {
    char msgAsStr[256] = "";
    int strIndex = 0;
    for (int i = 0; i < msgLength; i++)
    {
      strIndex += snprintf(msgAsStr + strIndex, 4, "%d ", msg[i]);
    }
    ESP_LOGI(TAG, "msg: %u len: %d, %s", msgNum, msgLength, msgAsStr);
  }
  else
  {
    ESP_LOGE(TAG, "msg %d too long! len: %d", msgNum, msgLength);
  }
}

uint32_t usSinceTx(void) { return esp_timer_get_time() - lastResetTimeUs; }
void resetTxTimer(void) { lastResetTimeUs = esp_timer_get_time(); }

void handleEdgeMsg(uint8_t rxPacket[])
{

  // Assumes length < 127. TODO: permit larger.
  int msgLength = rxPacket[0] + 1; // +1 is for the message size prefix.

  // Empty messages will have msgLength == 1 for prefix => Drop these.
  if (msgLength > 1 && (mqttWritePos + msgLength < MQTT_BUFFER_SIZE))
  {
    memcpy(mqttBuffer + mqttWritePos, rxPacket, msgLength);
    mqttWritePos += msgLength;
    // logMsgContents(rxPacket);
  }
  if (usSinceTx() > MQTT_PUB_PERIOD_US && (mqttWritePos > 0))
  {
    esp_mqtt_client_publish(
        client, "hq-bound", (char *)mqttBuffer, mqttWritePos, 1, 1);
    mqttWritePos = 0;
    resetTxTimer();
  }
}

void app_main(void)
{
  // The following block doesn't work.  Core Panics.  No clue why.
  // TaskHandle_t telemHandle = NULL;
  // BaseType_t xRet;
  // xRet = xTaskCreatePinnedToCore(telemetryTask, "telem", 2048, (void *)0, tskIDLE_PRIORITY, &telemHandle, 1);

  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
  esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
  esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
  esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
  esp_log_level_set("transport", ESP_LOG_VERBOSE);
  esp_log_level_set("outbox", ESP_LOG_VERBOSE);

  ESP_ERROR_CHECK(initSpi());
  ESP_ERROR_CHECK(initUart());

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
   * Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect());

  mqtt5_app_start();

  for (;;)
  {
    uint8_t rxPacket[MAX_PAYLOAD_BYTES];
    esp_err_t rxStatus;

    if (fmtTransport == FMT_SPI)
    {
      rxStatus = waitForSpiRx(rxPacket, MSG_TIMEOUT_MS);
    }
    else if (fmtTransport == FMT_UART)
    {
      rxStatus = waitForUartRx(rxPacket, MSG_TIMEOUT_MS);
    }
    else if (fmtTransport == FMT_UNKNOWN)
    {
      /* If neither transport had data, yeild to unstarve other tasks */
      vTaskDelay(pdMS_TO_TICKS(MSG_TIMEOUT_MS));
      rxStatus = ESP_ERR_TIMEOUT;
    }

    /* If we timed out waiting for a message, it could be that the edge is now
    transmitting on a different transport. */
    if (rxStatus == ESP_ERR_TIMEOUT)
    {
      fmtTransport = checkForTransportChange(&rxStatus, rxPacket);
    }

    /* This is not an else if because chackForTransportChange tries to read from
    SPI and if it succeeds, a new packet will be available and need handling
    before the next pass through the loop.*/
    if (rxStatus == ESP_OK)
    {
      handleEdgeMsg(rxPacket);
    }
    else if (rxStatus != ESP_ERR_TIMEOUT)
    {
      ESP_LOGE(TAG, "unknown error: %d", rxStatus);
    }
  }
}

enum fmtTransport_e checkForTransportChange(esp_err_t *rxStatus, uint8_t *rxPacket)
{
  enum fmtTransport_e ret = FMT_UNKNOWN;
  /* The only way to check for data presence on SPI is to try reading it. If rx
  succeeds, there will be data that needs handling => *rxStatus = ESP_OK. */
  if (waitForSpiRx(rxPacket, 0) == ESP_OK)
  {
    *rxStatus = ESP_OK;
    ret = FMT_SPI;
  }

  /* UART can check for data presence while leaving it in FIFO.*/
  else if (uart_hasData())
  {
    ret = FMT_UART;
  }
  return ret;
}
