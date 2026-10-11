#!/usr/bin/env python3
"""Export all EHMTXv2 icons from an ESPHome YAML file as GIF files.

Every icon is saved as <id>.gif, e.g. for use with AWTRIX NG.
Supported icon sources: lameid, url, file and str565.

Usage:
    python icons2gif.py ulanzi-easy.yaml [-o OUTPUT_DIR]

Requires: pip install pyyaml pillow requests
"""

import argparse
import io
import json
import os
import sys
from urllib.parse import urlparse

import requests
import yaml
from PIL import Image, ImageSequence

LAMETRIC_URL = "https://developer.lametric.com/content/apps/icon_thumbs/"
HEADERS = {"Cache-Control": "no-cache, no-store, must-revalidate", "Pragma": "no-cache"}


class EsphomeLoader(yaml.SafeLoader):
    """SafeLoader that tolerates ESPHome tags like !secret, !include, !lambda."""


def _ignore_tag(loader, tag_suffix, node):
    if isinstance(node, yaml.ScalarNode):
        return loader.construct_scalar(node)
    if isinstance(node, yaml.SequenceNode):
        return loader.construct_sequence(node)
    return loader.construct_mapping(node)


EsphomeLoader.add_multi_constructor("!", _ignore_tag)


def is_icon(entry):
    return isinstance(entry, dict) and "id" in entry


def find_icons(node):
    """Collect icon entries from every 'icons:' list in the config."""
    icons = []
    if isinstance(node, dict):
        for key, value in node.items():
            if key == "icons" and isinstance(value, list):
                icons.extend(i for i in value if is_icon(i))
            else:
                icons.extend(find_icons(value))
    elif isinstance(node, list):
        for item in node:
            icons.extend(find_icons(item))
    return icons


def rgb565_888(v565):
    b = (v565 & 0x001F) << 3
    g = (v565 & 0x07E0) >> 3
    r = (v565 & 0xF800) >> 8
    return (r, g, b)


def download(url):
    r = requests.get(url, headers=HEADERS, timeout=10.0)
    r.raise_for_status()
    return r.content


def load_icon(icon, base_dir):
    """Return raw image bytes or a PIL image for an icon entry."""
    if "lameid" in icon:
        return download(LAMETRIC_URL + str(icon["lameid"]))
    if "url" in icon:
        return download(icon["url"])
    if "file" in icon:
        with open(os.path.join(base_dir, icon["file"]), "rb") as f:
            return f.read()
    if "str565" in icon:
        data = list(json.loads(icon["str565"]))
        size = {64: (8, 8), 256: (32, 8)}.get(len(data))
        if size is None:
            raise ValueError(f"str565 must contain 64 or 256 values, got {len(data)}")
        image = Image.new("RGB", size)
        image.putdata([rgb565_888(v) for v in data])
        return image
    raise ValueError("no supported source (lameid, url, file, str565)")


def save_gif(source, path, frame_duration=None):
    if isinstance(source, bytes):
        image = Image.open(io.BytesIO(source))
        if image.format == "GIF" and not frame_duration:
            # Already a GIF: keep the original file untouched
            with open(path, "wb") as f:
                f.write(source)
            return
    else:
        image = source

    frames = []
    durations = []
    for frame in ImageSequence.Iterator(image):
        frames.append(frame.convert("RGBA"))
        durations.append(int(frame_duration or frame.info.get("duration") or 100))

    if len(frames) == 1:
        frames[0].save(path, format="GIF")
    else:
        frames[0].save(
            path,
            format="GIF",
            save_all=True,
            append_images=frames[1:],
            duration=durations,
            loop=0,
            disposal=2,
        )


def main():
    parser = argparse.ArgumentParser(description="Export EHMTXv2 icons from a YAML file as GIFs.")
    parser.add_argument("yaml_file", help="ESPHome YAML file containing the ehmtxv2 icons")
    parser.add_argument("-o", "--output", default="icons", help="output directory (default: icons)")
    args = parser.parse_args()

    with open(args.yaml_file, encoding="utf-8") as f:
        config = yaml.load(f, Loader=EsphomeLoader)

    # A file that only contains the icon list (without the ehmtxv2 config around it)
    if isinstance(config, list) and all(is_icon(i) for i in config):
        icons = config
    else:
        icons = find_icons(config)
    if not icons:
        sys.exit(f"No icons found in {args.yaml_file}")

    os.makedirs(args.output, exist_ok=True)
    base_dir = os.path.dirname(os.path.abspath(args.yaml_file))

    failed = 0
    for icon in icons:
        icon_id = str(icon["id"])
        path = os.path.join(args.output, icon_id + ".gif")
        try:
            save_gif(load_icon(icon, base_dir), path, icon.get("frame_duration"))
            print(f"OK    {icon_id:24} -> {path}")
        except Exception as e:
            failed += 1
            print(f"ERROR {icon_id:24} {e}", file=sys.stderr)

    print(f"\n{len(icons) - failed} of {len(icons)} icons saved to {args.output}")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
