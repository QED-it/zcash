#ifndef ZCASH_ASSET_H
#define ZCASH_ASSET_H

#include "rust/include/rust/orchard/asset.h"

class Asset {

private:
    /**
     * Default constructor is only used during Native singleton creation
     * @return Native asset id
     */
    Asset() {
    };


public:
    static const int ZC_ORCHARD_ZSA_ASSET_ID_SIZE = 32;
    static const int ZC_ORCHARD_MAX_ASSET_DESCRIPTION_SIZE = 512;

    static const int TEMP_IK_SIZE = 32; // TODO find proper place

    /**
     * TODO remove this one in favor of ik+description
     */
    Asset(unsigned char* id) {
        std::copy(id, id + 32, this->id);
    }

    /**
     * Similar to 'derive' method from Rust
     * @param ik asset issuance key
     * @param description asset description
     * @return asset id of a non-native ZSA
     */
    Asset(unsigned char ik[TEMP_IK_SIZE], unsigned char* description) {
        zsa_derive_asset(ik, description, this->id);
    }


    // TODO proper singleton/global
    static Asset& Native() {
        static Asset zecSingleton;
        zsa_native_asset((unsigned char*)zecSingleton.id);
        return zecSingleton;
    }

    /**
     * Asset ID (byte representation of a Pallas curve point)
     */
    unsigned char id[ZC_ORCHARD_ZSA_ASSET_ID_SIZE];
    /**
     * Asset Issuer Key
     */
    unsigned char ik[TEMP_IK_SIZE];
    /**
     * Human-readable description of an asset (maximum size is defined by ZC_ORCHARD_MAX_ASSET_DESCRIPTION_SIZE)
     */
    unsigned char* description;
};


#endif //ZCASH_ASSET_H
