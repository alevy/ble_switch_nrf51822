#include "softdevice_handler.h"

/**< Include or not the service_changed characteristic. if not enabled, the
 * server's database cannot be changed for the lifetime of the device*/
#define IS_SRVC_CHANGED_CHARACT_PRESENT  0

/**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#define ADV_INTERVAL     MSEC_TO_UNITS(100, UNIT_0_625_MS)

/**@brief Function for starting advertising.
 */
void advertising_start(void);

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
void advertising_init(char*);

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(ble_evt_handler_t handler);

