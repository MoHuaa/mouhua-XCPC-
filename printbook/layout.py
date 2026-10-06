"""Deterministic pagination. Layout records also serve as the coverage audit."""
from dataclasses import asdict, dataclass, field, replace
import hashlib
import json
from pathlib import Path

from reportlab.lib.pagesizes import A4, landscape
from reportlab.lib.units import mm


@dataclass(frozen=True)
class Profile:
    name: str
    label: str
    size: tuple
    columns: int
    font_size: float
    leading: float
    inner: float = 16 * mm
    outer: float = 11 * mm
    gutter: float = 8 * mm
    top: float = 16 * mm
    bottom: float = 15 * mm

    @property
    def width(self):
        return (self.size[0] - self.inner - self.outer
                - self.gutter * (self.columns - 1)) / self.columns

    def left(self, page):
        return self.inner if page % 2 else self.outer

    @property
    def flip(self):
        return "短边翻转" if self.size[0] > self.size[1] else "长边翻转"


PROFILES = {
    "print": Profile("print", "A4 横向双栏", landscape(A4), 2, 8.6, 10.3),
    "compact": Profile("compact", "A4 横向三栏", landscape(A4), 3, 7.5, 9.2, gutter=6 * mm),
    "readable": Profile("readable", "A4 纵向单栏", A4, 1, 10.0, 12.5),
}


def section_label(section):
    return f"[{section.key.rsplit('-s', 1)[-1]}] {section.title}"


def get_profile(name="print", font_size=None):
    profile = PROFILES[name]
    if font_size is not None:
        if not 5 <= font_size <= 18:
            raise ValueError("--font-size 必须在 5 到 18 pt 之间。")
        profile = replace(profile, font_size=font_size,
                          leading=font_size * profile.leading / profile.font_size)
    return profile


@dataclass
class Text:
    text: str
    x: float
    y: float
    size: float
    role: str = "body"
    bold: bool = False
    gray: float = 0
    target: str = ""


@dataclass
class Rule:
    x: float
    y: float
    width: float
    gray: float = 0.75


@dataclass
class Anchor:
    key: str
    title: str
    level: int
    x: float
    y: float


@dataclass
class CodeRow:
    key: str
    line: int
    column: int
    text: str
    continuation: bool
    x: float
    y: float
    width: float


@dataclass
class Page:
    number: int
    kind: str
    items: list = field(default_factory=list)
    rows: list = field(default_factory=list)
    categories: list = field(default_factory=list)
    topics: list = field(default_factory=list)


@dataclass
class Location:
    page: int
    column: int
    y: float


@dataclass
class Book:
    catalog: object
    profile: Profile
    pages: list
    locations: dict
    entry_ends: dict
    numbers: dict
    fingerprint: str
    navigation_pages: int


class BodyLayout:
    def __init__(self, catalog, profile, fonts, first_page):
        self.catalog, self.profile, self.fonts = catalog, profile, fonts
        self.pages, self.locations, self.ends, self.numbers = [], {}, {}, {}
        self.first_page = first_page
        self.page = None
        self.column = 0
        self.y = 0
        self.category = ""

    def new_page(self):
        self.page = Page(self.first_page + len(self.pages), "body")
        self.pages.append(self.page)
        self.column = 0
        self.y = self.profile.size[1] - self.profile.top
        self.page.categories.append(self.category)

    def new_column(self):
        self.column += 1
        if self.column >= self.profile.columns:
            self.new_page()
        else:
            self.y = self.profile.size[1] - self.profile.top

    @property
    def x(self):
        return self.profile.left(self.page.number) + self.column * (self.profile.width + self.profile.gutter)

    def add_text(self, text, y, size, **kwargs):
        self.page.items.append(Text(text, self.x, y, size, **kwargs))

    def anchor(self, key, title, level):
        self.locations[key] = Location(self.page.number, self.column + 1, self.y)
        self.page.items.append(Anchor(key, title, level, self.x, self.y))

    def heading_lines(self, entry, number):
        return self.fonts.wrap(f"{number}  {entry.title}", self.profile.width, 10.5)

    def entry_heading(self, entry, number, continuation=False, section=None):
        if continuation:
            title = section_label(section) if section else entry.title
            text = f"{number}  {title} / 续"
            lines = self.fonts.wrap(text, self.profile.width, 8.0)
            for line in lines:
                self.add_text(line, self.y - 8, 8, gray=0.18)
                self.y -= 10
            self.page.items.append(Rule(self.x, self.y - 1.5, self.profile.width, 0.7))
            self.y -= 7
        else:
            self.anchor(entry.anchor, f"{number} {entry.title}", 1)
            for line in self.heading_lines(entry, number):
                self.add_text(line, self.y - 10.5, 10.5)
                self.y -= 14
            for line in self.fonts.wrap(entry.key, self.profile.width, 6.8, "code"):
                self.add_text(line, self.y - 6.8, 6.8, role="code", gray=0.28)
                self.y -= 9
            if entry.summary:
                for line in self.fonts.wrap(entry.summary, self.profile.width, 7.5):
                    self.add_text(line, self.y - 7.5, 7.5, gray=0.2)
                    self.y -= 10
            self.page.items.append(Rule(self.x, self.y - 2, self.profile.width, 0.5))
            self.y -= 9
        if entry.title not in self.page.topics:
            self.page.topics.append(entry.title)

    def heading_height(self, entry, number):
        return (len(self.heading_lines(entry, number)) * 14
                + len(self.fonts.wrap(entry.key, self.profile.width, 6.8, "code")) * 9
                + (len(self.fonts.wrap(entry.summary, self.profile.width, 7.5)) * 10
                   if entry.summary else 0) + 9)

    def run(self):
        chapter, item = 0, 0
        p = self.profile
        for entry in self.catalog.entries:
            if entry.category != self.category:
                self.category = entry.category
                chapter += 1
                item = 0
                self.new_page()
                title = f"{chapter:02d}  {self.catalog.category_titles[self.category]}"
                self.anchor("category-" + self.category, title, 0)
                self.add_text(title, self.y - 15, 15)
                self.page.items.append(Rule(self.x, self.y - 22, p.width, 0.25))
                self.y -= 24
            item += 1
            number = self.numbers[entry.key] = f"{chapter:02d}.{item:02d}"
            first_lines = min(4, max(1, len(entry.lines)))
            if self.y - self.heading_height(entry, number) - first_lines * p.leading < p.bottom:
                self.new_column()
            self.entry_heading(entry, number)
            gutter = max(19.0, len(str(max(1, len(entry.lines)))) * p.font_size * 0.5 + 6)
            width = p.width - gutter
            section_at = {s.line: s for s in entry.subsections}
            prepared = []
            for lineno, raw in enumerate(entry.lines, 1):
                line = raw.expandtabs(4)
                # Visual continuation indentation is separate from source text.
                indent = min(len(line) - len(line.lstrip(" ")) + 2, 8) * p.font_size * 0.5
                fragments = self.fonts.wrap(line, width - indent, p.font_size, "code")
                # A normal-width first row avoids needless wraps caused by continuation space.
                if self.fonts.width(line, p.font_size, "code") <= width:
                    fragments = [line]
                assert "".join(fragments) == line
                for j, fragment in enumerate(fragments):
                    prepared.append((lineno, fragment, j, indent, line.lstrip().startswith("//")))
            active_section = None
            for row_index, (lineno, fragment, j, indent, comment) in enumerate(prepared):
                if j == 0 and lineno in section_at:
                    active_section = section_at[lineno]
                remaining = len(prepared) - row_index
                # Keep the final three visual rows together: no lone closing brace.
                need = min(remaining, 3) if remaining <= 3 else 1
                if j == 0 and lineno in section_at:
                    need = max(need, min(5, remaining))
                if self.y - need * p.leading < p.bottom:
                    self.new_column()
                    self.entry_heading(entry, number, True, active_section)
                if j == 0 and lineno in section_at:
                    sec = section_at[lineno]
                    self.anchor(sec.key, section_label(sec), 2)
                x = self.x + gutter + (indent if j else 0)
                baseline = self.y - p.font_size
                number_text = ">" if j else str(lineno)
                num_width = self.fonts.width(number_text, 6.2, "code")
                self.page.items.append(Text(number_text, self.x + gutter - 5 - num_width,
                                            baseline, 6.2, role="code", gray=0.35))
                self.page.items.append(Text(fragment, x, baseline, p.font_size,
                                            role="code", bold=lineno in section_at,
                                            gray=0.22 if comment else 0))
                self.page.rows.append(CodeRow(entry.key, lineno, self.column + 1,
                                              fragment, j > 0, x, baseline, width))
                self.y -= p.leading
            if not entry.lines:
                self.add_text("（空文件）", self.y - 8, 8, gray=0.35)
                self.y -= 12
            self.ends[entry.key] = self.page.number
            self.y -= 13
        return self


class NavigationLayout:
    def __init__(self, catalog, profile, fonts, locations, ends, numbers, fingerprint, total):
        self.catalog, self.profile, self.fonts = catalog, profile, fonts
        self.locations, self.ends, self.numbers = locations, ends, numbers
        self.fingerprint, self.total = fingerprint, total
        self.pages, self.page = [], None
        self.column, self.y = 0, 0
        self.columns = 2 if profile.size[0] > profile.size[1] else 1
        self.width = ((profile.size[0] - profile.inner - profile.outer
                       - profile.gutter * (self.columns - 1)) / self.columns)
        self.first_top = 0

    @property
    def x(self):
        return self.profile.left(self.page.number) + self.column * (self.width + self.profile.gutter)

    def new_page(self):
        self.page = Page(len(self.pages) + 1, "navigation")
        self.pages.append(self.page)
        self.column = 0
        self.y = self.profile.size[1] - self.profile.top
        if len(self.pages) == 1:
            x, y = self.x, self.y
            self.page.items.append(Text("XCPC / TEAM REFERENCE", x, y - 8, 8, gray=0.3))
            self.page.items.append(Text(self.catalog.title, x, y - 34, 22))
            line_count = sum(len(e.lines) for e in self.catalog.entries)
            summary = (f"{self.catalog.author}  |  {len(self.catalog.entries)} 份源码 / {line_count:,} 行"
                       f"  |  {self.profile.label} / {self.profile.font_size:g} pt")
            self.page.items.append(Text(summary, x, y - 52, 8.5))
            self.page.items.append(Text(
                f"打印：100% 实际大小，双面{self.profile.flip}。页脚编号与 PDF 页码一致。",
                x, y - 68, 8, gray=0.18))
            self.page.items.append(Text(
                f"查找：分类目录 / 别名速查 / 几何分节书签。版本 {self.fingerprint}，共 {self.total} 页。",
                x, y - 81, 8, gray=0.18))
            full_width = self.profile.size[0] - self.profile.inner - self.profile.outer
            self.page.items.append(Rule(x, y - 92, full_width, 0.25))
            self.y -= 105
            self.first_top = self.y

    def space(self, height):
        if self.y - height < self.profile.bottom:
            self.column += 1
            if self.column >= self.columns:
                self.new_page()
            else:
                self.y = self.first_top if len(self.pages) == 1 else self.profile.size[1] - self.profile.top

    def text(self, text, height=16, size=10, gray=0, target=""):
        self.space(height)
        self.page.items.append(Text(text, self.x, self.y - size, size, gray=gray, target=target))
        self.y -= height

    def row(self, title, target, pages, indent=0, size=8.5):
        # Fixed-width page field makes pagination independent of the page numbers.
        lines = self.fonts.wrap(title, self.width - 54 - indent, size)
        self.space(len(lines) * 12.5 + 2)
        for line in lines:
            self.page.items.append(Text(line, self.x + indent, self.y - size, size, target=target))
            self.y -= 12.5
        page_width = self.fonts.width(pages, size, "code")
        self.page.items.append(Text(pages, self.x + self.width - page_width,
                                    self.y + 12.5 - size, size, role="code", target=target))
        self.y -= 2

    def page_range(self, entry):
        start = self.locations[entry.anchor].page
        end = self.ends[entry.key]
        return str(start) if start == end else f"{start}-{end}"

    def run(self):
        self.new_page()
        self.page.items.append(Anchor("contents", "分类目录", 0, self.x, self.y))
        self.text("分类目录", 24, 13)
        category = None
        for entry in self.catalog.entries:
            if entry.category != category:
                category = entry.category
                title = self.catalog.category_titles[category]
                number = self.numbers[entry.key].split(".")[0]
                self.space(45)
                self.text(f"{number}  {title}", 21, 10.5, target="category-" + category)
            self.row(f"{self.numbers[entry.key]}  {entry.title}", entry.anchor, self.page_range(entry))
            for sec in entry.subsections:
                self.row(section_label(sec), sec.key, str(self.locations[sec.key].page), 12, 7.8)

        self.space(55)
        self.y -= 8
        self.page.items.append(Anchor("aliases", "别名速查", 0, self.x, self.y))
        self.text("别名速查", 24, 13)
        aliases = []
        seen = set()
        for entry in self.catalog.entries:
            stem = entry.key.rsplit("/", 1)[-1].rsplit(".", 1)[0]
            for term in (*entry.aliases, stem):
                key = (term.casefold(), entry.key)
                if key not in seen and term.casefold() != entry.title.casefold():
                    seen.add(key)
                    aliases.append((term, entry))
        aliases.sort(key=lambda a: (not a[0][0].isascii(), a[0].casefold(), self.numbers[a[1].key]))
        for term, entry in aliases:
            self.row(f"{term}  [{self.numbers[entry.key]}]", entry.anchor,
                     str(self.locations[entry.anchor].page), size=8)
        return self


def build_layout(catalog, profile, fonts):
    style = json.dumps(asdict(profile), sort_keys=True, ensure_ascii=False)
    generator = hashlib.sha256()
    for source in sorted(Path(__file__).parent.glob("*.py")):
        generator.update(source.name.encode())
        generator.update(source.read_bytes())
    for font in sorted(fonts.directory.glob("*.ttf")):
        generator.update(font.name.encode())
        generator.update(font.read_bytes())
    fingerprint = hashlib.sha256(("printbook-v1\n" + catalog.fingerprint + style
                                  + generator.hexdigest()).encode()).hexdigest()[:12]
    body = BodyLayout(catalog, profile, fonts, 1).run()
    nav = NavigationLayout(catalog, profile, fonts, body.locations, body.ends, body.numbers,
                           fingerprint, len(body.pages)).run()
    body = BodyLayout(catalog, profile, fonts, len(nav.pages) + 1).run()
    total = len(nav.pages) + len(body.pages)
    final_nav = NavigationLayout(catalog, profile, fonts, body.locations, body.ends, body.numbers,
                                 fingerprint, total).run()
    if len(nav.pages) != len(final_nav.pages):
        raise ValueError("导航页数未稳定；未发布 PDF。")
    return Book(catalog, profile, final_nav.pages + body.pages, body.locations,
                body.ends, body.numbers, fingerprint, len(final_nav.pages))


def validate_layout(book, fonts):
    """Check every source line, fragment, anchor and printed text boundary."""
    expected = {e.key: e for e in book.catalog.entries}
    rendered = {key: {} for key in expected}
    anchors, targets = set(), []
    p = book.profile
    for number, page in enumerate(book.pages, 1):
        if page.number != number:
            raise ValueError("PDF 页码不连续。")
        for item in page.items:
            if isinstance(item, Anchor):
                if item.key in anchors:
                    raise ValueError(f"重复导航目标：{item.key}")
                anchors.add(item.key)
            elif isinstance(item, Text):
                w = fonts.width(item.text, item.size, item.role, item.bold)
                if item.x < 0 or item.x + w > p.size[0] + 0.1 or item.y < p.bottom - 1:
                    raise ValueError(f"文字越界，第 {number} 页：{item.text[:60]}")
                if item.target:
                    targets.append(item.target)
        for row in page.rows:
            line = rendered[row.key].setdefault(row.line, [])
            line.append(row.text)
            right = (p.left(number) + row.column * p.width
                     + (row.column - 1) * p.gutter)
            if row.x + fonts.width(row.text, p.font_size, "code") > right + 0.1:
                raise ValueError(f"代码侵入相邻栏：{row.key}:{row.line}")
    for key, entry in expected.items():
        if set(rendered[key]) != set(range(1, len(entry.lines) + 1)):
            raise ValueError(f"打印遗漏或重复源行：{key}")
        for number, source in enumerate(entry.lines, 1):
            if "".join(rendered[key][number]) != source.expandtabs(4):
                raise ValueError(f"代码内容不一致：{key}:{number}")
        if entry.anchor not in anchors or any(s.key not in anchors for s in entry.subsections):
            raise ValueError(f"模板或分节缺少导航：{key}")
    if any(target not in anchors for target in targets):
        raise ValueError("目录含无效链接。")
