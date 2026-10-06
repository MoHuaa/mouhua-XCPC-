# PDF 打印字体

这里保存 PDF 生成器使用的完整 TrueType 字体，构建时不需要联网或安装系统字体。
这些文件均未按当前模板内容裁剪；以后新增中文内容仍可从完整字库取字。
ReportLab 生成 PDF 时再嵌入所用字形的子集，减小最终 PDF 体积。

| 文件 | 用途 | 版本 | 文件大小 | 许可证 |
| --- | --- | --- | ---: | --- |
| `Inconsolata-Regular.ttf` | 代码正文 | 3.000 | 108,684 B | SIL Open Font License 1.1 |
| `Inconsolata-Bold.ttf` | 代码强调、粗体关键字 | 3.000 | 109,728 B | SIL Open Font License 1.1 |
| `DroidSansFallback.ttf` | 中文文字、中文注释 | 2.55b | 3,451,900 B | Apache License 2.0 |
| `DejaVuSans.ttf` | 希腊字母、数学符号、其他后备字形 | 2.37 | 757,076 B | 字体内附 Bitstream Vera / Arev 许可；DejaVu 的改动为公有领域 |

四个字体共 4,427,388 B（约 4.22 MiB），不依赖 PDF 阅读器的字体替换。
Inconsolata 的 ASCII 字符固定为 `0.5 em`，中文方块字在 Droid Sans Fallback 中为 `1 em`，适合混排与双栏代码。
不同字体有各自的字符覆盖范围；生成器应按字符检查字体覆盖，不能把全部非 ASCII 字符直接交给中文字体。
例如 `π` 需要 DejaVu Sans。

## 来源及许可文件

### Droid Sans Fallback

字体直接取自 Android Open Source Project 的 `android10-release` 分支，未经修改：

- [官方字体](https://android.googlesource.com/platform/frameworks/base/+/android10-release/data/fonts/DroidSansFallback.ttf)
- [官方目录 NOTICE](https://android.googlesource.com/platform/frameworks/base/+/android10-release/data/fonts/NOTICE)
- 本目录保留完整 NOTICE 和 Apache 2.0 条款：`DroidSansFallback-NOTICE.txt`。

字体自身的版权字段为 `Digitized data copyright Google Corporation © 2006`，
许可字段明确为 Apache License 2.0。这个文件是官方完整字库，不是从其他 PDF 中提取的字体子集。

### Inconsolata

Regular 和 Bold 文件取自构建环境附带的 Inconsolata 3.000 静态 TrueType 发行文件，未经修改。
作者与许可证信息均保留在字体 `name` 表中。

- [上游项目](https://github.com/googlefonts/Inconsolata)
- [上游 OFL 许可](https://github.com/googlefonts/Inconsolata/blob/main/OFL.txt)
- 本目录保留上游完整许可：`Inconsolata-OFL.txt`。
- 版权：`Copyright 2006 The Inconsolata Project Authors (https://github.com/cyrealtype/Inconsolata)`。

### DejaVu Sans

字体取自构建环境附带的 DejaVu Sans 2.37 TrueType 发行文件，未经修改。
`DejaVuSans-LICENSE.txt` 是该字体 `name` 表中许可文本的完整副本，
保留 Bitstream Vera 和 Arev 字形的版权及许可。

- [上游项目](https://github.com/dejavu-fonts/dejavu-fonts)
- [上游 LICENSE](https://github.com/dejavu-fonts/dejavu-fonts/blob/master/LICENSE)

字体的许可与本仓库程序代码的许可分别适用。再分发字体时应一并保留本目录中的许可文件。

## 完整性与验证

`SHA256SUMS` 记录上述四个字体及三个许可文件的 SHA-256，可在本目录运行：

```sh
sha256sum -c SHA256SUMS
```

接入时已验证：

- 四个字体均能由 ReportLab `TTFont` 注册并子集嵌入。
- 生成 PDF 后，可完整提取中文、ASCII 代码以及 `π θ Ω λ ∑ ≤ ≥ ≠ → ↔ ∞ √ ∏ ∈ ∀ ∃`。
- 四个字库联合覆盖了验证时 `template/` 内文本与文件名中的全部非控制字符。

构建仍应检查实际内容的缺失字形；本次扫描不代表字库覆盖所有 Unicode 字符。
