<picture>
  <source media="(prefers-color-scheme: dark)" srcset="/docs/images/TOTEM_logo_dark.svg">
  <source media="(prefers-color-scheme: light)" srcset="/docs/images/TOTEM_logo_bright.svg">
  <img alt="TOTEM logo font" src="/docs/images/TOTEM_logo_bright.svg">
</picture>

# ZMK config for the Totem

Colemak-DH keymap ported from Aric's Clickety Split Leeloo
(`ariccb/qmk_firmware`, branch `dev_branch`). The live keymap is
[`config/totem.keymap`](config/totem.keymap). Adaptive-key rules are in
[`config/adaptive.dtsi`](config/adaptive.dtsi). How those rules behave, and
where they differ from the QMK engine, is written up in
[docs/adaptive-keys-analysis.md](docs/adaptive-keys-analysis.md).

Hardware and the build guide: <https://github.com/GEIGEIGEIST/totem>

![TOTEM layout](/docs/images/TOTEM_layout.svg)

## How to flash

- On GitHub, open Actions and download the latest `firmware.zip`.
- Connect the left half, press reset twice, and copy
  `totem_left-seeeduino_xiao_ble-zmk.uf2` onto the drive that appears.
- Repeat with the right half and `totem_right-seeeduino_xiao_ble-zmk.uf2`.

This firmware enables mouse keys, which changes the Bluetooth HID report.
Forget the keyboard on the host and pair it again after flashing. There is
no XIAO settings-reset image; clearing bonds from the host plus `&bt BT_CLR`
on Adjust is the reset path.

The keymap editor at <https://nickcoutsos.github.io/keymap-editor/> can edit
`config/totem.keymap`. Leave the `#include "adaptive.dtsi"` line in place.
The editor keeps that include and the `&ak_*` bindings; the trigger nodes
themselves are edited in the dtsi.

## Key positions

38 keys. Bindings are listed in this order:

```
  0  1  2  3  4        5  6  7  8  9
 10 11 12 13 14       15 16 17 18 19
 20 21 22 23 24 25    26 27 28 29 30 31
       32 33 34       35 36 37
```

`20` and `31` are the outer pinky keys. Left thumbs, outer to inner, are
`32 33 34`. Right thumbs, inner to outer, are `35 36 37`.

Hold-taps below are written `hold/tap`.

## Base, Colemak-DH

```
 q   w*  f*  p*  b          j*  l*  u   y*  ;
 a   r   s   t   g          m   n   e   i   o
Esc  z   x*  c   d   v*     k   h   ,   .   /* Esc
   Shift  Nav/Tab  Num/Space    Raise/Enter  Adjust/Bsp  Del
```

A star is an adaptive key, the second key of a rule. `a r s t` and `n e i o`
are home-row mods. Both pinkies are Esc on tap, Hyper on hold.
Hyper is Left Ctrl + Left Shift + Left Alt + Left GUI.

Thumbs, and why Tab sits where it does:

| Thumb | Key | Tap | Hold |
| --- | --- | --- | --- |
| Left outer | 32 | one-shot Shift | |
| Left middle | 33 | Tab | Nav |
| Left inner | 34 | Space | Num |
| Right inner | 35 | Enter | Raise |
| Right middle | 36 | Backspace | Adjust |
| Right outer | 37 | Delete | |

Space stays on the inner thumb, which is the resting key. Tab shares the
middle thumb with Nav, between Shift and Space, so the layer holds stay on
thumbs and Tab does not take a pinky. One-shot Shift is the outer left
thumb (1 second, quick-release), matching the Leeloo one-shot shift.
Repeat is not on a thumb; it is on Nav and Adjust.

QWERTY and Canary leave the thumbs and both pinkies transparent, so they
inherit this row. `&to` keeps Base underneath them.

## QWERTY

```
 q   w*  e   r   t          y*  u   i   o   p*
 a   s   d   f*  g          h   j*  k   l*  ;
Esc  z   x*  c   v*  b      n   m   ,   .   /* Esc
        (same thumbs and pinkies as Base)
```

`f` is Shift and adaptive, `j` is Shift and adaptive, `l` is Alt and
adaptive. Those three use their own hold-taps so a tap still runs the
adaptive rule.

## Canary

The Leeloo Canary, including the Q/W swap noted in that keymap.

```
 q   l*  y*  p*  b          z   f*  o   u   ;
 c   r   s   t   g          m   n   e   i   a
Esc  w*  j*  v*  d   k      x*  h   ,   .   /* Esc
        (same thumbs and pinkies as Base)
```

## Num

Leeloo Lower, folded onto the 3x5. Digits are the number row, not the keypad,
except the operators that adaptive rules match on.

```
 !   <   >   =   ^          *   7   8   9   :
 #   [   (   )   ]          &   4   5   6   -
 \   @   $   {   }   %      0   1   2   3   /   +
   S-Tab   Tab    (held)      Enter  Backspace  Delete
```

`*` is Shift+8, so the keypad-star adaptive rule stays dormant, as it was
on the Leeloo. `-` is keypad minus, `/` is keypad slash, `+` is keypad plus,
`=` is the equals key. `0` is adaptive: after one of those operators it
sends Space.

The pinkies are not Esc/Hyper on this layer. The left one is `\`, the right
one is keypad `+`, so `+` then `0` still has a key. Other layers leave the
pinkies transparent and keep Esc/Hyper.

Terminal chords, this layer only: `8+9` opens a terminal (`Ctrl+Grave`),
`7+8` next, `9+0` previous, `7+8+9` new terminal. `7+8` is a subset of
`7+8+9`; hit all three inside the 70 ms window.

## Nav

Hold the Tab thumb.

```
 F1  F2  F3  F4  F5       C-PgUp  Home  Up   End  (unlock)
 F6  F7  F8  F9  F10      Cmd-S-Tab Left Down Right Cmd-Tab
Esc F11 F12 Bri- Bri+ Rep  C-Tab  PgUp  AddCur PgDn Cmd-Right  sticky Cmd
   (held)  (held)  Space       Enter  Backspace  Delete
```

`AddCur` is Cmd+Alt+Down (VS Code add cursor). `Cmd-Right` is browser
forward. Cmd-Tab, Cmd-Shift-Tab, and Ctrl-Tab hold the modifier until the
key is released. The Leeloo's 800 ms app-switch timer and the encoder
super-tab are not reproduced; sticky Cmd on the right pinky of this layer
is the way to tap Tab repeatedly.

The `(unlock)` key turns Nav off after it was toggled on from Adjust. Holding
this layer and tapping it does not lock the layer. ZMK keeps one bit per
layer, so toggling the layer you are holding clears it, and releasing the
thumb clears it again.

## Raise

Hold the Enter thumb.

```
 MClk RClk  Up  LClk Mute     Vol+  LClk MClk RClk (unlock)
      Left Down Right MicMute SelWd SelPart Cmd-[ Cmd-] Play
Esc  Zoom- ScrlUp ScrlDn Zoom+ Line  Vol-  Brackets Parens Arrow Next Zoom0
   (held)   Tab   Space          (held)  Backspace  Delete
```

`MicMute` is Ctrl+Alt+Cmd+M. `SelWd` is Alt+Shift+Right (repeat extends the
selection). `SelPart` is Alt+Ctrl+Shift+Right. `Line` is Cmd+Left then
Cmd+Shift+Right. `Zoom0` is Cmd+0. Brackets, parens, and the arrow snippet
follow Shift the way the Leeloo macros did: unshifted is `[]` / `()` / `() =>`,
shifted is `{}` / `<>` / the longer arrow snippet. Quotes from the `H+,`
combo do the same (`"` unshifted, `'` shifted).

Mouse movement uses ZMK's defaults (300 ms to full speed, move value 600).
The Leeloo kinetic-mouse settings are not copied.

## Adjust

Hold the Backspace thumb. This is also where Bluetooth lives; the Leeloo was
wired.

```
Reset ClearBT OutToggle BT0 BT1     Base  QWERTY Canary #  (unlock)
Boot  NextBT  BT2  BT3  BT4          Num   Nav   Raise  `  Sleep
Esc   PrevBT  Bri+ Bri- Repeat Caps  Ins  PrtSc Scroll NumLock CapsWord Esc
   (held)   Tab   Space            Enter  (held)  Delete
```

`Base`, `QWERTY`, and `Canary` are `&to`. They replace the persistent
default layer. `Num`, `Nav`, and `Raise` are toggles on this layer, which
is the lock that works: hold Adjust, tap the layer, release Adjust, and
that layer stays up. `&to Base` (or QWERTY / Canary) turns the toggled
layers off. The Leeloo's 120 second layer-lock idle timeout is not ported.

`Boot` is the bootloader. Reset-by-combo is not ported, on purpose.

## Home-row mods

Positional hold-taps, the timeless setup from the ZMK hold-tap docs.

| | Left | Right |
| --- | --- | --- |
| Pinky | GUI | GUI |
| Ring | Alt | Alt |
| Middle | Ctrl | Ctrl |
| Index | Shift | Shift |

`G` and `M` are not mods. The same positions are mods on QWERTY
(`A S D F` / `J K L ;`) and Canary (`C R S T` / `N E I A`).

| Property | Value | Effect |
| --- | --- | --- |
| flavor | balanced | A hold is a key pressed and released while the mod is still down |
| tapping-term-ms | 280 | How long a same-hand hold can still become a modifier |
| quick-tap-ms | 175 | A second tap inside this window is a tap, so double letters work |
| require-prior-idle-ms | 150 | Typing faster than this keeps the key a tap |
| hold-trigger-key-positions | the other hand, including its pinky and thumbs | A same-hand roll is a tap |
| hold-trigger-on-release | set | Same-hand mods can still be held together |

There is no global quick-tap. Layer-taps are separate: balanced, 200 ms
tapping term, 120 ms quick-tap, and no prior-idle requirement, so Num and
Nav do not need a pause.

If mods fire while typing, raise `require-prior-idle-ms` (try 200) or
`tapping-term-ms`. If mods feel late, lower `require-prior-idle-ms` toward
100 or `tapping-term-ms` toward 240. Those properties are on `hrm_l` and
`hrm_r` in `config/totem.keymap`.

## Esc / Hyper

Both outer pinkies use `&eh`.

| Property | Value | Effect |
| --- | --- | --- |
| flavor | tap-preferred | A release before the tapping term is always Esc |
| tapping-term-ms | 200 | Hyper starts once the key has been down this long |
| require-prior-idle-ms | 150 | A pinky struck during typing stays Esc |

A fast Esc, including a roll from a letter into the pinky, does not become
Hyper. Hyper+key means a short pause, then holding the pinky past 200 ms.
If Esc still turns into Hyper, raise `tapping-term-ms`. If Hyper is hard to
reach, lower `require-prior-idle-ms` or remove that line. The node is `eh`
in `config/totem.keymap`.

Num replaces the pinkies, as drawn above. Every other layer keeps them.

## Combos

50 ms, alpha layers only (Base, QWERTY, Canary). Positions follow Colemak-DH,
including while QWERTY or Canary is active. Shortcuts are Mac: GUI is Cmd.

| Chord | Result |
| --- | --- |
| W F | Undo |
| F P | Redo |
| Z X | Cut |
| X C | Copy |
| C D | Paste |
| X D | Clipboard history (Alt+Shift+Cmd+Space) |
| X V | Paste without formatting |
| Z D | Select all |
| . / | ? |
| , . | _ |
| U Y | - (a roll is the adaptive `UE` instead) |
| E I | = |
| T G | @ |
| H , | `"` , or `'` if Shift is held |
| T N | Caps word (the two index Shift mods) |
| L U Y | Num Lock |
| letter + `.` | The Leeloo HTML / snippet macros (div, p, title, href, img, style, the generic tag, `console.log`, `() =>`) |

Not ported as combos: reset (bootloader key on Adjust), the four-key sleep
chord (Sleep is a key on Adjust), F11/F12 (those keys are on Nav),
exclamation from slash+backslash, and the O+quote double-quote chord.
Quote is not on the base alpha row; `H+,` covers quotes.

## What changed from the Leeloo

The Leeloo is a 4x6 plus four thumbs and two encoders. The Totem is a 3x5
plus one outer pinky and three thumbs per side. The number row, the outer
column, and both encoders had to move.

- F-keys, brightness, and the app/tab switchers moved to Nav.
- The symbol/number layer is Num, on the Space thumb. Shifted-tab from the
  Leeloo's lower outer column is the outer left thumb on Num.
- One-shot Shift, Tab/Nav, Space/Num on the left; Enter/Raise, Backspace/Adjust,
  Delete on the right. The Leeloo's repeat thumb and play/pause thumb are
  keys on Nav and Raise.
- Esc/Hyper is the outer pinky on both sides, not a full-height outer column.
  Quote and backslash moved to Num (backslash is the left pinky there,
  quote is the `H+,` combo).
- Home-row mods replace the Leeloo's corner modifiers (Ctrl, Alt, GUI, Shift
  scattered on thumbs and outer keys).
- Encoders became keys: volume, brightness, zoom, undo/redo (combos), mic
  mute, play/pause, next track.
- Three alpha layouts stay. Switching them is `&to` on Adjust.
- Bluetooth profile select, clear, next/previous, output toggle, bootloader,
  and sleep are on Adjust.
- Shortcuts are Mac. For Windows, change `LG` to `LC` on the shortcut combos
  and the GUI home-row mods, and change word-select `LA` to `LC`. There is
  no `CG_SWAP`.
- The TVPaint layers from the previous Totem keymap are not in the Leeloo
  map and are not in this one.
- Autocorrect is not ported.

## Adaptive keys

Full write-up: [docs/adaptive-keys-analysis.md](docs/adaptive-keys-analysis.md).

The second key of a bigram is replaced when it follows its trigger within
350 ms. The first key is sent immediately. `sv` sends `sc`. `pf` sends `pt`.
The implemented set is the last enabled QMK rules (2025-07-24): `LJ LK`,
`JL JU`, `SV SC`, `CX CS`, `PF PT`, `FW FS`, `E/ E,`, `WP WR`, `BW BD`,
`UY UE`, `DX DG`, and operator-then-`0` becoming operator-then-space.
`FW`, not the older `FP` from the Leeloo readme.

Ctrl, Alt, and Cmd suppress a substitution. Shift does not.

## Try these first

1. Forget the keyboard and pair it again. Mouse keys changed the HID report.
2. `sv` → `sc`, and `e/` → `e,`. Both start on a home-row mod.
3. `pf` → `pt`, `fw` → `fs`, `lj` → `lk`, `jl` → `ju`.
4. A fast Esc on each pinky, then a roll from a letter into that pinky.
   Then a paused hold of the same key plus a letter, which should be Hyper.
5. Tab on the middle left thumb, Nav while it is held. Space/Num, Enter/Raise,
   Backspace/Adjust.
6. A same-hand roll (`as`) stays letters. A cross-hand chord (`a` held, then
   a right-hand key) is Cmd.
7. Caps word from the two index Shift keys, then a shifted bigram.
8. `U` and `Y` together produce `-`. A roll produces `UE`.
9. Hold Adjust, tap Num, release Adjust. Num should stay. `&to Base` clears it.
