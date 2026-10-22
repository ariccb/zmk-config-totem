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
  `totem_left-xiao_ble__zmk-zmk.uf2` onto the drive that appears.
- Repeat with the right half and `totem_right-xiao_ble__zmk-zmk.uf2`.

This firmware enables mouse keys, which changes the Bluetooth HID report.
Forget the keyboard on the host and pair it again after flashing. There is
no XIAO settings-reset image; clearing bonds from the host plus `&bt BT_CLR`
on Adjust or System is the reset path. Clearing bonds does not clear the
saved Mac/Windows choice.

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

Terminal chords, this layer only: `8+9` opens a terminal (`Ctrl+Grave` on
Mac, `Win+Grave` in Windows mode), `7+8` next, `9+0` previous, `7+8+9` new
terminal. `7+8` is a subset of `7+8+9`; hit all three inside the 70 ms
window. Next and previous are Shift+Cmd+[ / ] on Mac and Shift+Ctrl+[ / ]
in Windows mode.

## Nav

Hold the Tab thumb.

```
 F1  F2  F3  F4  F5       C-PgUp  Home  Up   End  (unlock)
 F6  F7  F8  F9  F10      Cmd-S-Tab Left Down Right Cmd-Tab
Esc F11 F12 Bri- Bri+ Rep  C-Tab  PgUp  AddCur PgDn Fwd  sticky Cmd
   (held)  (held)  Space       Enter  Backspace  Delete
```

`AddCur` is Cmd+Alt+Down (VS Code add cursor). `Fwd` is browser forward
(Cmd+Right on Mac, Alt+Right on Windows). Cmd-Tab, Cmd-Shift-Tab, and
Ctrl-Tab hold the modifier until the key is released. In Windows mode those
follow the Ctrl/Cmd swap, so Cmd-Tab becomes Ctrl-Tab and Ctrl-Tab becomes
Win-Tab. Alt-Tab is not substituted. The Leeloo's 800 ms app-switch timer
and the encoder super-tab are not reproduced; sticky Cmd on the right pinky
of this layer is the way to tap Tab repeatedly. Windows mode makes that
sticky key Ctrl.

The `(unlock)` key turns Nav off after it was toggled on from Adjust. Holding
this layer and tapping it does not lock the layer. ZMK keeps one bit per
layer, so toggling the layer you are holding clears it, and releasing the
thumb clears it again.

## Raise

Hold the Enter thumb.

```
 MClk RClk  Up  LClk Mute     Vol+  LClk MClk RClk (unlock)
      Left Down Right MicMute SelWd SelPart Back Fwd Play
Esc  Zoom- ScrlUp ScrlDn Zoom+ Line  Vol-  Brackets Parens Arrow Next Zoom0
   (held)   Tab   Space          (held)  Backspace  Delete
```

`MicMute` is Ctrl+Alt+Cmd+M, so swapping Ctrl and Cmd does not change it.
`SelWd` is Alt+Shift+Right (repeat extends the selection). `SelPart` is
Alt+Ctrl+Shift+Right, which becomes Alt+Win+Shift+Right in Windows mode.
`Back` and `Fwd` are browser back and forward: Cmd+[ and Cmd+] on Mac,
Alt+Left and Alt+Right on Windows. `Line` is Cmd+Left then Cmd+Shift+Right
(Ctrl+Left then Ctrl+Shift+Right in Windows mode). `Zoom0` is Cmd+0
(Ctrl+0 in Windows mode), and the zoom-out / zoom-in keys next to the
scroll keys follow the same swap. Brackets, parens, and the arrow snippet
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
These Bluetooth, boot, and reset keys are duplicated on System. Adjust
keeps them so one thumb is still enough.

## System

Hold Num and Raise together. Those are the two inner thumbs: Space/Num on
the left, Enter/Raise on the right. Each is already a layer key, and they
sit under the resting position, one hand each. Nav stays on the middle Tab
thumb and Adjust stays on the Backspace thumb, so those remain one-thumb
layers.

ZMK turns System on only while both of those layers are held
(`conditional_layers` on Num + Raise). System is not also bound to its own
hold or toggle. Let go of either thumb and System drops. While it is up it
covers Num and Raise.

```
 Mac  Win  Out  BT0  BT1       Clear  Save  Find
 Boot Rst  BT2  BT3  BT4                            Sleep

           (held Num)              (held Raise)
```

The outer pinkies are blank on this layer, so the chord does not fire
Esc/Hyper or Num's `\` and `+`.

| Key | What it sends |
| --- | --- |
| Mac | Mac shortcut mode. Cmd is Cmd, and this is the boot default |
| Win | Windows shortcut mode. Ctrl and Cmd swap, including the home-row mods |
| Out | Toggle USB and Bluetooth output |
| BT0–BT4 | Select Bluetooth profile 0 through 4 |
| Clear | `&bt BT_CLR`, clear the current profile |
| Boot | Bootloader |
| Rst | `&sys_reset` |
| Save | Cmd+S, or Ctrl+S in Windows mode |
| Find | Cmd+F, or Ctrl+F in Windows mode |
| Sleep | System sleep |

Save and Find were not Leeloo combos. They live here so both modes have
them. ZMK Studio is not enabled in this build, so there is no
`&studio_unlock` key.

Release Ctrl and Cmd before pressing Mac or Win. A modifier that is already
down is released with the other mode's code and can stick on the host.

## Mac and Windows

The keymap is written for Mac. Home-row pinkies are GUI (Cmd), home-row
middles are Ctrl, and the shortcut combos are Cmd chords. Windows mode does
not replace those nodes and does not add a second set of hold-taps, so the
timing properties on `hrm_l` and `hrm_r` apply to both modes.

A listener on the central half rewrites the report after sticky key, caps
word, and key repeat have seen the original code, and before USB/BLE HID
sends it. Left Ctrl and Left GUI trade places, and the same for the right
side. Shift and Alt stay put. A chord that already contains both Ctrl and
Cmd (Hyper, mic mute) is unchanged.

The listener is not a layer, so switching alpha layout with `&to` does not
clear it. The choice is stored in ZMK settings (`totem/os/win`) and comes
back after a reboot. On a fresh board, before either key has been pressed,
the mode is Mac. Uncomment `CONFIG_TOTEM_OS_MODE_DEFAULT_WINDOWS` in
`config/totem.conf` to make that first boot Windows instead. After a Mac or
Windows key press, the saved value wins over that setting. `&bt BT_CLR`
does not erase it. There is no XIAO settings-reset firmware; press the other
mode key to change it back.

A straight swap is wrong for a few Leeloo shortcuts, so those are their own
behaviors in `config/os.dtsi` and do not get swapped a second time:

| Key | Mac | Windows |
| --- | --- | --- |
| Redo (F P) | Cmd+Shift+Z | Ctrl+Y |
| Clipboard history (X D) | Alt+Shift+Cmd+Space | Win+V |
| Nav `Fwd` | Cmd+Right | Alt+Right |
| Raise `Back` | Cmd+[ | Alt+Left |
| Raise `Fwd` | Cmd+] | Alt+Right |

Everything else that is Cmd in the keymap becomes Ctrl, and a bare Ctrl
becomes Win. That includes undo, cut, copy, paste, paste-without-formatting,
select-all, save, find, zoom, add-cursor, select-line, the HTML tag's
Cmd+D, and the home-row mods. Terminal chords on Num follow the same rule:
Ctrl+Grave becomes Win+Grave, and Shift+Cmd+[ / ] become Shift+Ctrl+[ / ].

The Leeloo did this differently. `guess_os.h` picks Mac or not when the
firmware is compiled (`MAC_HOTKEYS` when the host OS is macOS or iOS). That
is not a key. The compiled Mac set sends GUI for undo, cut, copy, paste,
select-all, and paste-plain, redo is GUI+Shift+Z, clipboard history is
Alt+Shift+GUI+Space, browser back/forward and zoom use GUI, and search is
GUI+F. The Windows compile uses Ctrl for those, redo is Ctrl+Y, clipboard
history is Win+V, and browser back/forward are Alt+Left / Alt+Right. Some
mod-taps are also compiled the other way around (Enter's hold is Ctrl
instead of GUI, Z's hold is GUI instead of Alt). On top of that, Raise has
QMK's `CG_NORM` and `CG_SWAP` magic keycodes, which swap Ctrl and GUI in
the report at runtime and are stored in EEPROM. Those magic keys do not
switch the compiled shortcut set, so a Mac build with `CG_SWAP` turns
Cmd+Shift+Z into Ctrl+Shift+Z rather than Ctrl+Y. There is no runtime key
that flips the whole `MAC_HOTKEYS` block. The Leeloo also has no home-row
Ctrl/Cmd: those modifiers are thumbs and outer keys (GUI on the Enter
thumb, Ctrl on another thumb, right Ctrl on quote, Alt on Z).

This port's home-row mods are new. Before this toggle, every shortcut on
the Totem was the Mac chord, and Windows meant editing `LG` to `LC` by
hand. `CG_SWAP` / `CG_NORM` were not ported; Mac and Win on System replace
them and also cover the shortcuts the magic swap gets wrong.

## Home-row mods

Positional hold-taps, the timeless setup from the ZMK hold-tap docs.

Mac mode is drawn below. Windows mode swaps pinky and middle on both
hands (pinky becomes Ctrl, middle becomes Win) and leaves Alt and Shift
where they are.

| | Left | Right |
| --- | --- | --- |
| Pinky | GUI / Cmd | GUI / Cmd |
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

Both outer pinkies use `&eh 0 ESC`. Hold-taps always take two parameters, and the `0` is unused because Hyper takes none.

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

Num replaces the pinkies, as drawn above. System leaves them blank. Every
other layer keeps Esc/Hyper.

## Combos

50 ms, alpha layers only (Base, QWERTY, Canary). Positions follow Colemak-DH,
including while QWERTY or Canary is active. Mac mode is the default.
Windows mode is the System-layer `Win` key, described above.

| Chord | Mac | Windows |
| --- | --- | --- |
| W F | Undo, Cmd+Z | Ctrl+Z |
| F P | Redo, Cmd+Shift+Z | Ctrl+Y |
| Z X | Cut, Cmd+X | Ctrl+X |
| X C | Copy, Cmd+C | Ctrl+C |
| C D | Paste, Cmd+V | Ctrl+V |
| X D | Clipboard history, Alt+Shift+Cmd+Space | Win+V |
| X V | Paste without formatting, Cmd+Shift+V | Ctrl+Shift+V |
| Z D | Select all, Cmd+A | Ctrl+A |
| . / | ? | same |
| , . | _ | same |
| U Y | - (a roll is the adaptive `UE` instead) | same |
| E I | = | same |
| T G | @ | same |
| H , | `"` , or `'` if Shift is held | same |
| T N | Caps word (the two index Shift mods) | same |
| L U Y | Num Lock | same |
| letter + `.` | The Leeloo HTML / snippet macros (div, p, title, href, img, style, the generic tag, `console.log`, `() =>`) | same, except the generic tag's Cmd+D becomes Ctrl+D |

Not ported as combos: reset (bootloader key on Adjust and System), the
four-key sleep chord (Sleep is a key on Adjust and System), F11/F12 (those
keys are on Nav),
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
  and sleep are on Adjust. System repeats profile select, clear, output
  toggle, bootloader, reset, and sleep, and adds Save and Find. Adjust's
  copies stay, so one thumb still reaches Bluetooth.
- Shortcuts start as Mac. System's Mac and Win keys swap Ctrl and Cmd at
  runtime, including home-row mods and the shortcut combos, and they keep
  the Leeloo exceptions (redo is Ctrl+Y on Windows, clipboard history is
  Win+V, browser back/forward are Alt+arrows). The Leeloo's compile-time
  `MAC_HOTKEYS` block and its `CG_SWAP` / `CG_NORM` magic keys are that
  story on the QMK side; see Mac and Windows above. The choice is saved
  across reboots. `config/totem.conf` can select the default for a board
  that has never pressed either key.
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
   a right-hand key) is Cmd in Mac mode and Ctrl in Windows mode.
7. Caps word from the two index Shift keys, then a shifted bigram.
8. `U` and `Y` together produce `-`. A roll produces `UE`.
9. Hold Adjust, tap Num, release Adjust. Num should stay. `&to Base` clears it.
10. Hold Space/Num and Enter/Raise together. System should come up. `Win`,
    then W+F is Ctrl+Z and F+P is Ctrl+Y. `Mac` puts those back to Cmd+Z and
    Cmd+Shift+Z. Power-cycle and the last choice should still be selected.
