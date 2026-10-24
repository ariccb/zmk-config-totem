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

This firmware still enables the pointing HID report, so a host that paired
before mouse keys existed needs to forget the keyboard and pair again.
Lower itself no longer sends move, scroll, or clicks. There is
no settings_reset image in this build. Clearing bonds from the host plus
`&bt BT_CLR` on Adjust or System drops the Bluetooth profile and does not
clear the saved Mac/Windows choice. A fresh board boots in Windows mode.
A board that already saved a mode keeps it until the System-layer Win key
is pressed once, or until settings are wiped. See Mac and Windows.

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
A/Nv Alt Sft Ctl Cmd       Cmd Ctl Sft Alt  o
Esc  z   x*  c   d   v*     k   h   ,   .   /* Bsp
   Shift  Nav/Tab  Lower/Space  Sym/Enter  Adjust/Bsp  Del
```

A star is an adaptive key, the second key of a rule. Home-row mods, Mac
mode: ring Alt, middle Shift, index Ctrl, inner column Cmd (`g` and `m`).
`a` taps A and holds Nav. `o` is a plain letter. The left outer pinky (the
key below the home pinky) is Esc on tap and Hyper on hold. The right outer
pinky is Backspace on tap and Hyper on hold. QWERTY and Canary leave those
keys transparent, so they inherit the same pair. Hyper is Left Ctrl + Left
Shift + Left Alt + Left GUI in both OS modes. Windows mode swaps Ctrl and
GUI, and a chord that already holds both still sends both.

Thumbs, and why Tab sits where it does:

| Thumb | Key | Tap | Hold |
| --- | --- | --- | --- |
| Left outer | 32 | one-shot Shift | |
| Left middle | 33 | Tab | Nav |
| Left inner | 34 | Space | Lower |
| Right inner | 35 | Enter | Sym |
| Right middle | 36 | Backspace | Adjust |
| Right outer | 37 | Delete | |

Space stays on the inner thumb, which is the resting key. Tab shares the
middle thumb with Nav, between Shift and Space. That thumb stays: holding
`a` opens Nav for the right hand, and the Tab thumb still opens Nav when
the left hand needs the F-keys and zoom keys on that side. One-shot Shift
is the outer left thumb (1 second, quick-release), matching the Leeloo
one-shot shift. Repeat is not on a thumb; it is on Adjust.

QWERTY and Canary leave the thumbs and both pinkies transparent, so they
inherit this row. `&to` keeps Base underneath them.

## QWERTY

```
 q   w*  e   r   t          y*  u   i   o   p*
A/Nv Alt Sft Ctl Cmd       Cmd Ctl Sft Alt  ;
Esc  z   x*  c   v*  b      n   m   ,   .   /* Bsp
        (same thumbs and outer pinkies as Base)
```

`f` is Ctrl and adaptive, `j` is Ctrl and adaptive, `l` is Alt and
adaptive. Those three use their own hold-taps so a tap still runs the
adaptive rule. `g` and `h` are the inner-column Cmd keys.

## Canary

The Leeloo Canary, including the Q/W swap noted in that keymap.

```
 q   l*  y*  p*  b          z   f*  o   u   ;
C/Nv Alt Sft Ctl Cmd       Cmd Ctl Sft Alt  a
Esc  w*  j*  v*  d   k      x*  h   ,   .   /* Bsp
        (same thumbs and outer pinkies as Base)
```

The left home pinky taps Canary's `c` and holds Nav. That is the same
physical key as Colemak `a`. Canary's `a` is the right home pinky and is a
plain letter.

## Lower

Hold the left Space thumb. This layer used to be Num. The left hand is the
Leeloo Lower symbol block, in that order. The right hand is the Leeloo
numpad, with the operators that fit beside it.

Leeloo's far-left column does not have its own column here. Plain Shift is
the left outer pinky. Backtick is the outer left thumb, and Shift plus that
key is tilde. Shift-Tab is the middle left thumb. The inner left thumb is
the key that is holding the layer.

The left home row is the symbols, not a transparent mod row. Leeloo held
Alt, Shift, Cmd, and Ctrl on `[ ( ) ]`. Those holds are not copied, so a
tap is the symbol.

```
 !   <   >   =   ^          *   7   8   9   +
 #   [   (   )   ]          &   4   5   6   -
Sft  @   $   {   }   %      0   1   2   3   /   |
   `   S-Tab  (held)          Space  Backspace  Delete
```

`! @ # $ % ^ & * +` are the shifted number-row symbols (`*` is Shift+8,
`&` is Shift+7, `%` is Shift+5, `+` is Shift+=). `-` is the minus key.
`/` is slash. `|` is pipe. `0` through `9` are the number keys, not the
keypad. `0` is plain: it does not turn into Space after an operator.

The right index column is `* & 0`. `7 8 9` / `4 5 6` / `1 2 3` sit to its
right. `+` and `-` are the outer column, where the markup put them. `/`
is the key that used to be adaptive `0`. `|` is the right outer pinky.
Space is the inner right thumb while Lower is held. Backspace and Delete
stay on the other right thumbs.

`:` is still Shift plus the base semicolon. `.` is still the base dot.
The Leeloo colon was the outer right column; `+` is there instead.

Mouse move, scroll, and clicks used to be the left hand of this layer.
They are not on Lower, and they were not moved. Leeloo keeps the mouse on
Raise. Sym's left side already has its own symbols. The firmware still
compiles pointing, so the Bluetooth report does not change.

Terminal chords follow the digit keys: `8+9` opens a terminal (`Ctrl+Grave`
on Mac, `Win+Grave` in Windows mode), `7+8` next, `9+0` previous, `7+8+9`
new terminal. `7+8` is a subset of `7+8+9`; hit all three inside the 70 ms
window. `0` is the inner bottom key, so previous is that key plus `9`.
Next and previous are Shift+Cmd+[ / ] on Mac and Shift+Ctrl+[ / ] in
Windows mode.

## Nav

Hold the Tab thumb, or hold the left home pinky (`a` on Colemak and
QWERTY, `c` on Canary). The pinky hold reaches the right-hand Nav keys.
The Tab thumb still reaches the F-keys and zoom on the left.

```
 F1  F2  F3  F4  F5       C-PgUp  Home  Up   End  (unlock)
 F6  F7  F8  F9  F10      Cmd-S-Tab Left Down Right Cmd-Tab
Esc F11 F12 Zoom- Zoom+ Zoom0  C-Tab  PgUp  AddCur PgDn Fwd  sticky Cmd
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

## Sym

Hold the Enter thumb. Coding symbols are on the left hand, pairs next to
each other, parens and brackets on the home row. The right home row is
transparent, so Cmd, Ctrl, Shift, and Alt still chord from that hand while
the thumb holds the layer. `<` and `>` are on the top row. Shift plus the
right-hand comma and dot (they fall through on layers that leave them
transparent) is the other way to type them, but on this layer those two
keys are select-part and select-line, so the dedicated `<` `>` keys are
the ones to use.

```
 {   }   <   >   |        Vol+ Mute Mic Play Next
 (   )   [   ]   \        (home row falls through to the mods)
 =   +   -   _   *   /    Vol- SelWd SelPart Line Back Fwd
    (thumbs fall through)        (held)  Backspace  Delete
```

`|` is the pipe. `\` is backslash. `Mic` is Ctrl+Alt+Cmd+M, so swapping
Ctrl and Cmd does not change it. `SelWd` is Alt+Shift+Right (repeat
extends the selection). `SelPart` is Alt+Ctrl+Shift+Right, which becomes
Alt+Win+Shift+Right in Windows mode. `Line` is Cmd+Left then
Cmd+Shift+Right (Ctrl+Left then Ctrl+Shift+Right in Windows mode). `Back`
and `Fwd` are browser back and forward: Cmd+[ and Cmd+] on Mac, Alt+Left
and Alt+Right on Windows. Nav still has the other forward, Cmd+Right /
Alt+Right.

`?` is Shift plus the `/` key (the right-hand Shift falls through on this
layer). `! @ # $ % ^ & *` are keys on Lower. Backtick is the outer left
thumb there, and Shift plus that key is `~`. Adjust still has a backtick
too. Quotes stay the `H+,` combo (`"` unshifted, `'` shifted).
`;` and `:` stay the base semicolon key. `.` stays the base dot.

Where the old Raise keys went:

| Old Raise key | Now |
| --- | --- |
| Mouse move, clicks, scroll | Removed. They were the left hand of Num, and Lower does not keep them |
| Zoom out, zoom in, zoom reset | Nav, left bottom. Brightness stays on Adjust. Repeat stays on Adjust |
| `()` / `<>` macro, arrow snippet, `[]` / `{}` macro | Adjust, in the old non-US `#`, Scroll Lock, and Num Lock spots. Num Lock remains the `L U Y` combo. `` ` `` stays on Adjust |
| Volume, mute, mic mute, play, next, select word / part / line, browser back and Cmd+] forward | Sym, right top and bottom |

The on-Raise key that toggled Raise while the layer was held is not copied.
It could not lock the layer. Adjust's Sym toggle is the lock that works.

## Adjust

Hold the Backspace thumb. This is also where Bluetooth lives; the Leeloo was
wired.

```
Reset ClearBT OutToggle BT0 BT1     Base  QWERTY Canary ()  (unlock)
Boot  NextBT  BT2  BT3  BT4          Lower Nav   Sym   `  Sleep
Esc   PrevBT  Bri+ Bri- Repeat Caps  Ins  PrtSc Arrow []  CapsWord Bsp
   (held)   Tab   Space            Enter  (held)  Delete
```

`Base`, `QWERTY`, and `Canary` are `&to`. They replace the persistent
default layer. `Lower`, `Nav`, and `Sym` are toggles on this layer, which
is the lock that works: hold Adjust, tap the layer, release Adjust, and
that layer stays up. `&to Base` (or QWERTY / Canary) turns the toggled
layers off. The Leeloo's 120 second layer-lock idle timeout is not ported.

`Boot` is the bootloader. Reset-by-combo is not ported, on purpose.
These Bluetooth, boot, and reset keys are duplicated on System. Adjust
keeps them so one thumb is still enough.

## System

Hold Lower and Sym together. Those are the two inner thumbs: Space/Lower on
the left, Enter/Sym on the right. Each is already a layer key, and they
sit under the resting position, one hand each. Nav stays on the middle Tab
thumb and Adjust stays on the Backspace thumb, so those remain one-thumb
layers.

ZMK turns System on only while both of those layers are held
(`conditional_layers` on Lower + Sym). System is not also bound to its own
hold or toggle. Let go of either thumb and System drops. While it is up it
covers Lower and Sym.

```
 Mac  Win  Out  BT0  BT1       Clear  Save  Find
 Boot Rst  BT2  BT3  BT4                            Sleep

          (held Lower)             (held Sym)
```

The outer pinkies are blank on this layer, so the chord does not fire
left Esc/Hyper, right Backspace/Hyper, Lower's Shift and `|`, or Sym's `/`
and browser forward.

| Key | What it sends |
| --- | --- |
| Mac | Mac shortcut mode. Cmd stays Cmd |
| Win | Windows shortcut mode, and the boot default. Ctrl and Cmd swap, including the home-row mods |
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

The keymap is written with Mac keycodes. The inner home-row column is GUI
(Cmd), the index fingers are Ctrl, and the shortcut combos are Cmd chords.
Windows mode is the boot default: `CONFIG_TOTEM_OS_MODE_DEFAULT_WINDOWS`
is set in `config/totem.conf`. A fresh board, and a board whose settings
have been cleared, starts in Windows mode. Windows mode does not replace
those nodes and does not add a second set of hold-taps, so the timing on
`hrm_l` and `hrm_r` applies in both modes. The inner column uses `hrm_in_l`
and `hrm_in_r` with the same swap.

A listener on the central half rewrites the report after sticky key, caps
word, and key repeat have seen the original code, and before USB/BLE HID
sends it. Left Ctrl and Left GUI trade places, and the same for the right
side. Shift and Alt stay put. A chord that already holds both Ctrl and
Cmd still sends both. Hyper is four key events, and the listener trades
the Ctrl and GUI keycodes, so the set stays Ctrl+Alt+Shift+GUI. Mic mute
is one event with both modifier bits, and the bit swap keeps both.

The listener is not a layer, so switching alpha layout with `&to` does not
clear it. The choice is stored in ZMK settings (`totem/os/win`) and comes
back after a reboot. A value already stored there overrides the compile
default. A board that saved Mac on an earlier flash still boots in Mac
mode until that saved value changes.

Press the System-layer Win key once to store Windows mode. That leaves
Bluetooth bonds in place. Flashing ZMK's `settings_reset` firmware for the
XIAO wipes the settings partition, bonds included, and the next boot of
this firmware then uses the Windows default. This repository does not
build a settings_reset image. `&bt BT_CLR` clears the current Bluetooth
profile and does not erase the saved mode.

A straight swap is wrong for a few Leeloo shortcuts, so those are their own
behaviors in `config/os.dtsi` and do not get swapped a second time:

| Key | Mac | Windows |
| --- | --- | --- |
| Redo (F P) | Cmd+Shift+Z | Ctrl+Y |
| Clipboard history (X D) | Alt+Shift+Cmd+Space | Win+V |
| Nav `Fwd` | Cmd+Right | Alt+Right |
| Sym `Back` | Cmd+[ | Alt+Left |
| Sym `Fwd` | Cmd+] | Alt+Right |

Everything else that is Cmd in the keymap becomes Ctrl, and a bare Ctrl
becomes Win. That includes undo, cut, copy, paste, paste-without-formatting,
select-all, save, find, zoom, add-cursor, select-line, the HTML tag's
Cmd+D, and the home-row mods. Terminal chords on Lower follow the same rule:
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

Windows mode is how a fresh board boots. Mac mode is drawn below. Windows
mode swaps Ctrl and GUI, so the index becomes Win and the inner column
becomes Ctrl. Alt and Shift stay put.
That keeps the shortcut modifier on the inner column in both modes: Cmd
on Mac, Ctrl on Windows. The index keeps the other modifier: Ctrl on Mac,
Win on Windows. Leaving the fingers unswapped would put Win on the inner
column and Ctrl on the index, which fights this layout.

| | Left | Right |
| --- | --- | --- |
| Pinky | letter, or Nav on the left | letter |
| Ring | Alt | Alt |
| Middle | Shift | Shift |
| Index | Ctrl | Ctrl |
| Inner | GUI / Cmd | GUI / Cmd |

Colemak letters are `A R S T G` / `M N E I O`. QWERTY uses the same
fingers: `A S D F G` / `H J K L ;`, with `F`, `J`, and `L` also adaptive.
Canary uses `C R S T G` / `M N E I A`. The left pinky hold is Nav on all
three. The right pinky is a plain letter.

The Leeloo did not use this home row. Its left home pinky was hold-Adjust
/ tap the letter, with the global 200 ms tapping term and no prior-idle.
Colemak `O` was a plain letter. QWERTY's semicolon was hold-Adjust; this
port leaves it a letter so the right pinky matches Colemak and Canary,
and Adjust stays on the thumb.

The left pinky uses the same balanced / 280 ms / 150 ms prior-idle tuning
as the other home-row mods, and it only becomes Nav with a key on the
right hand. The layer turns on as that key goes down, so an arrow can
repeat. The inner Cmd keys are stricter (320 ms tapping term, 200 ms
prior-idle, 200 ms quick-tap) because `G` and `M` are rolled, including
cross-hand rolls. If Cmd still fires while typing, raise
`require-prior-idle-ms` on `hrm_in_l` and `hrm_in_r`. If Cmd is hard to
reach, lower it toward 150.

| Property | Value | Effect |
| --- | --- | --- |
| flavor | balanced | A hold is a key pressed and released while the mod is still down |
| tapping-term-ms | 280 | How long a same-hand hold can still become a modifier |
| quick-tap-ms | 175 | A second tap inside this window is a tap, so double letters work |
| require-prior-idle-ms | 150 | Typing faster than this keeps the key a tap |
| hold-trigger-key-positions | the other hand, including its pinky and thumbs | A same-hand roll is a tap |
| hold-trigger-on-release | set | Same-hand mods can still be held together |

There is no global quick-tap. The table is the ring, middle, and index
(`hrm_l`, `hrm_r`). The inner Cmd keys are `hrm_in_l` and `hrm_in_r`.
Thumb layer-taps are separate: balanced, 200 ms tapping term, 120 ms
quick-tap, and no prior-idle requirement, so Lower and Sym do not need a
pause. The left pinky's Nav hold is `a_nav`, not that thumb tuning.

If mods fire while typing, raise `require-prior-idle-ms` (try 200) or
`tapping-term-ms`. If mods feel late, lower `require-prior-idle-ms` toward
100 or `tapping-term-ms` toward 240.

## Esc / Hyper

Both outer pinkies use the `eh` hold-tap. The left is `&eh 0 ESC`. The
right is `&eh 0 BSPC`. Hold-taps always take two parameters, and the `0`
is unused because Hyper takes none. The timing is the same on both keys.

| Property | Value | Effect |
| --- | --- | --- |
| flavor | tap-preferred | A release before the tapping term is always the tap key |
| tapping-term-ms | 200 | Hyper starts once the key has been down this long |
| require-prior-idle-ms | 150 | A pinky struck during typing stays Esc or Backspace |

A fast tap, including a roll from a letter into the pinky, does not become
Hyper. Hyper+key means a short pause, then holding the pinky past 200 ms.
If a tap still turns into Hyper, raise `tapping-term-ms`. If Hyper is hard
to reach, lower `require-prior-idle-ms` or remove that line. The node is
`eh` in `config/totem.keymap`.

Lower replaces the left pinky with Shift and the right pinky with `|`. Sym replaces
them with `/` and browser forward. Nav keeps Esc/Hyper on the left pinky
and puts sticky Cmd on the right. System leaves both blank. QWERTY,
Canary, and Adjust leave them transparent, so the left stays Esc/Hyper
and the right stays Backspace/Hyper.

## Combos

50 ms, alpha layers only (Base, QWERTY, Canary). Positions follow Colemak-DH,
including while QWERTY or Canary is active. The Mac column is the keymap
before the swap. Windows mode is the boot default; see Mac and Windows.

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
| T N | Caps word (the two index keys, which are Ctrl) | same |
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

- F-keys, brightness, and the app/tab switchers moved to Nav. Zoom replaced
  brightness on Nav; brightness stays on Adjust.
- Lower, on the Space thumb, is the Leeloo Lower layer: symbols on the
  left, numpad and operators on the right. The Leeloo mouse stays off this
  layer. Sym, on the Enter thumb, is the other symbol and media layer.
- One-shot Shift, Tab/Nav, Space/Lower on the left; Enter/Sym,
  Backspace/Adjust, Delete on the right. The Leeloo's repeat thumb is on
  Adjust, and play/pause is on Sym.
- The left outer pinky is Esc/Hyper and the right outer pinky is
  Backspace/Hyper, not a full-height outer column.
  Backslash is on Sym. Quote is the `H+,` combo.
- Home-row mods replace the Leeloo's corner modifiers. Ring is Alt, middle
  is Shift, index is Ctrl, and the inner column is Cmd. The left home
  pinky holds Nav. The right home pinky is a plain letter.
- Encoders became keys: volume, brightness, zoom, undo/redo (combos), mic
  mute, play/pause, next track.
- Three alpha layouts stay. Switching them is `&to` on Adjust.
- Bluetooth profile select, clear, next/previous, output toggle, bootloader,
  and sleep are on Adjust. System repeats profile select, clear, output
  toggle, bootloader, reset, and sleep, and adds Save and Find. Adjust's
  copies stay, so one thumb still reaches Bluetooth.
- Shortcuts are written as Mac chords. The firmware boots in Windows mode
  (`CONFIG_TOTEM_OS_MODE_DEFAULT_WINDOWS` in `config/totem.conf`). A
  previously saved mode overrides that default. Press System's Win key
  once, or flash `settings_reset` and then this firmware, to get Windows
  again. System's Mac and Win keys swap Ctrl and Cmd at runtime, including
  home-row mods and the shortcut combos, and they keep the Leeloo
  exceptions (redo is Ctrl+Y on Windows, clipboard history is Win+V,
  browser back/forward are Alt+arrows). The Leeloo's compile-time
  `MAC_HOTKEYS` block and its `CG_SWAP` / `CG_NORM` magic keys are that
  story on the QMK side; see Mac and Windows above. The choice is saved
  across reboots.
- The TVPaint layers from the previous Totem keymap are not in the Leeloo
  map and are not in this one.
- Autocorrect is not ported.

## Adaptive keys

Full write-up: [docs/adaptive-keys-analysis.md](docs/adaptive-keys-analysis.md).

The second key of a bigram is replaced when it follows its trigger within
350 ms. The first key is sent immediately. `sv` sends `sc`. `pf` sends `pt`.
The implemented set is the last enabled QMK rules (2025-07-24): `LJ LK`,
`JL JU`, `SV SC`, `CX CS`, `PF PT`, `FW FS`, `E/ E,`, `WP WR`, `BW BD`,
`UY UE`, and `DX DG`. The Leeloo operator-then-`0` rules are not in this
firmware: Lower's `0` is a plain digit. `FW`, not the older `FP` from the
Leeloo readme.

Ctrl, Alt, and Cmd suppress a substitution. Shift does not.

## Try these first

1. Forget the keyboard and pair it again. Mouse keys changed the HID report.
2. `sv` → `sc`, and `e/` → `e,`. Both start on a home-row mod.
3. `pf` → `pt`, `fw` → `fs`, `lj` → `lk`, `jl` → `ju`.
4. A fast Esc on the left outer pinky and a fast Backspace on the right,
   then a roll from a letter into that pinky. A paused hold of either pinky
   plus a letter is Hyper: Ctrl+Alt+Shift+Cmd in Mac mode and in Windows mode.
5. Tab on the middle left thumb, Nav while it is held. Space/Lower, Enter/Sym,
   Backspace/Adjust. Hold `a` and a right-hand arrow for Nav from the pinky.
6. A same-hand roll (`as` on QWERTY, `ar` on Colemak) stays letters. Hold
   `g` or `m` after a short pause, then a key on the other hand: that is
   Cmd in Mac mode and Ctrl in Windows mode. Hold `a` and tap a right-hand
   key for Nav.
7. Caps word from the two index keys (they are Ctrl), then a shifted bigram.
8. `U` and `Y` together produce `-`. A roll produces `UE`.
9. Hold Adjust, tap Lower, release Adjust. Lower should stay. `&to Base` clears it.
10. Hold Space/Lower and Enter/Sym together. System should come up. A fresh
    board is already in Windows mode: W+F is Ctrl+Z and F+P is Ctrl+Y.
    `Mac` puts those on Cmd+Z and Cmd+Shift+Z. `Win` puts them back, and
    one press is also how a previously saved Mac choice is replaced.
    Power-cycle and the last choice should still be selected.
