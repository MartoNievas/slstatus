# slstatus

My personal build of slstatus, suckless's status monitor, version 1.1 (see `VERSION` in `config.mk`). It feeds `dwm`'s status bar (the `WM_NAME` of the root window), tuned for the [dwm](https://github.com/MartoNievas/dwm) build in the sibling repo and its status2d/statuscmd patches.

## Patches

None. There's no `patches/` directory here, and `slstatus.c`, `util.c`, and the stock components are unmodified. Everything below is local configuration plus one component I added myself.

## Changes beyond stock

- **`battery_icon`** (`components/battery.c`) is a new function, not part of upstream slstatus. It reads `/sys/class/power_supply/<bat>/capacity` and `.../status` directly and returns a Nerd Font glyph sized to the charge level and charging state (a different icon roughly every 10 to 20 percent, plus separate icons for charging, full, and "not charging"). Two comments in that function, `FIX #1` and `FIX #2`, mark spots where the percentage thresholds were reordered and a redundant branch removed after the icon picked the wrong glyph on a TLP-managed laptop (see commits `7895570` and `676ccef`).
- `config.def.h` branches on `BUILD_DESKTOP` (set by the Makefile, see below) to run two different bars from the same config: the desktop side reads CPU temperature via `sensors k10temp-pci-00c3` for a Ryzen board and drops the battery segment entirely, the laptop side reads `/sys/class/thermal/thermal_zone0/temp` and adds battery percentage plus a Bluetooth status segment.
- Every segment is prefixed with a `\x01`-`\x0c` byte and wrapped in `^b#hex^^c#hex^...^d^` color codes. Those aren't decorative, they're read by the paired dwm build's statuscmd and status2d patches: the leading byte tells dwm which segment was clicked (so it can run something like `discord-status.sh` or `toggle-keyboard-language.sh`), and the `^b^`/`^c^`/`^d^` codes set the background, foreground, and reset colors inline. Point this at a stock dwm and you'll see the raw escape bytes instead of a colored bar.
- `MODULE(name)` expands to `$HOME/dev/suckless-btw/slstatus/scripts/name`. That path assumes this repo is checked out at `~/dev/suckless-btw/slstatus`, since the scripts it names now ship in `scripts/` right here rather than in some separate location you had to set up by hand.

## Scripts

`scripts/` holds every shell script the bar and its click handlers call out to. Three different things live in there:

Segments slstatus itself runs through `run_command` in `config.def.h`:
- `discord-status.sh` checks for a running `vesktop` process with `pgrep`, then pulls the unread count out of the window title with `xdotool` and prints `Off`, `On`, or the count.
- `bluetooth-status.sh` checks whether the `bluetooth` systemd unit and adapter are on, and if a device is connected, prints its name (truncated to 10 characters).

Handlers the paired dwm build calls when you click a bar segment (wired up through dwm's own `statuscmds`, not through anything in this repo's `config.def.h`):
- `calendar-click-handler.sh` and `discord-click-handler.sh` are one-liners that open `gsimplecal` and `vesktop`.
- `toggle-keyboard-language.sh` flips `xkb-switch` between the `us` and `es` layouts.
- `audio-device-selector-handler.sh` lists PipeWire sinks with `wpctl status`, offers them in a `dmenu` picker styled to match the dwm color scheme, and on a choice sets that sink as default and migrates every active stream over to it with `pactl`.

Standalone utilities, not tied to the bar at all, meant to be run by hand from a terminal:
- `check-health.sh` is an Arch-specific health check: failed systemd units, critical journal errors, pending `.pacnew` files, zombie processes, and root partition usage, printed with colored `OK`/`WARN`/`FAIL` labels.
- `shortcuts.sh` prints a cheat sheet of zsh emacs-mode keybindings and waits for Enter.

## Known issues

- `make install` copies `slstatus.1`, which isn't in this repository. `.gitignore` lists `*.1`, so the man page is treated like a build artifact even though upstream slstatus ships it as a real source file. Right now `make install` fails at the `cp -f slstatus.1 ...` step; either drop the `.1` line from `.gitignore` and add the man page back, or remove that step from `install` if you don't want one.
- The hostname check in the `Makefile` (`ifeq ($(CURRENT_HOST),)archlinux-desktop)`) has a stray parenthesis, the same mistake as in the sibling dwm build. GNU Make prints `extraneous text after 'ifeq' directive` and falls through as though the condition matched, so `BUILD_DESKTOP` isn't actually gated on the machine's hostname at the moment.
- `POWER_SUPPLY_STATUS` in `components/battery.c` is hardcoded to `"/sys/class/power_supply/BAT1/status"` instead of taking a `%s` for the battery name like the other three paths in that file do. `battery_state`, `battery_remaining`, and `battery_icon` all accept a battery name argument, but whatever you pass in, the status read always comes from BAT1. Harmless on a single-battery laptop where the battery actually is BAT1, wrong on anything with a BAT0 or a second battery.
- `make dist` copies a file named `README` with no extension; this repo only has `README.md`, so that target fails at the `cp -R` step too.

## Building

Needs the Xlib headers (`X11INC`/`X11LIB` in `config.mk` if yours are somewhere unusual), and on Linux the `snd-mixer-oss` kernel module for volume percentage; FreeBSD needs `sndio` for the same.

```sh
make
```

Copies `config.def.h` to `config.h` on the first run, then compiles `slstatus.c`, everything under `components/`, and `util.c` into a single `slstatus` binary. Edit `config.h` once it exists, not `config.def.h`.

Install system-wide:

```sh
sudo make install
```

Puts the binary in `${PREFIX}/bin` (`/usr/local/bin` by default) and tries to copy the man page into `${MANPREFIX}/share/man/man1` (currently broken, see above). Override `PREFIX`/`DESTDIR` as usual, e.g. `make install PREFIX=/usr`.

`make clean` removes the binary and object files.

To actually feed the dwm bar, run slstatus in a loop that sets the root window name (`slstatus &` from your `.xinitrc`, or through the autostart patch in the sibling dwm build). It doesn't daemonize or restart itself if X or dwm restarts underneath it.

## Credits

slstatus is written and maintained by suckless.org, originally by Aaron Marcher; see `LICENSE` for the full contributor list.
