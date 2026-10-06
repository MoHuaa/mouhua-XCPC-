"""Collect source code and navigation metadata without modifying either.

Automatic discovery is deliberately independent of the optional book index:
new source files remain printable even before they acquire titles or aliases.
Explicit references, on the other hand, must resolve so stale configuration
cannot silently remove an algorithm from the printed book.
"""

from __future__ import annotations

from dataclasses import dataclass
from fnmatch import fnmatchcase
from hashlib import sha256
import json
from pathlib import Path, PurePosixPath
import re
from typing import Any


@dataclass(frozen=True)
class Section:
    key: str
    title: str
    line: int


@dataclass(frozen=True)
class Entry:
    key: str
    category: str
    title: str
    aliases: tuple[str, ...]
    summary: str
    text: str
    lines: tuple[str, ...]
    subsections: tuple[Section, ...]
    source_hash: str

    @property
    def anchor(self) -> str:
        return _anchor(self.key)


@dataclass(frozen=True)
class Catalog:
    title: str
    author: str
    entries: tuple[Entry, ...]
    category_titles: dict[str, str]
    fingerprint: str


_ROOT_FIELDS = {
    "title", "author", "source_root", "extensions", "categories", "entries",
    "extra_sources",
}
_META_FIELDS = {"title", "aliases", "summary"}
_SECTION = re.compile(r"^\s*//\s*={3,}\s*\[(\d+)\]\s*(.*?)\s*={3,}\s*$")


def _anchor(key: str) -> str:
    return "entry-" + sha256(key.encode("utf-8")).hexdigest()[:12]


def _natural_key(value: str) -> tuple:
    chunks = tuple((1, int(s)) if s.isdecimal() else (0, s.casefold())
                   for s in re.split(r"(\d+)", value))
    return chunks, value


def _object(value: Any, label: str, fields: set[str] | None = None) -> dict:
    if not isinstance(value, dict):
        raise ValueError(f"{label} 必须是对象")
    if fields is not None and set(value) - fields:
        raise ValueError(f"{label} 含未知字段：{', '.join(sorted(set(value) - fields))}")
    return value


def _string(value: Any, label: str, *, empty: bool = False) -> str:
    if not isinstance(value, str) or (not empty and not value.strip()):
        raise ValueError(f"{label} 必须是{'可为空的' if empty else '非空'}字符串")
    return value


def _strings(value: Any, label: str) -> tuple[str, ...]:
    if not isinstance(value, list):
        raise ValueError(f"{label} 必须是字符串数组")
    result = tuple(_string(v, f"{label}[{i}]") for i, v in enumerate(value))
    if len(set(result)) != len(result):
        raise ValueError(f"{label} 中有重复项")
    return result


def _relative(value: Any, label: str) -> str:
    value = _string(value, label)
    path = PurePosixPath(value)
    # Require a single spelling for each path, on every operating system.
    if (path.is_absolute() or "\\" in value or ":" in value
            or any(p in {"", ".", ".."} for p in value.split("/"))):
        raise ValueError(f"{label} 必须是使用 / 的相对路径，不能含 . 或 ..：{value}")
    return path.as_posix()


def _within(root: Path, path: Path, label: str) -> Path:
    resolved = path.resolve()
    if not resolved.is_relative_to(root):
        raise ValueError(f"{label} 指向仓库之外：{path}")
    return resolved


def _decode(raw: bytes, label: str) -> str:
    try:
        return raw.decode("utf-8-sig")
    except UnicodeDecodeError as error:
        raise UnicodeDecodeError(error.encoding, error.object, error.start,
                                 error.end, f"{label}: {error.reason}") from error


def _unique_object(pairs: list[tuple[str, Any]]) -> dict:
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"JSON 中有重复字段：{key}")
        result[key] = value
    return result


def _metadata(value: Any, label: str, fallback: str) -> tuple[str, tuple[str, ...], str]:
    value = _object(value, label, _META_FIELDS)
    return (
        _string(value.get("title", fallback), f"{label}.title"),
        _strings(value.get("aliases", []), f"{label}.aliases"),
        _string(value.get("summary", ""), f"{label}.summary", empty=True),
    )


def _sections(key: str, lines: tuple[str, ...]) -> tuple[Section, ...]:
    result = []
    seen = set()
    for line_number, line in enumerate(lines, 1):
        match = _SECTION.fullmatch(line)
        if not match:
            continue
        section_key = f"{_anchor(key)}-s{int(match[1]):02d}"
        title = match[2].strip()
        if not title or section_key in seen:
            raise ValueError(f"{key}:{line_number} 的分节编号重复或标题为空")
        seen.add(section_key)
        result.append(Section(section_key, title, line_number))
    return tuple(result)


def load_catalog(
    root: Path,
    config_path: Path | None = None,
    selected_categories: tuple[str, ...] = (),
    exclude: tuple[str, ...] = (),
) -> Catalog:
    """Load the complete book, or a filtered subset, with deterministic order.

    ``exclude`` contains glob patterns matched against either repository-relative
    paths or paths relative to ``source_root``. Category filters use directory
    keys, not display titles. Filters do not weaken configuration validation.
    The decoded ``text`` preserves source whitespace and line endings; ``lines``
    contains the same logical lines without their newline characters. UTF-8 BOMs
    are accepted, while ``source_hash`` always hashes the original file bytes.
    """
    root = Path(root).resolve()
    if not root.is_dir():
        raise ValueError(f"仓库目录不存在：{root}")
    explicit_config = config_path is not None
    config_file = Path(config_path) if explicit_config else root / "printbook.json"
    if not config_file.is_absolute():
        config_file = root / config_file
    config = {}
    if config_file.exists() or explicit_config:
        config = json.loads(_decode(config_file.read_bytes(), str(config_file)),
                            object_pairs_hook=_unique_object)
    config = _object(config, "配置", _ROOT_FIELDS)
    title = _string(config.get("title", "mouhua XCPC 算法模板"), "title")
    author = _string(config.get("author", "mouhua"), "author", empty=True)
    source_name = _relative(config.get("source_root", "template"), "source_root")
    source = root / source_name
    _within(root, source, "source_root")
    if not source.is_dir():
        raise ValueError(f"源码目录不存在：{source}")
    extensions = _strings(config.get("extensions", [".cpp", ".hpp", ".h"]), "extensions")
    if (not extensions or any(not re.fullmatch(r"\.[A-Za-z0-9]+", e) for e in extensions)
            or len({e.lower() for e in extensions}) != len(extensions)):
        raise ValueError("extensions 必须是互不重复的扩展名，例如 .cpp、.hpp、.h")
    extension_set = {e.lower() for e in extensions}

    categories = config.get("categories", [])
    if not isinstance(categories, list):
        raise ValueError("categories 必须是数组")
    category_titles: dict[str, str] = {}
    orders: dict[str, tuple[str, ...]] = {}
    for i, category in enumerate(categories):
        label = f"categories[{i}]"
        category = _object(category, label, {"key", "title", "order"})
        key = _string(category.get("key"), f"{label}.key")
        if "/" in key or "\\" in key or key in {".", ".."}:
            raise ValueError(f"{label}.key 必须是分类目录名")
        if key in category_titles:
            raise ValueError(f"分类重复：{key}")
        category_titles[key] = _string(category.get("title", key), f"{label}.title")
        orders[key] = tuple(_relative(p, f"{label}.order")
                            for p in _strings(category.get("order", []), f"{label}.order"))

    metadata = _object(config.get("entries", {}), "entries")
    metadata = {_relative(k, "entries 的路径"): v for k, v in metadata.items()}
    # Records keep their original relative spellings: display paths and anchors
    # must not depend on where the repository is checked out.
    records = []
    discovered = {}
    physical_paths = {}

    def register(path: Path, category: str, relative: str | None, meta: Any) -> None:
        key = path.relative_to(root).as_posix()
        physical = _within(root, path, key)
        if not path.is_file():
            raise ValueError(f"显式源码文件不存在：{key}")
        if physical in physical_paths:
            raise ValueError(f"源码重复：{key} 与 {physical_paths[physical]}")
        physical_paths[physical] = key
        entry_title, aliases, summary = _metadata(meta, key, path.stem)
        records.append((path, key, category, relative, entry_title, aliases, summary))

    for path in sorted(source.rglob("*"), key=lambda p: _natural_key(p.as_posix())):
        if not path.is_file() or path.suffix.lower() not in extension_set:
            continue
        relative = path.relative_to(source).as_posix()
        parts = PurePosixPath(relative).parts
        category = parts[0] if len(parts) > 1 else "其他"
        discovered[relative] = (category, "/".join(parts[1:]) if len(parts) > 1 else relative)
        register(path, category, relative, metadata.get(relative, {}))
    missing = set(metadata) - set(discovered)
    if missing:
        raise ValueError("entries 引用了未收录的源码：" + ", ".join(sorted(missing)))

    # Resolve configured order before filtering. A typo must not go unnoticed
    # just because a particular print run excludes the affected category.
    item_order = {}
    for category, names in orders.items():
        for index, name in enumerate(names):
            matches = [relative for relative, pair in discovered.items()
                       if pair == (category, name)]
            if len(matches) != 1:
                raise ValueError(f"分类 {category} 的 order 路径不存在或不唯一：{name}")
            item_order[matches[0]] = index

    extras = config.get("extra_sources", [])
    if not isinstance(extras, list):
        raise ValueError("extra_sources 必须是数组")
    for i, extra in enumerate(extras):
        label = f"extra_sources[{i}]"
        extra = _object(extra, label, _META_FIELDS | {"path", "category"})
        path = root / _relative(extra.get("path"), f"{label}.path")
        category = _string(extra.get("category"), f"{label}.category")
        register(path, category, None, {k: v for k, v in extra.items() if k in _META_FIELDS})

    present = {record[2] for record in records}
    unknown_selected = set(selected_categories) - present
    if unknown_selected:
        raise ValueError("所选分类不存在或没有源码：" + ", ".join(sorted(unknown_selected)))
    for category in sorted(present - category_titles.keys(), key=_natural_key):
        category_titles[category] = category
    category_order = {category: index for index, category in enumerate(category_titles)}
    for pattern in exclude:
        _string(pattern, "exclude")
        if not any(fnmatchcase(r[1], pattern) or (r[3] is not None and fnmatchcase(r[3], pattern))
                   for r in records):
            raise ValueError(f"exclude 没有匹配任何源码：{pattern}")

    def record_order(record: tuple) -> tuple:
        _, key, category, relative, *_ = record
        if relative is None:
            # Python's stable sort preserves declaration order among extras.
            return category_order[category], 2, 0, ()
        if relative in item_order:
            return category_order[category], 0, item_order[relative], ()
        return category_order[category], 1, 0, _natural_key(relative)

    entries = []
    for path, key, category, relative, entry_title, aliases, summary in sorted(records, key=record_order):
        if selected_categories and category not in selected_categories:
            continue
        if any(fnmatchcase(key, pattern) or (relative is not None and fnmatchcase(relative, pattern))
               for pattern in exclude):
            continue
        raw = path.read_bytes()
        text = _decode(raw, key)
        lines = tuple(text.splitlines())
        entries.append(Entry(key, category, entry_title, aliases, summary, text, lines,
                             _sections(key, lines), sha256(raw).hexdigest()))
    if not entries:
        raise ValueError("没有可打印的源码；请检查源码目录、分类选择和排除规则")
    category_titles = {key: value for key, value in category_titles.items()
                       if any(entry.category == key for entry in entries)}
    fingerprint_data = {
        "title": title, "author": author, "category_titles": category_titles,
        "entries": [{
            "key": e.key, "category": e.category, "title": e.title,
            "aliases": e.aliases, "summary": e.summary, "source_hash": e.source_hash,
        } for e in entries],
    }
    fingerprint = sha256(json.dumps(fingerprint_data, ensure_ascii=False, sort_keys=True,
                                    separators=(",", ":")).encode("utf-8")).hexdigest()
    return Catalog(title, author, tuple(entries), category_titles, fingerprint)
