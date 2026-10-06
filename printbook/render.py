"""PDF output, embedded-font checks and atomic publication."""
from dataclasses import asdict
import json
import os
from pathlib import Path
import tempfile

from reportlab.lib.units import mm
from reportlab.pdfgen.canvas import Canvas

from .fonts import FONT_FILES
from .layout import Anchor, Rule, Text, section_label


def build_manifest(book):
    sources = []
    for entry in book.catalog.entries:
        spans = []
        for page in book.pages:
            for column in range(1, book.profile.columns + 1):
                rows = [r.line for r in page.rows if r.key == entry.key and r.column == column]
                if rows:
                    spans.append({"page": page.number, "column": column,
                                  "first_line": min(rows), "last_line": max(rows)})
        sources.append({
            "path": entry.key, "title": entry.title, "number": book.numbers[entry.key],
            "sha256": entry.source_hash, "source_lines": len(entry.lines),
            "start": asdict(book.locations[entry.anchor]), "end_page": book.entry_ends[entry.key],
            "spans": spans,
            "sections": [{"title": section_label(s), "line": s.line,
                          **asdict(book.locations[s.key])} for s in entry.subsections],
        })
    return {
        "schema": 1, "title": book.catalog.title, "fingerprint": book.fingerprint,
        "catalog_fingerprint": book.catalog.fingerprint,
        "profile": asdict(book.profile), "pages": len(book.pages),
        "navigation_pages": book.navigation_pages, "source_count": len(sources),
        "source_lines": sum(s["source_lines"] for s in sources),
        "fonts": list(FONT_FILES.values()), "sources": sources,
    }


def draw_running_matter(canvas, page, book, fonts):
    p = book.profile
    width, height = p.size
    left = p.left(page.number)
    right = width - (p.outer if page.number % 2 else p.inner)
    if page.kind == "navigation":
        context = "目录与别名速查"
    else:
        context = " / ".join(book.catalog.category_titles[c] for c in page.categories)
    fonts.draw(canvas, "mouhua XCPC", left, height - 9 * mm, 8, "code")
    fonts.draw(canvas, context, right - fonts.width(context, 8), height - 9 * mm, 8)
    canvas.setStrokeGray(0.55)
    canvas.setLineWidth(0.35)
    canvas.line(left, height - 11.5 * mm, right, height - 11.5 * mm)
    canvas.line(left, 11 * mm, right, 11 * mm)
    fonts.draw(canvas, book.fingerprint, left, 7 * mm, 7, "code", gray=0.3)
    label = f"{page.number} / {len(book.pages)}"
    label_width = fonts.width(label, 9, "code", True)
    fonts.draw(canvas, label, right - label_width, 7 * mm, 9, "code", True)
    back = "返回目录"
    bx = right - label_width - 18 - fonts.width(back, 7)
    fonts.draw(canvas, back, bx, 7 * mm, 7, gray=0.3)
    canvas.linkRect("返回分类目录", "contents", (bx, 6 * mm, bx + fonts.width(back, 7), 10 * mm), thickness=0)
    if page.kind == "body":
        categories = list(dict.fromkeys(e.category for e in book.catalog.entries))
        index = categories.index(page.categories[0])
        x = width - 6 * mm if page.number % 2 else 2 * mm
        y = height - (25 + index * 13) * mm
        canvas.setFillGray(0.2)
        canvas.rect(x, y, 4 * mm, 9 * mm, fill=1, stroke=0)
        fonts.draw(canvas, f"{index + 1:02d}", x + 1.2, y + 9, 7, "code", gray=1)
        canvas.setStrokeGray(0.82)
        for column in range(1, p.columns):
            x = left + column * p.width + (column - 0.5) * p.gutter
            canvas.line(x, p.bottom, x, height - p.top)


def render_pdf(book, fonts, output):
    canvas = Canvas(str(output), pagesize=book.profile.size, pageCompression=1,
                    invariant=1, initialFontName="PBMono", initialFontSize=8)
    canvas.setTitle(book.catalog.title)
    canvas.setAuthor(book.catalog.author)
    canvas.setSubject(f"ICPC/XCPC 算法参考；版本 {book.fingerprint}")
    canvas.setCreator("mouhua printbook / ReportLab")
    canvas.setKeywords("ICPC XCPC 算法模板 " + " ".join(e.title for e in book.catalog.entries))
    canvas.setViewerPreference("PrintScaling", "None")
    canvas.setViewerPreference("Duplex", "DuplexFlipShortEdge" if book.profile.flip == "短边翻转"
                               else "DuplexFlipLongEdge")
    canvas.showOutline()
    for page in book.pages:
        draw_running_matter(canvas, page, book, fonts)
        for item in page.items:
            if isinstance(item, Text):
                fonts.draw(canvas, item.text, item.x, item.y, item.size,
                           item.role, item.bold, item.gray)
                if item.target:
                    w = fonts.width(item.text, item.size, item.role, item.bold)
                    canvas.linkRect(item.text, item.target,
                                    (item.x, item.y - 2, item.x + w, item.y + item.size), thickness=0)
            elif isinstance(item, Rule):
                canvas.setStrokeGray(item.gray)
                canvas.setLineWidth(0.35)
                canvas.line(item.x, item.y, item.x + item.width, item.y)
            elif isinstance(item, Anchor):
                canvas.bookmarkPage(item.key, fit="XYZ", left=item.x, top=item.y + 3, zoom=0)
                canvas.addOutlineEntry(item.title, item.key, level=item.level,
                                       closed=(item.level == 1))
        canvas.showPage()
    canvas.save()


def validate_pdf(path, book):
    from pypdf import PdfReader
    reader = PdfReader(path, strict=True)
    if len(reader.pages) != len(book.pages):
        raise ValueError("生成的 PDF 页数与布局记录不一致。")
    if not reader.outline:
        raise ValueError("生成的 PDF 缺少书签。")
    texts = []
    for page in reader.pages:
        texts.append(page.extract_text() or "")
        for ref in page.get("/Resources", {}).get("/Font", {}).values():
            font = ref.get_object()
            if "/DescendantFonts" in font:
                font = font["/DescendantFonts"][0].get_object()
            descriptor = font.get("/FontDescriptor")
            if not descriptor:
                raise ValueError("PDF 中存在没有嵌入的字体。")
            descriptor = descriptor.get_object()
            if not any(name in descriptor for name in ("/FontFile", "/FontFile2", "/FontFile3")):
                raise ValueError("PDF 中存在没有嵌入的字体。")
    joined = "".join("".join(texts).split())
    if "返回目录" not in joined or any("".join(entry.title.split()) not in joined
                                    for entry in book.catalog.entries):
        raise ValueError("生成的 PDF 缺少可检索的中文标题。")


def publish(book, fonts, output):
    """Validate first; commit the PDF with one same-filesystem atomic replace."""
    output = Path(output).resolve()
    if output.suffix.lower() != ".pdf":
        raise ValueError("输出文件必须使用 .pdf 扩展名。")
    output.parent.mkdir(parents=True, exist_ok=True)
    manifest_path = output.with_suffix(".manifest.json")
    with tempfile.TemporaryDirectory(prefix=".printbook-", dir=output.parent) as tmp:
        tmp = Path(tmp)
        pdf = tmp / "book.pdf"
        manifest = tmp / "book.manifest.json"
        render_pdf(book, fonts, pdf)
        validate_pdf(pdf, book)
        manifest.write_text(json.dumps(build_manifest(book), ensure_ascii=False,
                                       indent=2) + "\n", encoding="utf-8")
        previous_manifest = manifest_path.read_bytes() if manifest_path.exists() else None
        os.replace(manifest, manifest_path)
        try:
            os.replace(pdf, output)
        except BaseException:
            if previous_manifest is None:
                manifest_path.unlink(missing_ok=True)
            else:
                manifest.write_bytes(previous_manifest)
                os.replace(manifest, manifest_path)
            raise
    return manifest_path
