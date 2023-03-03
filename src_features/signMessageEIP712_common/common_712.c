#include "shared_context.h"
#include "os_io_seproxyhal.h"
#include "ui_callbacks.h"
#include "common_712.h"
#include "ui_callbacks.h"
#include "common_ui.h"
#include "apdu_constants.h"

static const uint8_t EIP_712_MAGIC[] = {0x19, 0x01};

unsigned int ui_712_approve_cb() {
    uint8_t privateKeyData[INT256_LENGTH];
    uint8_t hash[INT256_LENGTH];
    uint8_t signature[100];
    cx_ecfp_private_key_t privateKey;

    io_seproxyhal_io_heartbeat();
    cx_keccak_init(&global_sha3, 256);
    cx_hash((cx_hash_t *) &global_sha3,
            0,
            (uint8_t *) EIP_712_MAGIC,
            sizeof(EIP_712_MAGIC),
            NULL,
            0);
    cx_hash((cx_hash_t *) &global_sha3,
            0,
            tmpCtx.messageSigningContext712.domainHash,
            sizeof(tmpCtx.messageSigningContext712.domainHash),
            NULL,
            0);
    cx_hash((cx_hash_t *) &global_sha3,
            CX_LAST,
            tmpCtx.messageSigningContext712.messageHash,
            sizeof(tmpCtx.messageSigningContext712.messageHash),
            hash,
            sizeof(hash));
    PRINTF("EIP712 Domain hash 0x%.*h\n", 32, tmpCtx.messageSigningContext712.domainHash);
    PRINTF("EIP712 Message hash 0x%.*h\n", 32, tmpCtx.messageSigningContext712.messageHash);
    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1,
                               tmpCtx.messageSigningContext712.bip32.path,
                               tmpCtx.messageSigningContext712.bip32.length,
                               privateKeyData,
                               NULL);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);
    explicit_bzero(privateKeyData, sizeof(privateKeyData));
    unsigned int info = 0;
    io_seproxyhal_io_heartbeat();
    cx_ecdsa_sign(&privateKey,
                  CX_RND_RFC6979 | CX_LAST,
                  CX_SHA256,
                  hash,
                  sizeof(hash),
                  signature,
                  sizeof(signature),
                  &info);
    explicit_bzero(&privateKey, sizeof(privateKey));
    G_io_apdu_buffer[0] = 27;
    if (info & CX_ECCINFO_PARITY_ODD) {
        G_io_apdu_buffer[0]++;
    }
    if (info & CX_ECCINFO_xGTn) {
        G_io_apdu_buffer[0] += 2;
    }
    format_signature_out(signature);
    reset_app_context();
    send_apdu_response(true, 65);
    // Display back the original UX
    ui_idle();
    return 0;  // do not redraw the widget
}

unsigned int ui_712_reject_cb() {
    reset_app_context();
    send_apdu_response_explicit(APDU_SW_CONDITION_NOT_SATISFIED, 0);
    // Display back the original UX
    ui_idle();
    return 0;  // do not redraw the widget
}
