# mouhua XCPC 模板库

面向 ICPC/XCPC 的 C++ 算法片段，按题摘取、手打和打印使用。优先保持接口明确、代码长度适中、常数开销可控。维护者：mouhua，QQ：1625382285。

## 编译与使用

使用 **GNU++20**，默认 `-O2`。公共头中的 `__int128`、PBDS 和 `bits` 头依赖 GCC/libstdc++；部分模板使用 C++20 的简写函数模板、比较运算和标准库接口。

```bash
g++ -std=gnu++20 -O2 -pipe -Wall -Wextra main.cpp -o main
```

从 [公共头](template/其他/head.cpp) 开始，把本题需要的片段放到 `solve`/`main` 前。各文件是独立摘取的模板，可能包含同名类型、全局变量或示例入口；不要把全库直接拼接成一个翻译单元。所需的模数、容量、图规模和信息合并规则以该文件顶部及接口注释为准。

[O3.cpp](template/其他/O3.cpp) 仅提供可选的普通 `O3`，在实测有收益时使用；默认不强制 AVX 指令集，也不启用 `Ofast`/`fast-math`。几何中的补偿求和依赖正常浮点求值语义。

## 接口惯例

| 项目 | 约定 |
| --- | --- |
| 整数类型 | 公共头提供 `ll`、`ull`、`i128`、`u128` 等别名；乘法是否需要加宽，以题目数值范围为准。 |
| 下标与区间 | 每份模板保留自己的约定，按文件注释使用。几何统一 0-based；离散化的对外排名为 1-based。区间是闭区间还是半开区间，不跨模板推断。 |
| 初始化与多测 | 每组数据按对应接口重新构造或初始化；预处理后再查询。修改原数组、图或离散化收集集后，相关预处理需重建。 |
| 模板前提 | 模数、字母表、连通性、树/一般图、容量上界等均是算法契约，不能仅通过更换参数假定仍成立。 |
| 几何 | 整数 `P` 做精确判定，`FP` 做近似构造与输出；详见 [几何目录与迁移](template/计算几何/目录与迁移.md)。 |
| 哈希 | 属于概率算法；相等的哈希值不是确定性字符串相等证明。 |

### 取整

[取整.cpp](template/其他/取整.cpp) 提供 `ceils(n, m)` 和 `floors(n, m)`，分别返回整数除法的上取整、下取整，支持正负分子与分母。两个参数使用同一整数类型，要求 `m != 0` 且 `n / m` 可表示；例如 `LLONG_MIN / -1` 不在契约内。

实现先计算商和余数再修正，不计算 `n + m - 1`，也不对最小负数取负。用法：`ceils(-5LL, 2LL) == -2`，`floors(-5LL, 2LL) == -3`。

### 离散化

[离散化.cpp](template/其他/离散化.cpp) 的使用顺序是 `add(...)`、`init()`、查询。`init()` 排序并去重，可重复调用；之后追加值需要再次 `init()`。

| 接口 | 含义 |
| --- | --- |
| `c.n` | 不同值数量，空集为 0。 |
| `c[x]` | 已收录值中 `<= x` 的数量，范围 `0..n`；若 `x` 已收录，就是它的 1-based 排名。 |
| `c(k)` | 排名 `k` 对应的原值，要求 `1 <= k <= n`。 |
| `c.alls` | 无哨兵的 0-based 有序存储；需要从排名还原时使用 `c(k)`。 |

不再占用最小值、最大值作为哨兵，所以整数极值可以正常加入。浮点类型也可使用，但不接受 `NaN`。若要查“是否存在”，不能只判断 `c[x] != 0`，还应验证该排名还原后的值是否等于 `x`。

### 大数的定位

[大数.cpp](template/数学/大数.cpp) 是按 9 位十进制分块的有符号整数实现，当前 168 行（含注释、空行和可选 `gcd`/`lcm`），提供 `+ - * / %`、比较、复合赋值、读写及 `divmod`。除法向零截断，余数与被除数同号，除数必须非零；只需要四则运算时，可省略末尾的 `gcd`/`lcm`。

加减比较为 O(n)，朴素乘法和一般长除为 O(nm)，单块除数走 O(n) 路径；这里 n、m 是 9 位块数。定位是便于手打的通用高精度运算。超长数的密集乘法应按题另接快速乘法，不能仅因为使用大数就忽略二次复杂度。

```cpp
bigint a("123456789012345678901234567890");
bigint b = -7;
auto [q, r] = divmod(a, b); // a == q*b + r
cout << a * b << '\n';
cout << q << ' ' << r << '\n';
```

原生整数构造的入口是 `long long`，支持 `LLONG_MIN..LLONG_MAX`。更大的整数使用十进制字符串或 `cin >> x`；特别是超过 `LLONG_MAX` 的 `unsigned long long`，应写 `bigint(to_string(u))`，不要直接传入构造函数。

本机 `GNU++20 -O2` 微基准如下，单位为毫秒，排除输入生成和 I/O；这些数值仅供判断规模，不承诺其他机器或 OJ 的耗时。可查看 [基准代码](tests/bigint_benchmark.cpp) 和 [原始结果](tests/benchmark_results.json)。

| 十进制位数 | 两个同位数整数相乘 | 除以一半位数的整数 | 除以 `100000007` |
| ---: | ---: | ---: | ---: |
| 1,000 | 0.0246 | 0.0259 | 0.0006 |
| 10,000 | 2.5235 | 1.5380 | 0.0053 |
| 50,000 | 62.0124 | 34.0682 | 0.0265 |

### 本轮接口迁移

| 模板 | 调用端需要调整 |
| --- | --- |
| [线性不定方程](template/数学/ax+by=c求解.cpp) | `solveLinear(a,b,c)` 返回 `optional<LinearSolution>`，先判无解；结构含 `x,y,dx,dy,all`，数值为 `i128`。`all=true` 表示 x、y 各自任取；否则通解是 `(x+k*dx,y+k*dy)`。 |
| [扩展欧几里得求逆元](template/数学/拓展exgcd求逆元.cpp) | `inv(n,M)` 返回 `optional<ll>`，要求 `M>1`；先确认非空再解引用，无逆元不再伪装成一个整数结果。 |
| [KMP 自动机](template/字符串/kmp自动机.cpp) | `autokmp(s)` 直接传普通 0-based 小写原串，不添加 `#` 占位；状态为已匹配前缀长度 `0..s.size()`。 |
| [费用流 B](template/图论/cost_flow_B.cpp) | 浮点费用须显式给有限正 `unit`，例如 `MCF<ll,double> g(n, unit)`；优化的是 `round(cost/unit)`。`flow(s,t)` 返回本次新增流量和原始费用增量，费用增量可以为负；不要当累计值或假定量化不改变最优解。 |
| [LCT](template/数据结构/lct.cpp) | `link`、`cut` 返回 `bool`，调用端可检查连接/断开是否成功。 |
| [mint](template/数学/mint.cpp) 与 [Poly](template/数学/Poly.cpp) | 先放 `mint.cpp`，再放 `Poly.cpp`，共享 `P`、`Z`、`power`，不重复定义；当前 NTT 固定使用 `998244353`。 |
| [离散化](template/其他/离散化.cpp) | 对外排名仍为 1-based；`alls` 去掉哨兵后按 0-based 存储，排名还原用 `c(k)`。 |

## 全库目录

### 其他

| 文件 | 内容 |
| --- | --- |
| [head.cpp](template/其他/head.cpp) | 公共头、别名、调试输出与比赛入口。 |
| [fastIO.cpp](template/其他/fastIO.cpp) | 缓冲整数输入；有效整数、空白分隔，取值须在目标类型范围内。 |
| [取整.cpp](template/其他/取整.cpp) | 整数除法上取整与下取整。 |
| [离散化.cpp](template/其他/离散化.cpp) | 排序去重、1-based 排名及还原。 |
| [O3.cpp](template/其他/O3.cpp) | 可选的普通 `O3` 优化。 |

### 数学

| 文件 | 内容 |
| --- | --- |
| [ax+by=c求解.cpp](template/数学/ax+by=c求解.cpp) | 线性不定方程。 |
| [拓展exgcd求逆元.cpp](template/数学/拓展exgcd求逆元.cpp) | 扩展欧几里得与逆元。 |
| [分数类.cpp](template/数学/分数类.cpp) | 分数运算。 |
| [mint.cpp](template/数学/mint.cpp) | 模整数。 |
| [Poly.cpp](template/数学/Poly.cpp) | 多项式与 NTT/FPS 运算。 |
| [PollardRho.cpp](template/数学/PollardRho.cpp) | 素性检验与整数分解。 |
| [线性基.cpp](template/数学/线性基.cpp) | 异或线性基。 |
| [莫比乌斯反演.cpp](template/数学/莫比乌斯反演.cpp) | 筛法与莫比乌斯函数相关预处理。 |
| [大数.cpp](template/数学/大数.cpp) | 有符号任意精度整数。 |

### 数据结构

| 文件 | 内容 |
| --- | --- |
| [树状数组.cpp](template/数据结构/树状数组.cpp) | 树状数组。 |
| [二维树状数组.cpp](template/数据结构/二维树状数组.cpp) | 稠密矩阵单点加、前缀和与闭矩形和。 |
| [离线二维树状数组.cpp](template/数据结构/离线二维树状数组.cpp) | 预登记修改位置的稀疏二维单点加与矩形和。 |
| [单点修改线段树.cpp](template/数据结构/单点修改线段树.cpp) | 单点修改与区间查询。 |
| [二维线段树.cpp](template/数据结构/二维线段树.cpp) | 稠密矩阵单点赋值与闭矩形合并。 |
| [区间修改+lazy线段树.cpp](template/数据结构/区间修改+lazy线段树.cpp) | 懒标记线段树。 |
| [主席树.cpp](template/数据结构/主席树.cpp) | 可持久化线段树。 |
| [st表.cpp](template/数据结构/st表.cpp) | 静态稀疏表。 |
| [二维ST.cpp](template/数据结构/二维ST.cpp) | 静态矩形 min/max 等幂等查询，每层仅分配有效起点。 |
| [RMQ.cpp](template/数据结构/RMQ.cpp) | 静态区间最值查询。 |
| [HLD.cpp](template/数据结构/HLD.cpp) | 重链剖分。 |
| [lct.cpp](template/数据结构/lct.cpp) | 动态树 Link-Cut Tree。 |
| [Trh.cpp](template/数据结构/Trh.cpp) | HLD 与线段树的路径操作封装。 |
| [笛卡尔树.cpp](template/数据结构/笛卡尔树.cpp) | 笛卡尔树。 |
| [珂朵莉树.cpp](template/数据结构/珂朵莉树.cpp) | 有序区间集合。 |
| [动态中位数.cpp](template/数据结构/动态中位数.cpp) | 动态中位数维护。 |
| [jls分块.cpp](template/数据结构/jls分块.cpp) | 分块模板。 |
| [莫队.cpp](template/数据结构/莫队.cpp) | 离线区间查询排序与移动。 |
| [二维结构用法.md](template/数据结构/二维结构用法.md) | 四种二维结构的选型、共同 Info、点修改示例、坐标契约及内存估算。 |

二维稠密矩阵统一 1-based，所有 `query` 参数均为闭矩形 `(x1,y1,x2,y2)`。二维 BIT 做点加；二维线段树做点赋值；二维 ST 用于静态幂等查询。稀疏坐标提前登记修改点后，离线二维 BIT 仍按原操作顺序执行查询与修改。代码和内存示例见 [二维结构用法](template/数据结构/二维结构用法.md)。

### 图论

| 文件 | 内容 |
| --- | --- |
| [并查集.cpp](template/图论/并查集.cpp) | 并查集。 |
| [Boruvka.cpp](template/图论/Boruvka.cpp) | 最小生成树。 |
| [SCC.cpp](template/图论/SCC.cpp) | 强连通分量。 |
| [VBCC.cpp](template/图论/VBCC.cpp) | 点双连通分量。 |
| [twosat.cpp](template/图论/twosat.cpp) | 2-SAT。 |
| [倍增lca.cpp](template/图论/倍增lca.cpp) | 倍增 LCA。 |
| [O1 LCA.cpp](template/图论/O1%20LCA.cpp) | 静态 LCA。 |
| [Dinic.cpp](template/图论/Dinic.cpp) | 最大流。 |
| [HLPP.cpp](template/图论/HLPP.cpp) | 最高标号预流推进最大流。 |
| [cost_flow_A.cpp](template/图论/cost_flow_A.cpp) | 费用流方案 A。 |
| [cost_flow_B.cpp](template/图论/cost_flow_B.cpp) | 费用流方案 B。 |

### 字符串

| 文件 | 内容 |
| --- | --- |
| [kmp.cpp](template/字符串/kmp.cpp) | KMP 匹配。 |
| [kmp自动机.cpp](template/字符串/kmp自动机.cpp) | KMP 自动机。 |
| [trie.cpp](template/字符串/trie.cpp) | 字典树。 |
| [acm.cpp](template/字符串/acm.cpp) | AC 自动机。 |
| [sam.cpp](template/字符串/sam.cpp) | 后缀自动机。 |
| [后缀数组.cpp](template/字符串/后缀数组.cpp) | 后缀数组。 |
| [sa+rmq求lcp.cpp](template/字符串/sa+rmq求lcp.cpp) | 后缀数组、RMQ 与 LCP 查询。 |
| [PAM.cpp](template/字符串/PAM.cpp) | 回文自动机。 |
| [mancher.cpp](template/字符串/mancher.cpp) | Manacher 回文半径。 |
| [hashhuiwen.cpp](template/字符串/hashhuiwen.cpp) | 哈希回文查询。 |
| [自动取模类hash.cpp](template/字符串/自动取模类hash.cpp) | 字符串哈希。 |

### 计算几何

| 文件 | 内容 |
| --- | --- |
| [geometry.cpp](template/计算几何/geometry.cpp) | 点线、凸包、圆、卡壳、半平面交、扫描线、覆盖面积等 17 节。 |
| [目录与迁移.md](template/计算几何/目录与迁移.md) | 分节依赖、函数入口、返回值、复杂度及摘取说明。 |

## 回归测试

算法测试使用 Python 3.9+ 和 GNU++20，采用固定种子，包含小规模暴力对照、边界输入和已修问题的复现。PDF 构建与回归建议使用 Python 3.11+，并先安装下方依赖。测试文件独立于手打片段，不增加比赛代码依赖。运行所有测试：

```bash
python3 -m pip install -r requirements-pdf.txt
python3 tests/run.py
python3 tests/run.py --sanitize
```

默认使用 `-O2` 编译 C++ 回归，并执行 PDF 回归；`--sanitize` 增加 UBSan 和 `_GLIBCXX_ASSERTIONS`。可选 `python3 tests/run.py --asan` 进行 AddressSanitizer 检查，需要运行环境支持。PDF 回归会实际生成并读取 PDF，检查嵌入字体、中文提取、书签和目录目标、源码行覆盖、三种版式及失败时保留旧输出。实际打印前仍应查看成品和试印页。

默认串行编译以控制内存占用；需要时可加 `--jobs 2`。单独运行一组可用 `--only common`、`--only geometry`、`--only bigint` 或 `--only pdf`。仓库也提供 [GitHub Actions](.github/workflows/verify.yml) 自动运行回归。

二维结构专项回归：

```bash
python3 tests/run.py --only ds2d
python3 tests/run.py --sanitize --only ds2d
```

只运行 PDF 与书目回归，无需 C++ 编译器：

```bash
python3 tests/run.py --only pdf
```

也可直接运行对应 Python 单测：

```bash
python3 -m unittest discover -s tests -p 'test_p*.py' -v
```

测试通过表示已覆盖的样例和性质通过，不能替代题目特有的数值、内存、复杂度与输入前提检查。

## 生成打印 PDF

在仓库根目录执行，保留原来的生成命令：

```bash
python3 -m pip install -r requirements-pdf.txt
python3 create_template_pdf.py
```

默认输出为 `output/pdf/algorithm_templates.pdf`，同目录的 `algorithm_templates.manifest.json` 记录源码哈希、页码和分栏位置。**仓库根目录旧的 `algorithm_templates.pdf` 不再是默认输出**；需要指定位置时使用 `--output`。生成器直接嵌入仓库自带字体，安装 Python 依赖后即可离线构建，无需 TeX 或额外安装系统字体。

默认递归收录 `template` 中的 `.cpp`、`.hpp`、`.h`，当前包含 57 份模板，以及显式加入的 [FC 压缩二维线段树](research/2d_point_query/range_fc.hpp)。[printbook.json](printbook.json) 管理分类顺序、显示标题、别名和简短说明；新增源码会自动追加，研究目录仅收录明确列出的文件。Markdown 说明书不原文插入比赛代码本，源码注释完整保留。

PDF 提供可点击目录、算法别名速查、分层书签、源码路径、原始行号和续栏标题；几何目录可直接定位到内部 17 节。目录标出的页码与 PDF 物理页码一致。每页的版本指纹与旁边清单对应，方便核对打印稿。

| 版式 | 页面与分栏 | 代码字号／行距 | 用途 |
| --- | --- | --- | --- |
| `print`（默认） | A4 横向双栏 | 8.6／10.5 pt | 日常打印与赛场查找 |
| `compact` | A4 横向三栏 | 7.5／9.2 pt | 优先减少页数，建议先试印 |
| `readable` | A4 纵向单栏 | 10／12.5 pt | 大字号阅读与逐行核对 |

```bash
# 查看实际收录项；这一步无需 PDF 依赖
python3 create_template_pdf.py --list

# 只检查字体、源码行覆盖和分页，不写文件
python3 create_template_pdf.py --check

# 单独生成数据结构和图论
python3 create_template_pdf.py --category 数据结构 --category 图论 --output output/pdf/ds_graph.pdf

# 紧凑版与大字号版
python3 create_template_pdf.py --profile compact --output output/pdf/compact.pdf
python3 create_template_pdf.py --profile readable --output output/pdf/readable.pdf
```

打印默认版与紧凑版时选择 **A4 横向、100% 实际大小、双面短边翻转**；`readable` 使用 **A4 纵向、双面长边翻转**。生成的 PDF 已经分栏，打印机每张纸打印 1 个 PDF 页面即可。

完整参数、筛选与配置示例、页数上限及维护流程见 [打印与生成指南](docs/PRINTING.md)。生成器会在编码、字形、配置引用、源码覆盖或布局校验失败时停止；`--max-pages N` 超限时也直接失败，不会自动缩小字号。正式 PDF 经过生成后校验才原子替换，构建失败会保留已有 PDF。
