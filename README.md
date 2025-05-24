# mouhua
# 联系方式
QQ:1625382285
# ACM算法模板生成器

这个工具可以将template文件夹下的C++算法模板文件转换成易于打印的PDF文档，包含目录和页码。

## 功能

- 自动收集template文件夹中所有子文件夹的C++模板代码
- 生成带有层次结构目录的PDF文件（按照算法分类组织）
- 包含页码，方便查找
- 代码高亮显示，易于阅读

## 使用前提

使用此脚本需要安装以下软件：

1. Python 3.x
2. TeX Live或MiKTeX（提供xelatex命令）
   - Windows: https://www.tug.org/texlive/ 或 https://miktex.org/
   - Linux: `sudo apt install texlive-full`（Ubuntu/Debian）
   - MacOS: `brew install --cask mactex`

确保还安装了以下LaTeX包：
- ctex（中文支持）
- listings（代码显示）
- xcolor（颜色支持）
- geometry（页面布局）
- hyperref（超链接支持）
- fancyhdr（页眉页脚）

## 使用方法

1. 确保您的模板文件已经组织在template文件夹的不同子文件夹中
2. 运行Python脚本：
   ```
   python create_template_pdf.py
   ```
3. 生成的PDF文件将命名为`algorithm_templates.pdf`

## 文件结构要求

程序假设您的模板文件按以下结构组织：

```
template/
  ├── 数据结构/
  │   ├── 线段树.cpp
  │   ├── 树状数组.cpp
  │   └── ...
  ├── 图论/
  │   ├── 最短路.cpp
  │   └── ...
  └── ...
```

每个分类会生成为一个章节，每个文件会生成为一个小节。 