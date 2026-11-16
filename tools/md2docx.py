"""Собирает REPORT.docx из REPORT.md в каждой папке labN.

Запуск из корня репозитория:  python tools/md2docx.py
Нужен пакет python-docx (pip install python-docx).
Поддерживается только то, что используется в отчётах: заголовки, абзацы,
списки, блоки кода, картинки, **жирный** и `код` внутри строки.
"""
import re
import sys
from pathlib import Path

from docx import Document
from docx.shared import Cm, Pt

INLINE = re.compile(r"(\*\*.+?\*\*|`[^`]+`)")


def add_inline(par, text):
    for part in INLINE.split(text):
        if not part:
            continue
        if part.startswith("**"):
            par.add_run(part[2:-2]).bold = True
        elif part.startswith("`"):
            run = par.add_run(part[1:-1])
            run.font.name = "Consolas"
        else:
            par.add_run(part)


def convert(md_path: Path):
    doc = Document()
    style = doc.styles["Normal"]
    style.font.name = "Times New Roman"
    style.font.size = Pt(12)

    lines = md_path.read_text(encoding="utf-8").splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("```"):
            code = []
            i += 1
            while i < len(lines) and not lines[i].startswith("```"):
                code.append(lines[i])
                i += 1
            par = doc.add_paragraph()
            run = par.add_run("\n".join(code))
            run.font.name = "Consolas"
            run.font.size = Pt(10)
        elif line.startswith("#"):
            level = len(line) - len(line.lstrip("#"))
            doc.add_heading(line[level:].strip(), level=min(level, 3))
        elif m := re.match(r"!\[(.*?)\]\((.+?)\)", line):
            doc.add_picture(str(md_path.parent / m.group(2)), width=Cm(16))
            cap = doc.add_paragraph(m.group(1))
            cap.runs[0].italic = True
        elif re.match(r"^(- |\d+\. )", line):
            numbered = line[0].isdigit()
            text = re.sub(r"^(- |\d+\. )", "", line)
            par = doc.add_paragraph(style="List Number" if numbered else "List Bullet")
            add_inline(par, text)
        elif line.strip():
            add_inline(doc.add_paragraph(), line)
        i += 1

    out = md_path.with_suffix(".docx")
    doc.save(out)
    print("written", out)


if __name__ == "__main__":
    root = Path(__file__).resolve().parent.parent
    targets = sys.argv[1:] or sorted(root.glob("lab*/REPORT.md"))
    for t in targets:
        convert(Path(t))
