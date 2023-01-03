#ifndef ZCASH_ASSET_H
#define ZCASH_ASSET_H

#include "rust/include/rust/orchard/asset.h"

class Asset {


public:
    static const int ZC_ORCHARD_ZSA_ASSET_ID_SIZE = 32;
    static const int ZC_ORCHARD_MAX_ASSET_DESCRIPTION_SIZE = 512;

    static const int TEMP_IK_SIZE = 32; // TODO find proper place


    /**
     * Similar to 'derive' method from Rust
     * @param ik asset issuance key
     * @param description asset description
     * @return asset id of a non-native ZSA
     */
    Asset(unsigned char ik[TEMP_IK_SIZE], unsigned char* description);


    // TODO proper singleton/global

    /**
     * Default constructor is only used during ZEC singleton creation
     * @return ZEC asset id
     */
    Asset() {};

    static Asset& ZEC() {
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
