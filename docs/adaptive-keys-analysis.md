# Adaptive keys

This is the behavior of the adaptive-key engine in Aric's Leeloo QMK keymap
(`keyboards/clickety_split/leeloo/keymaps/ariccb` on `dev_branch`), and how
the Totem firmware reproduces it.

The rules below are the last set that actually ran, from commit `0683ce8793`
(2025-07-24, "update layout 2025 july 24"). The copy of `adaptive_keys.def`
on the branch tip (`d67098eb8f`, 2025-07-25, "added canary keymap") is
entirely commented out, so that firmware sends nothing. The comment-out also
rewrote two rules, which looks accidental: `LJ → LK` became `LL → LU`, and
`JL → JU` disappeared. This port follows the 2025-07-24 rules. If the
comment-out was intentional, say so and the nodes in `config/adaptive.dtsi`
can be emptied.

Autocorrect is a separate feature and is not ported.

## What a rule does

Every enabled rule is `AK_SND_ONLY`. The first key is sent immediately. If
the second key is pressed within 350 ms, the second key is replaced. A
second key that is not in the rule is sent unchanged.

Typing `sv` therefore sends `s` and then `c`. Typing `pf` sends `p` and then
`t`. Typing `pq` sends `p` and then `q`.

There is an older `AK_BOTH` mode in the engine that holds the first key back
until the second key arrives or the timer expires. None of the enabled rules
use it. The `Q → !` block at the top of the `.def` file was only ever an
example and stays commented out. An early version also delayed `P`, `U`, and
`D` and used a following space, `?`, or `!` to flush them. That was removed
so the first letter would not lag.

## Rules

| First | Second | Output | Why it is there |
| --- | --- | --- | --- |
| L | J | LK | Same-finger `LK`, rolled into the index stretch |
| J | L | JU | `JU` stretch |
| S | V | SC | `SC` |
| C | X | CS | `CS` |
| P | F | PT | `option` and similar |
| F | W | FS | `FS`. The Leeloo readme still says `FP → FS`. The code has been `FW → FS` since 2022-09-07 (`af2269f3`) |
| E | / | E, | Comma without leaving the right hand |
| W | P | WR | `WR`. An older `PW → PR` rule was removed so it would not slow `PT` |
| B | W | BD | `BD` |
| U | Y | UE | `UE` |
| D | X | DG | `DG`. An older `DZ → DB` rule was removed |
| Keypad `-` | `0` | `-` then Space | Operator, then a space, from the number row |
| `=` | `0` | `=` then Space | Same |
| Keypad `+` | `0` | `+` then Space | Same. On the Leeloo this `+` lived on the outer column |
| Keypad `/` | `0` | `/` then Space | Same. The comment in the `.def` says "backslash"; the keycode is `KC_PSLS`, keypad slash |
| Keypad `*` | `0` | `*` then Space | Dormant. The number layer's `*` is `KC_ASTR` (shifted 8), not `KC_PAST`, so this trigger is never pressed |

`X` has two triggers (`C` and `D`). `W` has two (`F` and `B`). `0` has five.
The first matching trigger wins. In the ZMK nodes, `CX` is listed before `DX`,
and the operator rules are listed in the order above.

`U` then `Y` is also a combo when the two keys land together. A roll inside
350 ms becomes `UE`. A chord inside the 50 ms combo window becomes `-`.
That overlap exists in the QMK keymap too.

## Timing

`ADAPTIVE_TERM` is 350 (`config.h`). The window opens at the prior key's
keydown and is measured with `timer_elapsed32(prior_keydown)`.

`matrix_scan_user()` comments that the timeout is 30 seconds. The code uses
`ADAPTIVE_TERM`. For `AK_SND_ONLY` the timeout's "default key" is `KC_NO`,
so the scan emits nothing: the first key was already sent. The scan then
clears `prior_keycode`. A late second key is an ordinary key.

ZMK uses `max-prior-idle-ms = <350>` on each trigger, which is the same
window measured between the two press timestamps.

## Modifiers

On the way into `process_adaptive_key()`:

- If the current key's mods include `MOD_MASK_CAG` (Ctrl, Alt, or GUI, on
  either side), the function returns immediately and does **not** update
  `prior_keycode`. Shift is not in that mask. `Ctrl+V` shortly after `S`
  stays `Ctrl+V`. A held Ctrl also does not become the new prior, so the
  next letter can still adapt against the letter before the shortcut.
- Shift is allowed. The prior key is stored as `keycode & 0xFF` only when
  the press fell inside the term. Outside the term the full keycode is
  stored, so a mod-tap's full 16-bit code will not match a letter rule
  unless it was seen inside the window.

On a real substitution, and only when caps word is off, QMK does
`set_mods(prior_saved_mods)`, taps the first key (`KC_NO` for these rules),
`clear_mods()`, then taps the replacement. The replacement is unshifted,
and mods are left cleared afterwards. That last part is a quirk: a shift
held across the bigram is released as a side effect.

The ZMK port does not copy the cleared-mods quirk. Ctrl, Alt, and GUI still
suppress adaptation and do not replace the prior key. Shift does not
suppress it, and a shift that is held stays held, so the replacement follows
the shift. Caps word is the case that should still capitalize the
replacement; that path is below.

## Caps word and caps lock

When caps word is on, both the (empty) first tap and the replacement are
tapped with a weak left shift, and the prior mods are not restored.

Caps lock is not special-cased. Weak shift on top of host caps lock can
invert the case. The ZMK caps-word behavior adds an implicit shift, which
is not an explicit Ctrl/Alt/GUI mod, so caps word does not suppress a rule.
Whether the replacement comes out capitalized depends on ZMK's caps-word
listener seeing the keycode the adaptive behavior emits. Worth confirming
on the keyboard: turn caps word on, type `sv`, and expect `SC`.

## What resets the prior key

- Every press stores the incoming key as the new prior and restarts the
  timer, including a key that just adapted. After `sv → sc` the prior is
  `V`, the key that was pressed, not `C`, the key that was sent. `svx`
  does not become `sc` plus a `CX` rule. `CX` only fires when `C` itself
  was the previous press.
- Releasing a key does not clear the prior. `S`, release, `V` within 350 ms
  still becomes `SC`.
- The CAG early-return does not clear the prior.
- The matrix scan clears the prior after 350 ms.
- A repeated key goes through `process_record` again, so a repeat can adapt
  a second time. ZMK's `&key_repeat` repeats the last HID keycode and does
  not re-enter the adaptive behavior. Repeating an adapted `C` sends another
  `C` and does not run a new rule.

## Layers, combos, home-row mods

Rules are keycode checks. There is no layer gate. They fire on Colemak-DH,
QWERTY, and Canary whenever the second key is an adaptive key.

Combos are simultaneous and adaptive keys are sequential. QMK
`COMBO_TERM` is `TAPPING_TERM / 5` (40 ms). The Totem combos use 50 ms,
and 70 ms for the three-key ones. `U+Y` is the pair that is both.

Home-row mods are new on the Totem; the Leeloo did not put these letters
under hold-taps. Two rules start on a home-row mod on Colemak-DH:

- `S` is left Shift (middle finger), and `V` is the same hand (`SV → SC`).
- `E` is right Shift (middle finger), and `/` is the same hand (`E/ → E,`).

The hold-taps are positional and `balanced`, with `hold-trigger-on-release`.
A same-hand follower is captured, the home-row key is then decided as a tap
because that follower is not a hold-trigger position, the tap keycode is
sent, and the captured key is replayed. The adaptive behavior therefore
sees `S` before it handles `V`. `require-prior-idle-ms` also forces a tap
when the previous key was recent, which is the fast-typing case. This is
the interaction to try first on hardware. `PF`, `FW`, `LJ`, `JL`, and the
other Colemak pairs do not start on a home-row mod. `G` and `M` are Cmd
holds, but no rule uses them as the first letter of a pair.

On QWERTY, `F`, `J`, and `L` are both home-row mods and adaptive keys.
They use dedicated hold-taps (`qw_f`, `qw_j`, `qw_l`) whose tap binding is
the adaptive behavior. `F` and `J` hold Ctrl. `L` holds Alt. A rolled `PF`
on QWERTY can still become Ctrl instead of `PT` when `F` is held while `P`
(a hold-trigger position on the other hand) is released. Colemak `PF` does
not have that overlap. Canary's adaptive letters are not on home-row-mod
positions. Canary `S` is a home-row Shift and is the trigger for `V`, same
as Colemak. Windows mode swaps Ctrl and GUI after the adaptive behavior
has chosen the letter, and both modifiers stay in the suppress mask.

## ZMK implementation

The behavior is urob's `zmk-adaptive-key` (v0.3.0, `e987227`), vendored
under `config/` as the Zephyr module `totem_adaptive_key` so it can be
patched. `config/west.yml` does not add a git project. The driver is
compiled into the central half only; the peripheral does not run the keymap
and does not link the HID helpers the driver calls.

Each second key is a `zmk,behavior-adaptive-key` node in
`config/adaptive.dtsi`, included from `config/totem.keymap`. One binding
means "send this key". A child node is "if the prior key was the trigger
and the idle time is under 350 ms, send the replacement instead". No dead
keys are configured, which matches `AK_SND_ONLY`: the first key is never
swallowed.

Two patches in `config/src/behaviors/behavior_adaptive_key.c` line the
module up with the QMK engine:

- While Ctrl, Alt, or GUI is an explicit mod, the behavior sends its
  default key and does not record a new prior. The listener also ignores
  key-down events in that state, so a shortcut cannot replace the prior.
  Windows mode swaps Ctrl and GUI in the report, but both stay in this
  mask, so a shortcut still suppresses a substitution in either mode.
- After the chosen binding is sent, the prior is set from the default
  binding (`V`), not from the key that was emitted (`C`). The stock module
  records the emitted keycode, which would make `SV` then `X` look like
  `CX`.

`strict-modifiers` is left off, so a trigger with no mods matches a shifted
prior. Shift is not part of the suppress mask.

The keymap editor does not load behavior nodes out of an included `.dtsi`.
It keeps the `#include "adaptive.dtsi"` line and the `&ak_*` references.
Edit the rules in the dtsi. Deleting the include drops every adaptive key.

## Gaps

- The 2025-07-25 comment-out is not what runs here. See the top of this file.
- `AK_BOTH` (delayed first key) is not implemented. Nothing enabled uses it.
- The QMK "leave mods cleared" side effect is not reproduced. A held shift
  still applies to the replacement.
- `&key_repeat` does not re-run a rule.
- Keypad `*` then `0` has no key that sends keypad `*`. The number layer's
  `*` is `STAR`.
- Caps word plus a substitution has not been confirmed on hardware.
- `SV` and `E/` depend on the home-row mod emitting its tap before the
  captured follower is replayed. That is what the current ZMK hold-tap
  source does; it is still the first thing to try on the keyboard.
- Combos are limited to the alpha layers (and the terminal chords to Num).
  QMK's `combo_ref_from_layer` made them positional on every layer. Symbol
  layers were eating letter combos, so the ZMK combos name their layers.
