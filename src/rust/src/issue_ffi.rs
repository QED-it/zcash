use crate::streams_ffi::{CppStreamReader, CppStreamWriter, ReadCb, StreamObj, WriteCb};
use orchard::issuance::{verify_issue_bundle, IssueBundle, Signed, Unauthorized};
use orchard::keys::{IssuanceAuthorizingKey, IssuanceKey, IssuanceValidatingKey};
use orchard::note::AssetBase;
use orchard::value::NoteValue;
use orchard::Address;
use rand_core::OsRng;
use std::ffi::{c_char, CStr};
use std::{ptr, slice};
use tracing::error;
use zcash_primitives::transaction::components::issuance as issuance_serialization;

#[no_mangle]
pub extern "C" fn issuance_key_to_issuance_authorizing_key(
    key: *const IssuanceKey,
) -> *mut IssuanceAuthorizingKey {
    unsafe { key.as_ref() }
        .map(|key| Box::into_raw(Box::new(IssuanceAuthorizingKey::from(key))))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn issuance_key_free(key: *mut IssuanceKey) {
    if !key.is_null() {
        drop(unsafe { Box::from_raw(key) });
    }
}

#[no_mangle]
pub extern "C" fn issuance_key_clone(key: *const IssuanceKey) -> *mut IssuanceKey {
    unsafe { key.as_ref() }
        .map(|key| Box::into_raw(Box::new(key.clone())))
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
pub extern "C" fn issuance_key_for_account(
    seed: *const u8,
    seed_len: usize,
    bip44_coin_type: u32,
    account_id: u32,
) -> *mut IssuanceKey {
    let seed = unsafe { slice::from_raw_parts(seed, seed_len) };
    IssuanceKey::from_zip32_seed(seed, bip44_coin_type, account_id)
        .map(|key| Box::into_raw(Box::new(key)))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn issue_bundle_clone(
    bundle: *const IssueBundle<Signed>,
) -> *mut IssueBundle<Signed> {
    unsafe { bundle.as_ref() }
        .map(|bundle| Box::into_raw(Box::new(bundle.clone())))
        .unwrap_or(std::ptr::null_mut())
}

#[no_mangle]
pub extern "C" fn add_recipient(
    bundle: *mut IssueBundle<Unauthorized>,
    value: u64,
    recipient: *const Address,
    asset_descr: *const c_char,
) {
    let rng = OsRng;
    let recipient = unsafe { recipient.as_ref() }
        .copied()
        .expect("Recipient may not be null.");
    let asset_descr_str = unsafe { CStr::from_ptr(asset_descr) };
    let bundle = unsafe { bundle.as_mut() }.expect("Bundle may not be null.");

    let asset_descr = asset_descr_str
        .to_str()
        .expect("Asset description should contain correct UTF-8 string")
        .to_string();
    assert!(
        !asset_descr.is_empty(),
        "Add recipient: Asset description should not be empty"
    );

    bundle
        .add_recipient(asset_descr, recipient, NoteValue::from_raw(value), rng)
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
                "An error occurred while authorizing the issue bundle: {:?}",
                e
            )
        }
    }
}

#[no_mangle]
pub extern "C" fn issue_bundle_parse(
    stream: Option<StreamObj>,
    read_cb: Option<ReadCb>,
    bundle_ret: *mut *mut IssueBundle<Signed>,
) -> bool {
    let reader = CppStreamReader::from_raw_parts(stream, read_cb.unwrap());

    match issuance_serialization::read_v5_bundle(reader) {
        Ok(parsed) => {
            unsafe {
                *bundle_ret = if let Some(bundle) = parsed {
                    Box::into_raw(Box::new(bundle))
                } else {
                    ptr::null_mut::<IssueBundle<Signed>>()
                };
            };
            true
        }
        Err(e) => {
            error!("Failed to parse issue bundle: {}", e);
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn issue_bundle_serialize(
    bundle: *const IssueBundle<Signed>,
    stream: Option<StreamObj>,
    write_cb: Option<WriteCb>,
) -> bool {
    let bundle = unsafe { bundle.as_ref() };
    let writer = CppStreamWriter::from_raw_parts(stream, write_cb.unwrap());

    match issuance_serialization::write_v5_bundle(bundle, writer) {
        Ok(()) => true,
        Err(e) => {
            error!("Failed to serialize issue bundle: {}", e);
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn issue_bundle_verify(
    bundle: *const IssueBundle<Signed>,
    sighash: *const [u8; 32],
) -> bool {
    let bundle = unsafe { bundle.as_ref() };
    let sighash = unsafe { sighash.as_ref() };

    // TODO: implement reading/saving finalization set from/to global state
    let finalized = std::collections::HashSet::<AssetBase>::new();

    // TODO: conside returning false and log errors instead of calling "unwrap"
    match verify_issue_bundle(bundle.unwrap(), *sighash.unwrap(), &finalized) {
        Ok(_) => true,
        Err(e) => {
            error!("Issue bundle verification error: {}", e);
            false
        }
    }
}
