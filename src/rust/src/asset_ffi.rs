use orchard::note::AssetId;

#[no_mangle]
pub extern "C" fn zsa_native_asset(asset_ret: *mut [u8; 32]) -> bool {
    *unsafe { &mut *asset_ret } = AssetId::native().to_bytes();
    true
}

// TODO implement derivation
// #[no_mangle]
// pub extern "C" fn zsa_derive_asset(
//     ik_bytes: *mut [u8; 32],
//     asset_desc_ptr: *mut u8,
//     asset_desc_len: usize,
//     note_type_ret: *mut [u8; 32],
// ) -> bool {
//
//     unsafe {
//         assert!(!ik_bytes.is_null());
//         assert!(!asset_desc_ptr.is_null());
//         assert!(!note_type_ret.is_null());
//
//         let ik = IssuanceValidatingKey::try_from(ik_bytes.as_ref().copied().expect("Error during copying of VerificationKey")).expect("Error during building IssuanceValidatingKey from bytes");
//         let mut asset_desc = String::from_utf8_unchecked(slice::from_raw_parts(asset_desc_ptr as *const u8, asset_desc_len).to_vec()).as_str();
//
//         *note_type_ret = AssetId::derive(&ik, asset_desc).to_bytes();
//     }
//     true
// }
