use std::convert::TryInto;

use zcash_note_encryption::{
    try_output_recovery_with_ovk, EphemeralKeyBytes, ShieldedOutput,
};
use zcash_primitives::{
    consensus::BlockHeight,
    keys::OutgoingViewingKey,
    memo::MemoBytes,
    sapling::{
        self,
        note_encryption::{PreparedIncomingViewingKey, SaplingDomain},
        value::ValueCommitment,
        SaplingIvk,
    },
};
use zcash_primitives::sapling::note_encryption::{CompactNoteCiphertextBytes, NoteCiphertextBytes};

use crate::{bridge::ffi::SaplingShieldedOutput, params::Network};

/// Trial decryption of the full note plaintext by the recipient.
///
/// Attempts to decrypt and validate the given shielded output using the given `raw_ivk`.
/// If successful, the corresponding note and memo are returned, along with the address to
/// which the note was sent.
///
/// Implements section 4.19.2 of the
/// [Zcash Protocol Specification](https://zips.z.cash/protocol/protocol.pdf#decryptivk).
pub(crate) fn try_sapling_note_decryption(
    network: &Network,
    height: u32,
    raw_ivk: &[u8; 32],
    output: SaplingShieldedOutput,
) -> Result<Box<DecryptedSaplingOutput>, &'static str> {
    let ivk = parse_and_prepare_sapling_ivk(raw_ivk)
        .ok_or("Invalid Sapling ivk passed to wallet::try_sapling_note_decryption()")?;

    let (note, recipient, memo) = sapling::note_encryption::try_sapling_note_decryption(
        network,
        BlockHeight::from_u32(height),
        &ivk,
        &output,
    )
    .ok_or("Decryption failed")?;

    Ok(Box::new(DecryptedSaplingOutput {
        note,
        recipient,
        memo,
    }))
}

/// Recovery of the full note plaintext by the sender.
///
/// Attempts to decrypt and validate the given shielded output using the given `ovk`.
/// If successful, the corresponding note and memo are returned, along with the address to
/// which the note was sent.
///
/// Implements [Zcash Protocol Specification section 4.19.3][decryptovk].
///
/// [decryptovk]: https://zips.z.cash/protocol/protocol.pdf#decryptovk
pub(crate) fn try_sapling_output_recovery(
    network: &Network,
    height: u32,
    ovk: [u8; 32],
    output: SaplingShieldedOutput,
) -> Result<Box<DecryptedSaplingOutput>, &'static str> {
    let domain = SaplingDomain::for_height(*network, BlockHeight::from_u32(height));

    let cv = Option::from(ValueCommitment::from_bytes_not_small_order(&output.cv))
        .ok_or("Invalid output.cv passed to wallet::try_sapling_note_decryption()")?;

    let (note, recipient, memo) = try_output_recovery_with_ovk(
        &domain,
        &OutgoingViewingKey(ovk),
        &output,
        &cv,
        &output.out_ciphertext,
    )
    .ok_or("Decryption failed")?;

    Ok(Box::new(DecryptedSaplingOutput {
        note,
        recipient,
        memo,
    }))
}

/// Parses and validates a Sapling incoming viewing key, and prepares it for decryption.
pub(crate) fn parse_and_prepare_sapling_ivk(
    raw_ivk: &[u8; 32],
) -> Option<PreparedIncomingViewingKey> {
    jubjub::Fr::from_bytes(raw_ivk)
        .map(|scalar_ivk| PreparedIncomingViewingKey::new(&SaplingIvk(scalar_ivk)))
        .into()
}

impl ShieldedOutput<SaplingDomain<Network>> for SaplingShieldedOutput {
    fn ephemeral_key(&self) -> EphemeralKeyBytes {
        EphemeralKeyBytes(self.ephemeral_key)
    }

    fn cmstar_bytes(&self) -> [u8; 32] {
        self.cmu
    }

    fn enc_ciphertext(&self) -> Option<NoteCiphertextBytes> {
        Some(NoteCiphertextBytes(self.enc_ciphertext))
    }

    fn enc_ciphertext_compact(&self) -> CompactNoteCiphertextBytes {
        todo!()
    }
}

/// A Sapling output that we successfully decrypted with an `ivk`.
pub(crate) struct DecryptedSaplingOutput {
    note: sapling::Note,
    recipient: sapling::PaymentAddress,
    memo: MemoBytes,
}

impl DecryptedSaplingOutput {
    pub(crate) fn note_value(&self) -> u64 {
        self.note.value().inner()
    }

    pub(crate) fn note_rseed(&self) -> [u8; 32] {
        match self.note.rseed() {
            sapling::Rseed::BeforeZip212(rcm) => rcm.to_bytes(),
            sapling::Rseed::AfterZip212(rseed) => *rseed,
        }
    }

    pub(crate) fn zip_212_enabled(&self) -> bool {
        matches!(self.note.rseed(), sapling::Rseed::AfterZip212(_))
    }

    pub(crate) fn recipient_d(&self) -> [u8; 11] {
        self.recipient.diversifier().0
    }

    pub(crate) fn recipient_pk_d(&self) -> [u8; 32] {
        self.recipient.to_bytes()[11..].try_into().unwrap()
    }

    pub(crate) fn memo(&self) -> [u8; 512] {
        *self.memo.as_array()
    }
}
