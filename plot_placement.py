#!/usr/bin/env python3
"""Render Phase 1's initial placement and baseline wirelength as SVG."""

import csv
import html
import sys
from pathlib import Path


WIDTH = 1200
HEIGHT = 900
MARGIN = 90


def read_placement(filename):
    points = {"cell": ([], []), "pad": ([], [])}
    with filename.open(newline="") as source:
        for row in csv.DictReader(source):
            kind = row["type"]
            points[kind][0].append(float(row["x"]))
            points[kind][1].append(float(row["y"]))
    return points


def write_placement_svg(filename, points, title):
    all_x = points["cell"][0] + points["pad"][0]
    all_y = points["cell"][1] + points["pad"][1]
    maximum_x = max(all_x, default=1.0)
    maximum_y = max(all_y, default=1.0)
    x_span = max(maximum_x + 1.0, 1.0)
    y_span = max(maximum_y + 1.0, 1.0)
    plot_width = WIDTH - 2 * MARGIN
    plot_height = HEIGHT - 2 * MARGIN
    scale = min(plot_width / x_span, plot_height / y_span)
    drawn_width = x_span * scale
    drawn_height = y_span * scale
    left = (WIDTH - drawn_width) / 2.0
    bottom = (HEIGHT + drawn_height) / 2.0

    def position(x, y):
        return left + x * scale, bottom - y * scale

    with filename.open("w", encoding="utf-8") as output:
        output.write(
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" '
            f'height="{HEIGHT}" viewBox="0 0 {WIDTH} {HEIGHT}">\n'
            '<rect width="100%" height="100%" fill="white"/>\n'
            f'<text x="{WIDTH / 2}" y="42" text-anchor="middle" '
            f'font-family="sans-serif" font-size="24">{html.escape(title)}</text>\n'
            f'<rect x="{left:.2f}" y="{bottom - drawn_height:.2f}" '
            f'width="{drawn_width:.2f}" height="{drawn_height:.2f}" '
            'fill="none" stroke="#888" stroke-width="1"/>\n'
        )

        for kind, color in (("cell", "#2878b5"), ("pad", "#d62728")):
            xs, ys = points[kind]
            for x, y in zip(xs, ys):
                px, py = position(x, y)
                if kind == "cell":
                    output.write(
                        f'<circle cx="{px:.2f}" cy="{py:.2f}" r="2.2" '
                        f'fill="{color}" fill-opacity="0.45"/>\n'
                    )
                else:
                    output.write(
                        f'<path d="M {px - 4:.2f} {py - 4:.2f} '
                        f'L {px + 4:.2f} {py + 4:.2f} M {px - 4:.2f} '
                        f'{py + 4:.2f} L {px + 4:.2f} {py - 4:.2f}" '
                        f'stroke="{color}" stroke-width="1.5"/>\n'
                    )

        output.write(
            f'<text x="{WIDTH / 2}" y="{HEIGHT - 18}" text-anchor="middle" '
            'font-family="sans-serif" font-size="16">x</text>\n'
            f'<text x="24" y="{HEIGHT / 2}" text-anchor="middle" '
            'font-family="sans-serif" font-size="16" '
            f'transform="rotate(-90 24 {HEIGHT / 2})">y</text>\n'
            f'<text x="{WIDTH - 210}" y="{HEIGHT - 42}" '
            'font-family="sans-serif" font-size="14" fill="#2878b5">'
            '● movable cells</text>\n'
            f'<text x="{WIDTH - 210}" y="{HEIGHT - 20}" '
            'font-family="sans-serif" font-size="14" fill="#d62728">'
            '× I/O pads</text>\n'
            '</svg>\n'
        )


def write_wirelength_svg(filename, csv_filename):
    with csv_filename.open(newline="") as source:
        rows = list(csv.DictReader(source))
    if not rows:
        raise ValueError(f"No Phase 1 wirelength data in {csv_filename}")
    wirelengths = [float(row["wirelength"]) for row in rows]
    max_wirelength = max(wirelengths, default=1.0)
    max_wirelength = max(max_wirelength, 1e-12)
    plot_left = MARGIN
    plot_right = WIDTH - MARGIN
    plot_top = 100
    plot_bottom = HEIGHT - MARGIN

    def position(stage_index, wirelength):
        x = plot_left + stage_index / max(len(rows) - 1, 1) * (
            plot_right - plot_left
        )
        y = plot_bottom - wirelength / max_wirelength * (
            plot_bottom - plot_top
        )
        return x, y

    coordinates = [
        position(index, value) for index, value in enumerate(wirelengths)
    ]
    polyline = " ".join(f"{x:.2f},{y:.2f}" for x, y in coordinates)
    with filename.open("w", encoding="utf-8") as output:
        output.write(
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" '
            f'height="{HEIGHT}" viewBox="0 0 {WIDTH} {HEIGHT}">\n'
            '<rect width="100%" height="100%" fill="white"/>\n'
            f'<text x="{WIDTH / 2}" y="42" text-anchor="middle" '
            'font-family="sans-serif" font-size="24">'
            'Phase 1 baseline wirelength</text>\n'
            f'<path d="M {plot_left} {plot_top} V {plot_bottom} '
            f'H {plot_right}" fill="none" stroke="#444"/>\n'
            f'<polyline points="{polyline}" fill="none" stroke="#2878b5" '
            'stroke-width="2"/>\n'
        )
        for (x, y), row in zip(coordinates, rows):
            output.write(
                f'<circle cx="{x:.2f}" cy="{y:.2f}" r="5" fill="#2878b5"/>\n'
                f'<text x="{x + 8:.2f}" y="{y - 8:.2f}" '
                'font-family="sans-serif" font-size="13">'
                f'{html.escape(row["stage"])}: '
                f'{float(row["wirelength"]):.4g}</text>\n'
            )
        output.write(
            f'<text x="{WIDTH / 2}" y="{HEIGHT - 18}" text-anchor="middle" '
            'font-family="sans-serif" font-size="16">Phase 1 stage</text>\n'
            f'<text x="24" y="{HEIGHT / 2}" text-anchor="middle" '
            'font-family="sans-serif" font-size="16" '
            f'transform="rotate(-90 24 {HEIGHT / 2})">'
            'sqrt(weighted squared wirelength)</text>\n'
            '</svg>\n'
        )


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {Path(sys.argv[0]).name} <benchmark-basename>",
              file=sys.stderr)
        return 2

    prefix = Path(sys.argv[1] + "_phase1")
    try:
        initial = prefix.with_name(prefix.name + "_initial.csv")
        wirelength = prefix.with_name(prefix.name + "_wirelength.csv")
        write_placement_svg(
            prefix.with_name(prefix.name + "_initial.svg"),
            read_placement(initial),
            "Phase 1 initial center placement",
        )
        write_wirelength_svg(
            prefix.with_name(prefix.name + "_wirelength.svg"), wirelength
        )
    except (OSError, KeyError, ValueError) as error:
        print(f"Could not create placement plots: {error}", file=sys.stderr)
        return 1

    print(f"SVG plots written with prefix {prefix}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
