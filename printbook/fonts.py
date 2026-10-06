"""Bundled, embedded fonts and exact glyph measurements for printed code."""
from functools import lru_cache
from pathlib import Path

from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont


FONT_FILES = {
    "PBMono": "Inconsolata-Regular.ttf",
    "PBMonoBold": "Inconsolata-Bold.ttf",
    "PBCJK": "DroidSansFallback.ttf",
    "PBSans": "DejaVuSans.ttf",
}


class FontBook:
    def __init__(self, directory=None):
        self.directory = Path(directory or Path(__file__).resolve().parents[1]
                              / "assets" / "print-fonts")
        self.widths = {}
        for name, filename in FONT_FILES.items():
            path = self.directory / filename
            if not path.is_file():
                raise ValueError(f"缺少随仓库分发的字体：{path}")
            pdfmetrics.registerFont(TTFont(name, str(path)))
            self.widths[name] = pdfmetrics.getFont(name).face.charWidths

    @lru_cache(maxsize=32768)
    def font_for(self, char, role="body", bold=False):
        mono = "PBMonoBold" if bold else "PBMono"
        choices = (mono, "PBCJK", "PBSans") if role == "code" else ("PBSans", "PBCJK", mono)
        for name in choices:
            if ord(char) in self.widths[name]:
                return name
        raise ValueError(f"字体缺少字形 {char!r} (U+{ord(char):04X})；请补充字体后再生成。")

    def runs(self, text, role="body", bold=False):
        name, part = None, []
        for char in text:
            selected = self.font_for(char, role, bold)
            if selected != name and part:
                yield name, "".join(part)
                part = []
            name = selected
            part.append(char)
        if part:
            yield name, "".join(part)

    def width(self, text, size, role="body", bold=False):
        return sum(self.widths[self.font_for(c, role, bold)][ord(c)]
                   for c in text) * size / 1000

    def draw(self, canvas, text, x, y, size, role="body", bold=False, gray=0):
        obj = canvas.beginText(x, y)
        obj.setFillGray(gray)
        for name, part in self.runs(text, role, bold):
            obj.setFont(name, size)
            obj.textOut(part)
        canvas.drawText(obj)

    def wrap(self, text, width, size, role="body", bold=False):
        """Wrap without dropping or replacing any character; no ellipses."""
        if width <= 0:
            raise ValueError("栏宽不足，无法排版。")
        if not text:
            return [""]
        parts, start = [], 0
        while start < len(text):
            used, stop, preferred = 0.0, start, start
            while stop < len(text):
                char = text[stop]
                advance = self.width(char, size, role, bold)
                if used + advance > width + 1e-7:
                    break
                used += advance
                stop += 1
                if char.isspace() or char in ",;)]}":
                    preferred = stop
            if stop == start:
                raise ValueError(f"字号过大或栏宽不足，连一个字符都无法容纳：{text[start]!r}")
            if stop < len(text) and preferred > start + (stop - start) * 0.55:
                stop = preferred
            parts.append(text[start:stop])
            start = stop
        return parts
