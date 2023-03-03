use orchard::issuance::Signed;

#[cxx::bridge(namespace = "issue_bundle")]
mod ffi {
    extern "Rust" {
        type IssueNote;
        type IssueAction;
        type IssueBundle;
        type TxIssueBundle;

        // Issue Note
        fn recipient(self: &IssueNote) -> [u8; 43];
        fn value(self: &IssueNote) -> u64;
        fn asset(self: &IssueNote) -> [u8; 32];
        fn rho(self: &IssueNote) -> [u8; 32];
        fn rseed(self: &IssueNote) -> [u8; 32];

        // Issue Action
        fn asset_desc(self: &IssueAction) -> Vec<u8>;
        fn notes(self: &IssueAction) -> Vec<IssueNote>;
        fn is_finalized(self: &IssueAction) -> bool;

        // Issue Bundle
        unsafe fn from_tx_bundle(bundle: *const TxIssueBundle) -> Box<IssueBundle>;
        fn ik(self: &IssueBundle) -> [u8; 32];
        fn actions(self: &IssueBundle) -> Vec<IssueAction>;
        fn num_actions(self: &IssueBundle) -> usize;
        fn authorization(self: &IssueBundle) -> [u8; 64]; // Assumption is that we only need auth for IssueBundle<Signed> here, ignoring empty for Unauthorized and sighash for Prepared
    }
}

pub struct IssueNote(orchard::note::Note);

impl IssueNote {

    fn recipient(self: &IssueNote) -> [u8; 43] {
        self.0.recipient().to_raw_address_bytes()
    }

    fn value(self: &IssueNote) -> u64 {
        self.0.value().inner()
    }

    fn asset(self: &IssueNote) -> [u8; 32] {
        self.0.asset().to_bytes()
    }

    fn rho(self: &IssueNote) -> [u8; 32] {
        self.0.rho().to_bytes()
    }

    fn rseed(self: &IssueNote) -> [u8; 32] {
        *self.0.rseed().as_bytes()
    }
}

pub struct IssueAction(orchard::issuance::IssueAction);

impl IssueAction {

    fn asset_desc(self: &IssueAction) -> Vec<u8> {
        self.0.asset_desc().as_bytes().to_vec()
    }

    fn notes(self: &IssueAction) -> Vec<IssueNote> {
        self.0.notes().iter().map(|it| IssueNote(*it)).collect()
    }

    fn is_finalized(self: &IssueAction) -> bool {
        self.0.is_finalized()
    }
}

pub struct IssueBundle(Option<orchard::issuance::IssueBundle<Signed>>);
pub struct TxIssueBundle;

unsafe fn from_tx_bundle(bundle: *const TxIssueBundle) -> Box<IssueBundle> {
    Box::new(IssueBundle(
        { (bundle as *const orchard::issuance::IssueBundle<Signed>).as_ref() }.cloned(),
    ))
}

impl IssueBundle {
    fn actions(&self) -> Vec<IssueAction> {
        self.0
            .iter()
            .flat_map(|b| b.actions().iter())
            .cloned()
            .map(IssueAction)
            .collect()
    }

    fn num_actions(&self) -> usize {
        self.0.as_ref().map(|b| b.actions().len()).unwrap_or(0)
    }

    fn ik(&self) -> [u8; 32] {
        self.0
            .as_ref()
            .expect("IssueBundle may not be null")
            .ik()
            .to_bytes()
    }

    fn authorization(&self) -> [u8; 64] {
        self.0
            .as_ref()
            .expect("IssueBundle may not be null")
            .authorization()
            .signature()
            .into()
    }
}
