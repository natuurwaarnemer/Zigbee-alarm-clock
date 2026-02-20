// zigbee.cpp - Zigbee device implementation using ESP-Zigbee-SDK
// Hardware: ESP32-C6 native Zigbee radio

#include "zigbee.h"
#include <Arduino.h>
#include <string.h>

// NOTE: The ESP-Zigbee-SDK requires ESP-IDF and is included via the
// platformio.ini sdkconfig. The headers below come from the ESP-IDF
// zigbee component (esp-zigbee-sdk).
#include "esp_zigbee_core.h"

// ---- Internal state ----
static bool s_connected = false;
static zigbee_alarm_config_cb_t s_alarm_cb = nullptr;

// Zigbee device configuration
static esp_zb_cfg_t s_zb_config = {
    .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,   // End Device
    .install_code_policy = false,
    .nwk_cfg = {
        .zed_cfg = {
            .ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN,
            .keep_alive = 3000, // ms
        }
    }
};

// ---- Zigbee signal handler ----
static void zigbee_signal_handler(esp_zb_app_signal_t *signal_struct) {
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = (esp_zb_app_signal_type_t)*p_sg_p;

    switch (sig_type) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
            break;

        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (err == ESP_OK) {
                esp_zb_bdb_start_top_level_commissioning(
                    ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err == ESP_OK) {
                s_connected = true;
                Serial.println("[Zigbee] Joined network successfully");
            } else {
                s_connected = false;
                Serial.println("[Zigbee] Network steering failed - retrying");
                esp_zb_bdb_start_top_level_commissioning(
                    ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
            break;

        case ESP_ZB_ZDO_SIGNAL_LEAVE:
            s_connected = false;
            Serial.println("[Zigbee] Left network");
            break;

        default:
            break;
    }
}

// ---- Attribute write callback (receive config from coordinator) ----
static esp_err_t zb_attribute_handler(
    const esp_zb_zcl_set_attr_value_params_t *message)
{
    if (message->info.cluster == ZB_CLUSTER_ALARM_CONFIG) {
        // Parse alarm configuration attributes
        static uint8_t  pending_day    = 0;
        static uint8_t  pending_hour   = 7;
        static uint8_t  pending_minute = 0;
        static bool     pending_en     = false;

        switch (message->attribute.id) {
            case ZB_ATTR_ALARM_DAY:
                pending_day = *(uint8_t*)message->attribute.data.value;
                break;
            case ZB_ATTR_ALARM_HOUR:
                pending_hour = *(uint8_t*)message->attribute.data.value;
                break;
            case ZB_ATTR_ALARM_MINUTE:
                pending_minute = *(uint8_t*)message->attribute.data.value;
                break;
            case ZB_ATTR_ALARM_ENABLED:
                pending_en = (*(uint8_t*)message->attribute.data.value) != 0;
                // ENABLED is always last - commit the update
                if (s_alarm_cb && pending_day >= 1 && pending_day <= 7) {
                    s_alarm_cb(pending_day, pending_hour, pending_minute, pending_en);
                }
                break;
            case ZB_ATTR_MASTER_SWITCH:
                // Master switch handled separately via on_off cluster
                break;
        }
    }
    return ESP_OK;
}

// ---- Public API ----

void zigbee_init(void) {
    // Initialize Zigbee stack (non-blocking init)
    esp_zb_platform_config_t config = {
        .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
        .host_config  = ESP_ZB_DEFAULT_HOST_CONFIG(),
    };
    esp_zb_platform_config(&config);
    esp_zb_init(&s_zb_config);

    // ---- Cluster list for alarm endpoint ----
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();

    // Basic cluster
    esp_zb_basic_cluster_cfg_t basic_cfg = {
        .zcl_version   = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
        .power_source  = 0x04, // DC source
    };
    esp_zb_cluster_list_add_basic_cluster(cluster_list,
        esp_zb_basic_cluster_create(&basic_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // On/Off cluster (for button state and master switch)
    esp_zb_on_off_cluster_cfg_t on_off_cfg = { .on_off = false };
    esp_zb_cluster_list_add_on_off_cluster(cluster_list,
        esp_zb_on_off_cluster_create(&on_off_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // ---- Endpoint list ----
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_endpoint_config_t ep_cfg = {
        .endpoint         = ZB_ALARM_ENDPOINT,
        .app_profile_id   = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id    = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, ep_cfg);

    // Register device
    esp_zb_device_register(ep_list);

    // Register attribute write callback
    esp_zb_core_action_handler_register(
        (esp_zb_core_action_callback_t)zb_attribute_handler);
}

bool zigbee_start(void) {
    esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
    esp_zb_start(false);
    // Stack processes in main loop via esp_zb_main_loop_iteration()
    return true;
}

void zigbee_update(void) {
    esp_zb_main_loop_iteration();
}

void zigbee_send_button_press(void) {
    if (!s_connected) return;
    // Toggle On/Off attribute to signal button press
    esp_zb_zcl_on_off_cmd_t cmd = {
        .zcl_basic_cmd = {
            .src_endpoint  = ZB_ALARM_ENDPOINT,
        },
        .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
        .on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID,
    };
    esp_zb_zcl_on_off_cmd_req(&cmd);
}

void zigbee_send_alarm_status(bool active) {
    if (!s_connected) return;
    uint8_t val = active ? 1 : 0;
    esp_zb_zcl_set_attribute_val(
        ZB_ALARM_ENDPOINT,
        ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
        ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
        &val, false);
}

bool zigbee_is_connected(void) {
    return s_connected;
}

void zigbee_set_alarm_config_callback(zigbee_alarm_config_cb_t cb) {
    s_alarm_cb = cb;
}
