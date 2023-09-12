/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019-2020 The LineageOS Project.
   Copyright (C) 2021 The Android Open Source Project.
   Copyright (C) 2022-2023 Paranoid Android.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

std::vector<std::string> ro_props_default_source_order = {
    "",
    "bootimage.",
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm.",
};


void property_override(char const prop[], char const value[], bool add = true) {
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const std::string &prop, const std::string &value) {
    for (const auto &source : ro_props_default_source_order) {
        auto prop_name = "ro." + source + "build." + prop;
        if (source == "")
            property_override(prop_name.c_str(), value.c_str());
        else
            property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void set_ro_product_prop(const std::string &prop, const std::string &value) {
    for (const auto &source : ro_props_default_source_order) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    }
};


void set_device_props(const string model, const string name, const string marketname,
        const string mod_device) {
    set_ro_build_prop("model", model);
    set_ro_build_prop("name", name);
    set_ro_build_prop("marketname", marketname);

    property_override("ro.product.mod_device", mod_device.c_str());
    property_override("bluetooth.device.default_name", marketname.c_str());
}

void vendor_load_properties() {

    std::string hardware_revision = GetProperty("ro.boot.hwversion", "UNKNOWN");
    std::string hwname = GetProperty("ro.boot.hwname", "surya");
    std::string region = GetProperty("ro.boot.hwc", "INT");
    std::string build_type = GetProperty("ro.build.type", "user");

    std::string model;
    std::string device;
    std::string fingerprint;
    std::string mod_device;
    std::string description;
    std::string name;
    std::string marketname;
    std::string flavor;

    if (region == "CN") { // China
        model = "22101320C";
        name = "redwood";
        device = "redwood";
        mod_device = "redwood";
        fingerprint = "Redmi/redwood/redwood:12/RKQ1.211001.001/V14.0.8.0.SMSINXM:user/release-keys";
        description = "redwood-user 12 RKQ1.211001.001 V14.0.8.0.SJGMIXM release-keys";
        marketname = "Redmi Note 12 Pro Speed";

    } else if (region == "IN") { // India
        model = "22101320I";
        name = "redwoodin";
        device = "redwood";
        mod_device = "redwoodin";
        fingerprint = "POCO/redwoodin/redwoodin:12/RKQ1.211001.001/V14.0.8.0.SMSINXM:user/release-keys";
        description = "redwoodin-user 12 RKQ1.211001.001 V14.0.6.0.SJGMIXM release-keys";
        marketname = "Poco X5 Pro 5G";
    } else { // Global
        model = "22101320G";
        name = "redwood_global";
        device = "redwood";
        mod_device = "redwood_global";
        fingerprint = "POCO/redwood_global/redwood:13/RKQ1.211001.001/V14.0.2.0.TMSMIXM:user/release-keys";
        description = "redwood_global-user 13 RKQ1.211001.001 V14.0.2.0.TMSMIXM release-keys";
        marketname = "Poco X5 Pro 5G";
    }

    set_ro_build_prop("fingerprint", fingerprint);
    set_ro_product_prop("brand", "POCO");
    set_ro_product_prop("device", device);
    set_ro_product_prop("product", device);
    set_ro_product_prop("model", model);
    set_ro_product_prop("name", name);
    property_override("ro.build.product", device.c_str());
    property_override("ro.build.board", device.c_str());
    property_override("ro.build.description", description.c_str());
    property_override("ro.product.mod_device", mod_device.c_str());
    property_override("ro.boot.hardware.revision", hardware_revision.c_str());
    property_override("bluetooth.device.default_name", marketname.c_str());
    property_override("vendor.usb.product_string", marketname.c_str());
    property_override("ro.build.flavor", flavor.c_str());
    property_override("ro.boot.hardware.sku", device.c_str());
}
