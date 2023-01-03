use orchard::note::AssetId;


#[no_mangle]
pub extern "C" fn zsa_native_asset(asset_ret: *mut [u8; 32]) -> bool {
    *unsafe { &mut *asset_ret } = AssetId::native().to_bytes();
    true
}


#[no_mangle]
pub extern "C" fn zsa_derive_asset(asset_ret: *mut [u8; 32]) -> bool {
    false // TODO derive
}
