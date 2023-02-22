use orchard::issuance::{IssueBundle, Signed, Unauthorized};
use orchard::keys::{IssuanceAuthorizingKey, IssuanceValidatingKey, SpendingKey};
use orchard::value::NoteValue;
use orchard::Address;
use rand_core::OsRng;
use std::ffi::{c_char, CStr};

#[no_mangle]
pub extern "C" fn orchard_spending_key_to_issuance_authorizing_key(
    key: *const SpendingKey,
) -> *mut IssuanceAuthorizingKey {
    unsafe { key.as_ref() }
        .map(|key| Box::into_raw(Box::new(IssuanceAuthorizingKey::from(key))))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn issuance_authorizing_key_free(key: *mut IssuanceAuthorizingKey) {
    if !key.is_null() {
        drop(unsafe { Box::from_raw(key) });
    }
}

#[no_mangle]
pub extern "C" fn issuance_authorizing_key_clone(
    key: *const IssuanceAuthorizingKey,
) -> *mut IssuanceAuthorizingKey {
    unsafe { key.as_ref() }
        .map(|key| Box::into_raw(Box::new(key.clone())))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn issuance_authorizing_key_to_issuance_validating_key(
    key: *const IssuanceAuthorizingKey,
) -> *mut IssuanceValidatingKey {
    unsafe { key.as_ref() }
        .map(|key| Box::into_raw(Box::new(IssuanceValidatingKey::from(key))))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn issuance_validating_key_free(key: *mut IssuanceValidatingKey) {
    if !key.is_null() {
        drop(unsafe { Box::from_raw(key) });
    }
}

#[no_mangle]
pub extern "C" fn issue_bundle_free(bundle: *mut IssueBundle<Unauthorized>) {
    if !bundle.is_null() {
        drop(unsafe { Box::from_raw(bundle) });
    }
}

#[no_mangle]
pub extern "C" fn create_issue_bundle(
    isk: *const IssuanceAuthorizingKey,
) -> *mut IssueBundle<Unauthorized> {
    let isk = unsafe { isk.as_ref() }.expect("IssuanceAuthorizingKey may not be null.");
    let ik: IssuanceValidatingKey = isk.into();

    Box::into_raw(Box::new(IssueBundle::new(ik)))
}

#[no_mangle]
pub extern "C" fn add_recipient(
    bundle: *mut IssueBundle<Unauthorized>,
    value: u64,
    recipient: *const Address,
    asset_descr: *const c_char,
    finalize: bool,
) {
    let rng = OsRng;
    let recipient = unsafe { recipient.as_ref() }
        .copied()
        .expect("Recipient may not be null.");
    let asset_descr_str = unsafe { CStr::from_ptr(asset_descr) };
    let bundle = unsafe { bundle.as_mut() }.expect("Bundle may not be null.");

    bundle
        .add_recipient(
            asset_descr_str
                .to_str()
                .expect("Asset description should contain correct UTF-8 string")
                .to_string(),
            recipient,
            NoteValue::from_raw(value),
            finalize,
            rng,
        )
        .expect("An error occurred while adding recipient");
}

#[no_mangle]
pub extern "C" fn sign_issue_bundle(
    bundle: *mut IssueBundle<Unauthorized>,
    isk: *const IssuanceAuthorizingKey,
) -> *mut IssueBundle<Signed> {
    let rng = OsRng;
    let isk = unsafe { isk.as_ref() }.expect("IssuanceAuthorizingKey may not be null.");
    let bundle = unsafe { Box::from_raw(bundle) };
    let sighash: [u8; 32] = bundle.commitment().into();
    let res = bundle.prepare(sighash).sign(rng, isk);
    match res {
        Ok(signed) => Box::into_raw(Box::new(signed)),
        Err(e) => {
            panic!(
                "An error occurred while authorizing the orchard bundle: {:?}",
                e
            )
        }
    }
}
