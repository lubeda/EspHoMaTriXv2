# Migrating from EspHoMaTriXv2 to AWTRIX NG

> [!NOTE]
> This guide is still being written. More details will follow soon.

> [!CAUTION]
> **USE THE SCRIPTS AT YOUR OWN RISK!**
>
> The tools in this guide are provided "as is", without any warranty or support. They are not part of AWTRIX NG.
>
> **`gif2awtrix.py` overwrites icons on your AWTRIX NG without asking.** Every existing icon that has the same name as one of the uploaded files is replaced. Back up the icons on your AWTRIX NG first if you want to keep them.

> [!IMPORTANT]
> **Check the icon licenses before you export or import icons.** Icons from LaMetric, URLs or other sources may be protected by copyright or come with license terms of their own. Make sure you are allowed to download, convert and use each icon before you export it with `icons2gif.py` or upload it with `gif2awtrix.py`.

## Export your icons

EHMTXv2 bakes the icons into the firmware at compile time, so they are not stored on the device as files. To reuse them with [AWTRIX NG](https://github.com/Blueforcer/awtrix-ng), you can export all icons from your ESPHome YAML as GIF files with the tool [`tools/icons2gif.py`](./tools/icons2gif.py).

Each icon is saved as `<id>.gif`, so the icon `id` from your YAML becomes the file name, e.g. `home_assistant.gif`.

### Requirements

- Python 3
- The Python packages `pyyaml`, `pillow` and `requests`:

```bash
pip install pyyaml pillow requests
```

### Usage

```bash
python tools/icons2gif.py <your-config>.yaml -o icons
```

| Parameter | Description |
| --- | --- |
| `<your-config>.yaml` | Your ESPHome YAML file with the `ehmtxv2` configuration, or a file that contains only the icon list |
| `-o`, `--output` | Output directory for the GIF files (default: `icons`) |

Example:

```bash
python tools/icons2gif.py install/ulanzi-easy.yaml -o icons
```

```text
OK    error                    -> icons/error.gif
OK    home_assistant           -> icons/home_assistant.gif
OK    temperature              -> icons/temperature.gif
...
28 of 28 icons saved to icons
```

### What is supported

The tool finds every `icons:` list in your YAML and supports all icon sources of EHMTXv2:

| Source | Handling |
| --- | --- |
| `lameid` | Downloaded from LaMetric |
| `url` | Downloaded from the given URL |
| `file` | Loaded from disk, relative to the YAML file |
| `str565` | Converted from the RGB565 array |

- Icons that are already GIFs are saved unchanged.
- Other formats (e.g. PNG) are converted to GIF. Animations keep their frames and timing; if `frame_duration` is set, it is used for all frames.
- ESPHome tags like `!secret` or `!include` don't stop the tool from reading the file. Included files are not followed, so if your icons are in a separate file, run the tool on that file directly.
- If an icon can't be loaded, an error is printed and the remaining icons are still exported. The tool exits with code `1` if any icon failed.

## Upload your icons to AWTRIX NG

After exporting, you can upload all GIF files from a directory to your AWTRIX NG device with the tool [`tools/gif2awtrix.py`](./tools/gif2awtrix.py). It uses the AWTRIX NG HTTP API (`POST /api/v1/files?dir=/ICONS`).

The file name becomes the icon ID on AWTRIX NG, so `home_assistant.gif` can be used as `"icon": "home_assistant"`. Icon IDs are case-sensitive.

> [!WARNING]
> Existing icons on your AWTRIX NG with the same name are **overwritten without confirmation**. Use at your own risk!

### Requirements

- Python 3
- The Python package `requests`:

```bash
pip install requests
```

### Usage

```bash
python tools/gif2awtrix.py <awtrix-ip> -d icons
```

| Parameter | Description |
| --- | --- |
| `<awtrix-ip>` | IP address or hostname of your AWTRIX NG device |
| `-d`, `--dir` | Directory with the GIF files (default: `icons`) |
| `-u`, `--user` | Web login user, only needed if you enabled authentication on AWTRIX NG |
| `-p`, `--password` | Web login password |

Example:

```bash
python tools/icons2gif.py install/ulanzi-easy.yaml -o icons
python tools/gif2awtrix.py 192.168.1.50 -d icons
```

```text
OK    error.gif
OK    home_assistant.gif
...
28 of 28 icons uploaded to http://192.168.1.50
```

- If an upload fails, an error is printed and the remaining icons are still uploaded. The tool exits with code `1` if any upload failed.
- Keep an eye on the storage: on boards with 4 MB flash, AWTRIX NG has about 512 KB for icons, melodies, palettes and scripts together.
