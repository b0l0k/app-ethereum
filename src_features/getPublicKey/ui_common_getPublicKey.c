#include "shared_context.h"
#include "feature_getPublicKey.h"
#include "common_ui.h"
#include "apdu_constants.h"

unsigned int io_seproxyhal_touch_address_ok(__attribute__((unused)) const bagl_element_t *e) {
    uint32_t tx = set_result_get_publicKey();
    return ui_cb_ok(tx);
}

unsigned int io_seproxyhal_touch_address_cancel(__attribute__((unused)) const bagl_element_t *e) {
    return ui_cb_cancel();
}
