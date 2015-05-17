#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "simple_uart.h"

#include "error.h"

/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t *
    p_file_name) {
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    //NVIC_SystemReset();
    simple_uart_putstring((uint8_t*)"ERROR ");
    simple_uart_putstring(p_file_name);
    simple_uart_putstring((uint8_t*)": ");
    simple_uart_put(line_num / 100 + 48);
    simple_uart_put((line_num % 100) / 10 + 48);
    simple_uart_put(line_num % 10 + 48);
    while (1) {
    }
}


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

