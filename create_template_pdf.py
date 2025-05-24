import os
import subprocess
import re

# 生成LaTeX文件
def create_latex_file():
    # 创建LaTeX文件头部
    latex_content = """\\documentclass[a4paper,10pt]{article}
\\usepackage{ctex}
\\usepackage{listings}
\\usepackage{xcolor}
\\usepackage[margin=0.7cm,top=1.2cm,bottom=1.2cm,nomarginpar]{geometry}
\\usepackage[hidelinks]{hyperref}
\\usepackage{fancyhdr}

\\title{ACM算法竞赛模板}
\\author{mouhua}
\\date{\\today}

% 代码格式设置
\\lstset{
    backgroundcolor=\\color[RGB]{245,245,245},
    keywordstyle=\\color[RGB]{40,40,255},
    commentstyle=\\color[RGB]{0,96,96},
    stringstyle=\\color[RGB]{128,0,0},
    breaklines=true,
    numberstyle=\\tiny\\color{gray},
    numbers=left,
    basicstyle=\\ttfamily\\small,
    frame=single,
    tabsize=4,
    showstringspaces=false,
    language=C++,
    xleftmargin=0pt,
    xrightmargin=0pt,
    keepspaces=true,
    columns=flexible,
    belowskip=-0.1em,
    aboveskip=0.1em,
    linewidth=\\textwidth,
    resetmargins=true,
    breakindent=0pt
}

% 设置页眉页脚
\\pagestyle{fancy}
\\fancyhf{}
\\fancyhead[C]{ACM算法竞赛模板}
\\fancyfoot[C]{\\thepage}
\\renewcommand{\\headrulewidth}{0.4pt}
\\renewcommand{\\footrulewidth}{0.4pt}
\\setlength{\\headheight}{13.6pt}
\\setlength{\\headsep}{10pt}
\\setlength{\\footskip}{13pt}

\\begin{document}

\\maketitle
\\thispagestyle{fancy}
\\newpage
\\tableofcontents
\\thispagestyle{fancy}
\\newpage

"""
    
    # 获取模板目录下的所有子目录
    template_dir = "template"
    categories = os.listdir(template_dir)
    categories.sort()  # 对目录进行排序
    
    # 遍历每个子目录
    for category in categories:
        category_path = os.path.join(template_dir, category)
        if os.path.isdir(category_path):
            # 添加分类标题
            latex_content += f"\\section{{{category}}}\n\\thispagestyle{{fancy}}\n\n"
            
            # 获取该目录下的所有cpp文件
            cpp_files = [f for f in os.listdir(category_path) if f.endswith('.cpp')]
            cpp_files.sort()  # 对文件名进行排序
            
            # 遍历每个cpp文件
            for cpp_file in cpp_files:
                file_path = os.path.join(category_path, cpp_file)
                # 添加文件标题（不包含扩展名）
                title = os.path.splitext(cpp_file)[0]
                latex_content += f"\\subsection{{{title}}}\n\\thispagestyle{{fancy}}\n\n"
                
                # 读取文件内容
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    code_content = f.read()
                
                # 添加代码环境
                latex_content += "\\noindent\\begin{lstlisting}\n"
                latex_content += code_content
                latex_content += "\\end{lstlisting}\n\n"
    
    # 文档结束
    latex_content += "\\end{document}"
    
    # 写入LaTeX文件
    with open("algorithm_templates.tex", "w", encoding="utf-8") as tex_file:
        tex_file.write(latex_content)
    
    return "algorithm_templates.tex"

# 生成PDF
def generate_pdf(tex_file):
    try:
        # 运行xelatex两次以确保目录正确
        subprocess.run(["xelatex", "-interaction=nonstopmode", tex_file], check=True)
        subprocess.run(["xelatex", "-interaction=nonstopmode", tex_file], check=True)
        print(f"PDF生成成功！文件名: {os.path.splitext(tex_file)[0]}.pdf")
        return True
    except subprocess.CalledProcessError as e:
        print(f"PDF生成失败: {e}")
        return False
    except FileNotFoundError:
        print("错误: 未找到xelatex。请确保已安装TeX Live或MiKTeX并将其添加到PATH中。")
        return False

def main():
    print("开始生成算法模板PDF...")
    tex_file = create_latex_file()
    if generate_pdf(tex_file):
        print("处理完成！")
    else:
        print("生成PDF时出错，请检查错误信息。")

if __name__ == "__main__":
    main() 