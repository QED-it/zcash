use std::{mem, ptr};
use crate::{bridge::ffi, streams::CppStream};

use orchard::{Address, issuance as issuance};
use issuance::Signed;
use memuse::DynamicUsage;
use orchard::value::NoteValue;
use orchard::issuance::IssueInfo;
use orchard::keys::{IssuanceAuthorizingKey, IssuanceValidatingKey};
use rand_core::OsRng;
use zcash_primitives::transaction::components::{issuance as issue_serialization};

pub struct IssueNote(orchard::note::Note);

impl IssueNote {
    pub(crate) fn recipient(self: &IssueNote) -> [u8; 43] {
        self.0.recipient().to_raw_address_bytes()
    }

    pub(crate) fn value(self: &IssueNote) -> u64 {
        self.0.value().inner()
    }

    pub(crate) fn asset(self: &IssueNote) -> [u8; 32] {
        self.0.asset().to_bytes()
    }

    pub(crate) fn rho(self: &IssueNote) -> [u8; 32] {
        self.0.rho().to_bytes()
    }

    pub(crate) fn rseed(self: &IssueNote) -> [u8; 32] {
        *self.0.rseed().as_bytes()
    }
}

pub struct IssueAction(orchard::issuance::IssueAction);

impl IssueAction {
    pub(crate) fn asset_desc(self: &IssueAction) -> Vec<u8> {
        self.0.asset_desc().as_bytes().to_vec()
    }

    pub(crate) fn notes(self: &IssueAction) -> Vec<IssueNote> {
        self.0.notes().iter().map(|it| IssueNote(*it)).collect()
    }

    pub(crate) fn is_finalized(self: &IssueAction) -> bool {
        self.0.is_finalized()
    }
}

#[derive(Clone)]
pub struct IssueBundle(Option<issuance::IssueBundle<Signed>>);

pub(crate) fn none_issue_bundle() -> Box<IssueBundle> {
    Box::new(IssueBundle(None))
}

pub(crate) unsafe fn issue_bundle_from_raw_box(
    bundle: *mut ffi::IssueBundlePtr,
) -> Box<IssueBundle> {
    IssueBundle::from_raw_box(bundle)
}

/// Parses an authorized Orchard bundle from the given stream.
pub(crate) fn parse_issue_bundle(reader: &mut CppStream<'_>) -> Result<Box<IssueBundle>, String> {
    IssueBundle::parse(reader)
}

pub(crate) fn create_issue_bundle(
    isk: *const ffi::IssuanceAuthorizingKeyPtr,
    value: u64,
    recipient: *const ffi::OrchardRawAddressPtr,
    asset_descr: String,
) -> Box<IssueBundle> {
    let recipient: Address = *unsafe {
        recipient
            .cast::<Address>()
            .as_ref()
    }.expect("IssuanceAuthorizingKey may not be null.");

    let isk = unsafe {
        isk
            .cast::<IssuanceAuthorizingKey>()
            .as_ref()
    }.expect("IssuanceAuthorizingKey may not be null.");

    let ik: IssuanceValidatingKey = IssuanceValidatingKey::from(isk);

    let rng = OsRng;

    let bundle = issuance::IssueBundle::new(ik, asset_descr, Some(IssueInfo { recipient, value: NoteValue::from_raw(value) }), rng).unwrap().0;

    let sighash: [u8; 32] = bundle.commitment().into();
    Box::new(IssueBundle(Some(bundle.prepare(sighash).sign(rng, isk).unwrap())))
}

impl IssueBundle {

    pub(crate) unsafe fn from_raw_box(bundle: *mut ffi::IssueBundlePtr) -> Box<Self> {
        Box::new(IssueBundle(if bundle.is_null() {
            None
        } else {
            let bundle: *mut issuance::IssueBundle<Signed> = bundle.cast();
            Some(*Box::from_raw(bundle))
        }))
    }

    /// Returns a copy of the value.
    pub(crate) fn box_clone(&self) -> Box<Self> {
        Box::new(self.clone())
    }

    /// Parses an authorized Orchard bundle from the given stream.
    pub(crate) fn parse(reader: &mut CppStream<'_>) -> Result<Box<Self>, String> {
        match issue_serialization::read_v5_bundle(reader) {
            Ok(parsed) => Ok(Box::new(IssueBundle(parsed))),
            Err(e) => Err(format!("Failed to parse Orchard bundle: {}", e)),
        }
    }

    /// Serializes an authorized Orchard bundle to the given stream.
    ///
    /// If `bundle == None`, this serializes `nActionsOrchard = 0`.
    pub(crate) fn serialize(&self, writer: &mut CppStream<'_>) -> Result<(), String> {
        issue_serialization::write_v5_bundle(self.inner(), writer)
            .map_err(|e| format!("Failed to serialize Orchard bundle: {}", e))
    }

    pub(crate) fn inner(&self) -> Option<&issuance::IssueBundle<Signed>> {
        self.0.as_ref()
    }

    pub(crate) fn as_ptr(&self) -> *const ffi::IssueBundlePtr {
        if let Some(bundle) = self.inner() {
            let ret: *const issuance::IssueBundle<Signed> = bundle;
            ret.cast()
        } else {
            ptr::null()
        }
    }

    /// Returns the amount of dynamically-allocated memory used by this bundle.
    pub(crate) fn recursive_dynamic_usage(&self) -> usize {
        self.inner()
            // Bundles are boxed on the heap, so we count their own size as well as the size
            // of `Vec`s they allocate.
            .map(|bundle| mem::size_of_val(bundle) + bundle.dynamic_usage())
            // If the transaction has no Orchard component, nothing is allocated for it.
            .unwrap_or(0)
    }

    /// Returns whether the Orchard bundle is present.
    pub(crate) fn is_present(&self) -> bool {
        self.0.is_some()
    }

    pub(crate) fn actions(&self) -> Vec<IssueAction> {
        self.0
            .iter()
            .flat_map(|b| b.actions().iter())
            .cloned()
            .map(IssueAction)
            .collect()
    }

    pub(crate) fn num_actions(&self) -> usize {
        self.0.as_ref().map(|b| b.actions().len()).unwrap_or(0)
    }

    pub(crate) fn num_notes(&self) -> usize {
        self.0
            .as_ref()
            .map(|b| b.actions().iter().map(|a| a.notes().len()).sum())
            .unwrap_or(0)
    }

    pub(crate) fn ik(&self) -> [u8; 32] {
        self.0
            .as_ref()
            .expect("IssueBundle may not be null")
            .ik()
            .to_bytes()
    }

    pub(crate) fn authorization(&self) -> [u8; 64] {
        self.0
            .as_ref()
            .expect("IssueBundle may not be null")
            .authorization()
            .signature()
            .into()
    }
}
