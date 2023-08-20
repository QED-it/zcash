use std::convert::TryInto;

use tracing::{debug, error};

use crate::{
    bundlecache::{issue_bundle_validity_cache, issue_bundle_validity_cache_mut, CacheEntries},
    issue_bundle::IssueBundle,
};

struct BatchValidatorInner {
    validator: orchard::issuance::BatchValidator,
    queued_entries: CacheEntries,
}

pub struct BatchValidator(Option<BatchValidatorInner>);

/// Creates an Issue bundle batch validation context.
pub fn issue_batch_validation_init(cache_store: bool) -> Box<BatchValidator> {
    Box::new(BatchValidator(Some(BatchValidatorInner {
        validator: orchard::issuance::BatchValidator::new(),
        queued_entries: CacheEntries::new(cache_store),
    })))
}

impl BatchValidator {
    /// Adds an Issue bundle to this batch.
    pub(crate) fn add_bundle(&mut self, bundle: Box<IssueBundle>, sighash: [u8; 32]) {
        let batch = self.0.as_mut();
        let bundle = bundle.inner();

        match (batch, bundle) {
            (Some(batch), Some(bundle)) => {
                let cache = issue_bundle_validity_cache();

                // Compute the cache entry for this bundle.
                let cache_entry = {
                    let bundle_commitment = bundle.commitment();
                    let bundle_authorizing_commitment = bundle.authorizing_commitment();
                    cache.compute_entry(
                        bundle_commitment.0.as_bytes().try_into().unwrap(),
                        bundle_authorizing_commitment
                            .0
                            .as_bytes()
                            .try_into()
                            .unwrap(),
                        &sighash,
                    )
                };

                // Check if this bundle's validation result exists in the cache.
                if !cache.contains(cache_entry, &mut batch.queued_entries) {
                    // The bundle has been added to `inner.queued_entries` because it was not
                    // in the cache. We now add its authorization to the validation batch.
                    batch.validator.add_bundle(bundle, sighash);
                }
            }
            (Some(_), None) => debug!("Tx has no Issue component"),
            (None, _) => error!("orchard::issuance::BatchValidator has already been used"),
        }
    }

    /// Validates this batch.
    ///
    /// - Returns `true` if `batch` is null.
    /// - Returns `false` if any item in the batch is invalid.
    ///
    /// The batch validation context is freed by this function.
    pub(crate) fn validate(&mut self) -> bool {
        if let Some(inner) = self.0.take() {
            // FIXME: TODO: consider uncommenting this and adding ISSUE_VK
            //let vk = unsafe { crate::ISSUE_VK.as_ref() }
            //    .expect("Parameters not loaded: ISSUE_VK should have been initialized");
            if inner.validator.validate() {
                // `BatchValidator::validate()` is only called if every
                // `BatchValidator::check_bundle()` returned `true`, so at this point
                // every bundle that was added to `inner.queued_entries` has valid
                // authorization.
                issue_bundle_validity_cache_mut().insert(inner.queued_entries);
                true
            } else {
                false
            }
        } else {
            error!("orchard::issuance::BatchValidator has already been used");
            false
        }
    }
}
