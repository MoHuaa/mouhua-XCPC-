"""One-command entry point; catalog listing needs no PDF packages."""
import argparse
from pathlib import Path
import sys

from .catalog import load_catalog


def main(argv=None, root=None):
    root = Path(root or Path(__file__).resolve().parents[1]).resolve()
    parser = argparse.ArgumentParser(description="生成便于打印和检索的 XCPC 模板 PDF。")
    parser.add_argument("--config", type=Path, help="书目配置 JSON；默认仓库 printbook.json")
    parser.add_argument("-o", "--output", type=Path,
                        help="输出 PDF；默认 output/pdf/algorithm_templates.pdf")
    parser.add_argument("--profile", choices=("print", "compact", "readable"), default="print",
                        help="print 横向双栏；compact 横向三栏；readable 纵向单栏")
    parser.add_argument("--font-size", type=float, help="覆盖代码字号，单位 pt")
    parser.add_argument("--category", action="append", default=[], help="只打印指定分类，可重复")
    parser.add_argument("--exclude", action="append", default=[], help="排除源码路径 glob，可重复")
    parser.add_argument("--max-pages", type=int, help="页数超过此值就失败，不会自动缩小字号")
    modes = parser.add_mutually_exclusive_group()
    modes.add_argument("--list", action="store_true", help="仅列出实际收录项，无需 PDF 依赖")
    modes.add_argument("--check", action="store_true", help="校验收录、字体、分页与代码完整性，不写 PDF")
    args = parser.parse_args(argv)
    try:
        catalog = load_catalog(root, args.config, tuple(args.category), tuple(args.exclude))
        if args.list:
            for entry in catalog.entries:
                print(f"{entry.category}\t{entry.title}\t{entry.key}")
            print(f"共 {len(catalog.entries)} 份源码，{sum(len(e.lines) for e in catalog.entries)} 行。")
            return 0
        from .fonts import FontBook
        from .layout import build_layout, get_profile, validate_layout
        from .render import publish
        fonts = FontBook()
        profile = get_profile(args.profile, args.font_size)
        book = build_layout(catalog, profile, fonts)
        validate_layout(book, fonts)
        if args.max_pages is not None:
            if args.max_pages < 1:
                raise ValueError("--max-pages 必须为正整数。")
            if len(book.pages) > args.max_pages:
                raise ValueError(f"当前排版 {len(book.pages)} 页，超过限制 {args.max_pages} 页；"
                                 "请筛选模板或选择 --profile compact。")
        print(f"{len(catalog.entries)} 份源码 / {sum(len(e.lines) for e in catalog.entries)} 行 / "
              f"{len(book.pages)} 页（导航 {book.navigation_pages} 页）")
        print(f"{profile.label}，代码 {profile.font_size:g} pt，版本 {book.fingerprint}")
        if args.check:
            print("收录、字形、原始行覆盖、栏边界和导航目标检查通过。")
            return 0
        output = (args.output or root / "output" / "pdf" / "algorithm_templates.pdf").resolve()
        manifest = publish(book, fonts, output)
        print(f"PDF：{output}")
        print(f"页码与源码清单：{manifest}")
        return 0
    except ImportError as error:
        print(f"缺少 PDF 依赖：{error}\n请执行：python3 -m pip install -r requirements-pdf.txt",
              file=sys.stderr)
    except (OSError, UnicodeError, ValueError, TypeError) as error:
        print(f"生成失败：{error}", file=sys.stderr)
    return 1
