# Change requests to the controller

These items were found while designing the HMI. They belong to the controller repository
(`robot_charge_controller`) and must go through that repository's controlled change
process (ICD/FDD). The HMI must **not** assume any of them has been done.

| ID | Title | Effect on the HMI | Related |
|---|---|---|---|
| CR-01 | Tell a peer which interface is the active control interface | The HMI cannot know whether it may START/STOP until it is rejected | ICD §10, §11.1 |
| CR-02 | Freeze fault and inhibit bit positions in the ICD | The HMI decodes `active_fault_mask` / `inhibit_mask` by bit positions that exist only in firmware source | ICD §11.1.1, FDD-004 |
| CR-03 | `UART.md` §7 says the operational UART may send unsolicited EVENTs; the firmware sends none | Two documents disagree; the HMI polls | `UART.md` §7, ICD §14.2, `ICD-OPEN-011` |
| CR-04 | Production authorization of the operational node | Without it the HMI can control only a bench image | `FDD10-OPEN-005`, `ICD-OPEN-006` |
| CR-05 | Bind ledger retention and timeouts | The HMI needs a known safe retry window | `ICD-OPEN-005` |

## CR-01: Report the active control interface

**Problem.** `GET_CONFIG` is denied on the operational UART; GET_DEVICE_INFO has only
build flag bit2 (operational node trusted or not), not `operational_interface`. The HMI
learns that it lacks authority only when it receives `REJECTED/UNAUTHORIZED_SOURCE`,
which is exactly when an operator has just pressed STOP.

**Proposal.** Add to the GET_STATUS response (schema 2) or GET_DEVICE_INFO one byte
`operational_interface` (values of the `source_interface` enumeration) and a flag "the
asking interface is the control interface". Read-only information; it grants nothing.

**HMI workaround until then.** Treat authority as "unknown" until the first START/STOP;
warn in advance that STOP may be rejected.

## CR-02: Freeze bit positions

**Problem.** The ICD defines `active_fault_mask` and `inhibit_mask` as bitmasks but does
not assign bits. The current positions (fault bits 0–18 per `rcc_fault_registry.h`;
inhibit bit0 REMOTE, bit1 RESET, bit2 RECOVERY per `rcc_inhibit.h`) can move when the
registry changes, and the HMI would then name faults wrongly.

**Proposal.** Put the bit tables in ICD §11.1.1 (or `CAN.md` §9) as a versioned registry,
with the rule "show unknown bits raw".

## CR-03: Unsolicited events on UART

**Problem.** `UART.md` §7 says the controller "may send unsolicited EVENTs on the
operational UART"; `rcc_runtime_serial.c` says "No events are sent unsolicited on either
link", and ICD §14.2 says event delivery on RS485/UART "is not bound yet".

**Proposal.** Correct `UART.md` to match the firmware, or decide to send events and update
firmware and ICD together. The HMI polls and must ignore EVENT objects safely if they ever
appear.

## CR-04: Production authorization of the operational node

**Problem.** `operational_node_authorized` is enabled only by a bench knob. An HMI that
controls the charger cannot run with an ordinary image
([HMI-D02](decisions.md#hmi-d02-control-authority) uses a bench image for development).

**Proposal.** Covered by `FDD10-OPEN-005` (controlled bench-enable mechanism and artifact
allowlists) and `ICD-OPEN-006` (peer identity). To be decided in the controller
repository; the HMI only records the dependency.

## CR-05: Retention and timeouts

**Problem.** The 2 s ledger retention and the ledger depths are provisional. The HMI needs
the window within which a retry with the same `request_id` counts as a duplicate.

**Proposal.** Bind `ICD-OPEN-005`; until then the HMI limits automatic retries to about
150 ms, per `UART.md` §7, far below 2 s.

## Note: controller time stays unsynchronized

Only the active control interface may send `SET_TIME`. With the HMI as that interface
([HMI-D01](decisions.md#hmi-d01-role-of-the-hmi)) and not sending `SET_TIME`
([HMI-D07](decisions.md#hmi-d07-time-source)), the controller's UTC annotation stays
unsynchronized; event times are relative only. This is not a change request, only a
consequence to keep in mind if wall-clock event times are ever required.
