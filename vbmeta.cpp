#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>

#include <sys/system_properties.h>

#include "libavb/libavb.h"
#include "libavb_user/libavb_user.h"

// Helper function to extract digest from command line
std::string get_vbmeta_digest_from_cmdline(const char* cmdline) {
    if (cmdline == nullptr) {
        return "";
    }
    std::string cmdline_str(cmdline);
    std::string digest_key = "androidboot.vbmeta.digest=";
    size_t start_pos = cmdline_str.find(digest_key);
    if (start_pos == std::string::npos) {
        return "";
    }
    start_pos += digest_key.length();
    size_t end_pos = cmdline_str.find(" ", start_pos);
    if (end_pos == std::string::npos) {
        return cmdline_str.substr(start_pos);
    }
    return cmdline_str.substr(start_pos, end_pos - start_pos);
}

int main() {
    char buf[PROP_VALUE_MAX];
    std::string slot_suffix;

    if (__system_property_get("ro.boot.slot_suffix", buf) > 0) {
        slot_suffix = buf;
    }

    AvbOps* ops = avb_ops_user_new();
    if (ops == nullptr) {
        std::cerr << "Error: Could not create AvbOps." << std::endl;
        return 1;
    }

    AvbSlotVerifyData* slot_data = nullptr;
    const char* requested_partitions[] = {nullptr};

    AvbSlotVerifyResult verify_result = avb_slot_verify(ops,
                                                        requested_partitions,
                                                        slot_suffix.c_str(),
                                                        true, /* allow_verification_error */
                                                        &slot_data);

    if (verify_result != AVB_SLOT_VERIFY_RESULT_OK &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX) {
        std::cerr << "Error verifying slot: " << avb_slot_verify_result_to_string(verify_result) << std::endl;
        if (slot_data) {
            avb_slot_verify_data_free(slot_data);
        }
        // ops is const and doesn't have a free function in the user version.
        return 1;
    }

    if (slot_data == nullptr) {
        std::cerr << "Error: avb_slot_verify returned no data." << std::endl;
        return 1;
    }

    std::string digest = get_vbmeta_digest_from_cmdline(slot_data->cmdline);

    if (digest.empty()) {
        std::cerr << "Error: Could not find vbmeta digest in cmdline." << std::endl;
    } else {
        std::cout << "[VBMETA_DIGEST]:" << digest << std::endl;
    }

    avb_slot_verify_data_free(slot_data);

    return 0;
}