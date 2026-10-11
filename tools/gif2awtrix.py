#!/usr/bin/env python3
"""Upload all GIF icons from a directory to an AWTRIX NG device.

The file name becomes the icon ID on AWTRIX NG, e.g. home_assistant.gif
can be used as "icon": "home_assistant".

Usage:
    python gif2awtrix.py 192.168.1.50 [-d ICON_DIR] [-u USER -p PASSWORD]

Requires: pip install requests
"""

import argparse
import glob
import os
import sys

import requests


def main():
    parser = argparse.ArgumentParser(description="Upload GIF icons to an AWTRIX NG device.")
    parser.add_argument("ip", help="IP address or hostname of the AWTRIX NG device")
    parser.add_argument("-d", "--dir", default="icons", help="directory with the GIF files (default: icons)")
    parser.add_argument("-u", "--user", help="web login user, if authentication is enabled")
    parser.add_argument("-p", "--password", help="web login password, if authentication is enabled")
    args = parser.parse_args()

    files = sorted(glob.glob(os.path.join(args.dir, "*.gif")))
    if not files:
        sys.exit(f"No GIF files found in {args.dir}")

    host = args.ip if args.ip.startswith("http") else "http://" + args.ip
    url = host.rstrip("/") + "/api/v1/files"
    auth = (args.user, args.password or "") if args.user else None

    failed = 0
    for path in files:
        name = os.path.basename(path)
        try:
            with open(path, "rb") as f:
                r = requests.post(
                    url,
                    params={"dir": "/ICONS"},
                    files={"file": (name, f, "image/gif")},
                    auth=auth,
                    timeout=30.0,
                )
            if r.status_code == 401:
                sys.exit("ERROR authentication failed, use --user and --password")
            r.raise_for_status()
            print(f"OK    {name}")
        except requests.RequestException as e:
            failed += 1
            print(f"ERROR {name:28} {e}", file=sys.stderr)

    print(f"\n{len(files) - failed} of {len(files)} icons uploaded to {host}")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
