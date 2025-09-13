#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>

#include <sys/system_properties.h>

#include "libavb/libavb.h"
#include "libavb_user/libavb_user.h"

// Helper function to extract a specific value from the cmdline
std::string parse_cmdline(const char* cmdline, const std::string& key_prefix) {
    if (cmdline == nullptr) {
        return "";
    }
    std::string cmdline_str(cmdline);
    size_t start_pos = cmdline_str.find(key_prefix);
    if (start_pos == std::string::npos) {
        return "";
    }
    start_pos += key_prefix.length();
    size_t end_pos = cmdline_str.find(" ", start_pos);
    if (end_pos == std::string::npos) {
        return cmdline_str.substr(start_pos);
    }
    return cmdline_str.substr(start_pos, end_pos - start_pos);
}

int main(int argc, char* argv[]) {
    int ret = 0;
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
                                                        AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR,
                                                        AVB_HASHTREE_ERROR_MODE_EIO,
                                                        &slot_data);

    if (slot_data == nullptr) {
        // try with NO_VBMETA_PARTITION flag
        verify_result = avb_slot_verify(ops,
                                        requested_partitions,
                                        slot_suffix.c_str(),
                                        AVB_SLOT_VERIFY_FLAGS_NO_VBMETA_PARTITION,
                                        AVB_HASHTREE_ERROR_MODE_EIO,
                                        &slot_data);
    }

    if (verify_result != AVB_SLOT_VERIFY_RESULT_OK &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_OOM &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_IO &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_VERIFICATION &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_ROLLBACK_INDEX &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_PUBLIC_KEY_REJECTED &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_METADATA &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_UNSUPPORTED_VERSION &&
        verify_result != AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT) {
        std::cerr << "Error verifying slot: " << avb_slot_verify_result_to_string(verify_result) << std::endl;
        if (slot_data) {
            avb_slot_verify_data_free(slot_data);
        }
        avb_ops_user_free(ops);
        return 1;
    }

    if (slot_data == nullptr) {
        std::cerr << "Error: avb_slot_verify returned no data." << std::endl;
        avb_ops_user_free(ops);
        return 1;
    }

    std::string digest = parse_cmdline(slot_data->cmdline, "androidboot.vbmeta.digest=");
    std::string hash_alg = parse_cmdline(slot_data->cmdline, "androidboot.vbmeta.hash_alg=");
    std::string size = parse_cmdline(slot_data->cmdline, "androidboot.vbmeta.size=");

    if (argc == 1) {
        if (!digest.empty()) {
            std::cout << "digest:" << digest << std::endl;
        }
        if (!hash_alg.empty()) {
            std::cout << "hash_alg:" << hash_alg << std::endl;
        }
        if (!size.empty()) {
            std::cout << "size:" << size << std::endl;
        }
    } else if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "digest") {
            std::cout << digest << std::endl;
        } else if (arg == "hash_alg") {
            std::cout << hash_alg << std::endl;
        } else if (arg == "size") {
            std::cout << size << std::endl;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " digest|hash_alg|size" << std::endl;
        } else {
            std::cerr << "Error: Unknown argument. Usage: " << argv[0] << " digest|hash_alg|size" << std::endl;
            ret = 1;
            goto cleanup;
        }
    } else {
        std::cerr << "Error: Too many arguments. Usage: " << argv[0] << " digest|hash_alg|size" << std::endl;
        ret = 1;
        goto cleanup;
    }

cleanup:
    if (slot_data) {
        avb_slot_verify_data_free(slot_data);
    }
    if (ops) {
        avb_ops_user_free(ops);
    }

    return ret;
}