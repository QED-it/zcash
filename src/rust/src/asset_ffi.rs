use std::ffi::{c_char, CStr};
use orchard::keys::IssuanceValidatingKey;
use orchard::note::AssetId;

#[no_mangle]
pub extern "C" fn zsa_native_asset(asset_ret: *mut [u8; 32]) -> bool {
    *unsafe { &mut *asset_ret } = AssetId::native().to_bytes();
    true
}

#[no_mangle]
pub extern "C" fn zsa_derive_asset(
    ik_bytes: *mut [u8; 32],
    asset_desc_ptr: *const c_char,
    asset_ret: *mut [u8; 32],
) -> bool {

    unsafe {
        assert!(!ik_bytes.is_null());
        assert!(!asset_desc_ptr.is_null());
        assert!(!asset_ret.is_null());

        let ik_bytes: &[u8] = &ik_bytes.as_ref().copied().expect("Error during copying of VerificationKey");
        let ik = IssuanceValidatingKey::from_bytes(ik_bytes).expect("Error during building IssuanceValidatingKey from bytes");
        let asset_desc = CStr::from_ptr(asset_desc_ptr).to_str().expect("Asset description should contain correct UTF-8 string");

        *asset_ret = AssetId::derive(&ik, asset_desc).to_bytes();
    }
    true
}
