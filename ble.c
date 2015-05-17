#include "ble.h"
#include "ble_advdata.h"
#include "app_error.h"
#include "softdevice_handler.h"

/**< Parameters to be passed to the stack when starting advertising. */
ble_gap_adv_params_t m_adv_params = {
  .type = BLE_GAP_ADV_TYPE_ADV_IND,
  .fp = BLE_GAP_ADV_FP_ANY,
  .interval = ADV_INTERVAL,
  .timeout = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED
};

void advertising_start() {
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

}

void advertising_init(char* device_name) {
  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
  APP_ERROR_CHECK(sd_ble_gap_device_name_set(&sec_mode, (uint8_t*)device_name,
        strlen(device_name)));

  uint8_t adv_flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  int8_t tx_power_level = 0;
  sd_ble_gap_tx_power_set(tx_power_level);
  ble_advdata_t advdata = {
    .name_type               = BLE_ADVDATA_FULL_NAME,
    .include_appearance      = true,
    .p_tx_power_level        = &tx_power_level,
    .flags.size              = 1,
    .flags.p_data            = &adv_flags
  };

  APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(ble_evt_handler_t evt_handler) {
  uint32_t err_code;

  APP_ERROR_CHECK(softdevice_ble_evt_handler_set(evt_handler));

  // Enable BLE stack 
  ble_enable_params_t ble_enable_params;
  memset(&ble_enable_params, 0, sizeof(ble_enable_params));
  ble_enable_params.gatts_enable_params.service_changed =
    IS_SRVC_CHANGED_CHARACT_PRESENT;
  err_code = sd_ble_enable(&ble_enable_params);
  APP_ERROR_CHECK(err_code);
}

