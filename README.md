# hypractive

Adds a minimally intrusive active window indicator,
in the form of a dot in the top right corner of the active window.

Example config:

```
plugin {
    hypractive {
        dot_size = 16
        dot_color = rgba(ffffffaa)

        # Adjust offset for dot position
        dot_offset_x = 8
        dot_offset_y = -3

        # Override rounding
        dot_rounding = 0

        # Whether to draw decoration under or over window
        decorate_under = true

        # Wether to decorate floating and grouped windows
        decorate_floating = true
        decorate_grouped = false
    }
}
```

## Install with `hyprpm`

To install this plugin from the command line run:
```bash
hyprpm update
```
Then add this repository:
```bash
hyprpm add https://github.com/sh224/hypractive
```
then enable the plugin with
```bash
hyprpm enable hypractive
```

# Contributing

Feel free to open issues and MRs with fixes.
