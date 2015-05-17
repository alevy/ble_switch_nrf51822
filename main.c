#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "boards.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "softdevice_handler.h"
#include "simple_uart.h"

#include "app_button.h"
#include "app_gpiote.h"
#include "app_timer.h"

#include "error.h"
#include "ble.h"

#define BUTTON_DETECTION_DELAY  50

int err_code;

char device_name[20];

const ble_uuid128_t VENDOR_BASE = {{0x5F, 0x51, 0xB6, 0x84, 0x12, 0xA5, 0xA3, 0xFF,
  0x51, 0x64, 0x8A, 0x5A, 0x0, 0x0, 0x94,0xFD}};

const ble_uuid128_t ONOFF_CHAR = {{0xFD, 0x94, 0x00, 0x00, 0x5A, 0x8A, 0x64, 0x51,
  0xFF, 0xA3, 0xA5, 0x12, 0x84, 0xB6, 0x51,0x5F}};

uint8_t vendor_type;

uint16_t light_conn_handle;

ble_uuid_t light_srvc;
ble_uuid_t onoff_char;
uint16_t onoff_value_handle;

uint8_t on_value[1] = {1};
uint8_t off_value[1] = {0};

ble_gattc_handle_range_t light_srvc_handle_range;

static void button_event_handler(uint8_t pin_no, uint8_t button_action) {
  simple_uart_putstring((uint8_t*)"Button Event\r\n");
  if (button_action == APP_BUTTON_PUSH) {
    ble_gattc_write_params_t write_cmd = {
      .write_op = BLE_GATT_OP_WRITE_REQ,
      .handle = onoff_value_handle,
      .offset = 0,
      . flags = 0
    };
    switch (pin_no) {
      case BUTTON_0:
        simple_uart_putstring((uint8_t*)"Light off\r\n");
        write_cmd.p_value = off_value;
        write_cmd.len = sizeof(off_value);
        sd_ble_gattc_write(light_conn_handle, &write_cmd);
        break;
      case BUTTON_1:
        simple_uart_putstring((uint8_t*)"Light on\r\n");
        write_cmd.p_value = on_value;
        write_cmd.len = sizeof(on_value);
        sd_ble_gattc_write(light_conn_handle, &write_cmd);
        break;
      default:
        APP_ERROR_HANDLER(pin_no);
        break;
    }
  }    
}

static void buttons_init(void) {
    static app_button_cfg_t buttons[] =
    {
        {BUTTON_0, false, BUTTON_PULL, button_event_handler},
        {BUTTON_1, false, BUTTON_PULL, button_event_handler}
    };
    
    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
}

static void handle_connected(ble_gap_evt_t *evt) {
  uint16_t conn_handle = evt->conn_handle;
  sd_ble_gattc_primary_services_discover(conn_handle, 1, &light_srvc);
}

static void handle_prim_srvc_rsp(ble_gattc_evt_t *evt) {
  ble_gattc_evt_prim_srvc_disc_rsp_t rsp = evt->params.prim_srvc_disc_rsp;
  if (rsp.count > 0) {
    simple_uart_putstring((uint8_t*)"Service discovered!\r\n");
    light_srvc_handle_range = rsp.services[0].handle_range;
    sd_ble_gattc_characteristics_discover(evt->conn_handle,
        &light_srvc_handle_range);
  }
}

static void handle_char_disc_rsp(ble_gattc_evt_t *evt) {
  ble_gattc_evt_char_disc_rsp_t rsp = evt->params.char_disc_rsp;
  for (int i = 0; i < rsp.count; ++i) {
    ble_uuid_t uuid = rsp.chars[i].uuid;
    if (uuid.uuid == onoff_char.uuid && uuid.type == onoff_char.type) {
      onoff_value_handle = rsp.chars[i].handle_value;
      light_conn_handle = evt->conn_handle;
      simple_uart_putstring((uint8_t*)"Handle found!\r\n");
      app_button_enable();
      break;
    }
  }
}

static void evt_handler(ble_evt_t *evt) {
  simple_uart_putstring((uint8_t*)"Event!\r\n");
  switch (evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      simple_uart_putstring((uint8_t*)"Connected!\r\n");
      handle_connected(&evt->evt.gap_evt);
      break;
    case BLE_GAP_EVT_DISCONNECTED:
      break;
    case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
      handle_prim_srvc_rsp(&evt->evt.gattc_evt);
      break;
    case BLE_GATTC_EVT_CHAR_DISC_RSP:
      handle_char_disc_rsp(&evt->evt.gattc_evt);
      break;
    //default:
      //APP_ERROR_HANDLER(evt->header.evt_id);
  }

}

/**@brief Function for the application main entry.
 */
int main(void) {
    
  // Initialize the SoftDevice handler module.
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);
  APP_TIMER_INIT(0, 5, 5, false);
  APP_GPIOTE_INIT(1);
  buttons_init();

  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER,
      RX_PIN_NUMBER, false);

  snprintf(device_name, sizeof(device_name), "Light Switch %d", 1);

  ble_stack_init(&evt_handler);

  sd_ble_uuid_vs_add(&VENDOR_BASE, &vendor_type);
  light_srvc.uuid = 0x00 << 8 | 0x01;
  light_srvc.type = vendor_type;
  onoff_char.uuid = 0x00 << 8 | 0x00;
  onoff_char.type = vendor_type;

  advertising_init(device_name);
  advertising_start();

  simple_uart_putstring((const uint8_t*)"Advertising\r\n");


  while(1) {
    // Switch to a low power state until an event is available for the application
    err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
  }
}

