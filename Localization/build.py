import json
from pathlib import Path

from format.gxt_file import GxtFile
from format.kanji_dat_file import KanjiDatFile

root = Path(__file__).resolve().parent
build_dir = root / "build"
font_dir = root / "font"
translation_dir = root / "translation"

for path in sorted(Path("translation").glob("*.json")):
    with path.open("r", encoding="utf-8") as f:
        data = json.load(f)

    for item in data["texts"]:
        text = item["text"]

        if text and 0x2100 <= ord(text[0]) <= 0x21FF:
            text = text[1:]

        try:
            text.encode("gbk")
        except UnicodeEncodeError as e:
            char = text[e.start]
            raise ValueError(
                f"{path}: {item['key']!r} 包含 GBK 不支持的字符：{char!r} (U+{ord(char):04X})"
            ) from e

with (translation_dir / "j.json").open("r", encoding="utf-8") as f:
    data = json.load(f)

chars = set()

for item in data["texts"]:
    text = item["text"]

    if text and 0x2100 <= ord(text[0]) <= 0x21FF:
        text = text[1:]

    chars.update(char for char in text if ord(char) >= 0x20)

font_files = list(font_dir.glob("unifont-*.hex"))

if not font_files:
    raise FileNotFoundError(f"未找到 Unifont 字体文件：{font_dir}")


glyphs = {}

with font_files[0].open("r", encoding="ascii") as f:
    for line_number, line in enumerate(f, 1):
        line = line.strip()

        if not line:
            continue

        try:
            codepoint, bitmap_hex = line.split(":", 1)
            glyphs[int(codepoint, 16)] = bytes.fromhex(bitmap_hex)
        except ValueError as e:
            raise ValueError(f"字体文件第 {line_number} 行遇到错误") from e


kidx = [0] * 65536
kbit = [bytes(32)]

for char in sorted(chars, key=ord):
    codepoint = ord(char)

    if codepoint not in glyphs:
        raise ValueError(f"字体缺少字符：{char!r} U+{codepoint:04X}")

    bitmap = glyphs[codepoint]

    if len(bitmap) == 16:
        bitmap = b"".join((row << 4).to_bytes(2, "big") for row in bitmap)
    elif len(bitmap) != 32:
        raise ValueError(f"字符 {char!r} 的点阵数据大小异常：{len(bitmap)} 字节")

    try:
        code = int.from_bytes(char.encode("gbk"), "big")
    except UnicodeEncodeError as e:
        raise ValueError(f"GBK 不支持字符：{char!r} U+{codepoint:04X}") from e

    if code < 0x100:
        code <<= 8

    if kidx[code] != 0:
        raise ValueError(f"字形索引冲突：{char!r}，索引 0x{code:04X}")

    kidx[code] = len(kbit)
    kbit.append(bitmap)

build_dir.mkdir(parents=True, exist_ok=True)
KanjiDatFile(kidx=kidx, kbit=kbit).save_to_file(build_dir / "kanji.dat")
GxtFile.import_from_json(translation_dir / "bob_j.json").save_to_file(
    build_dir / "bob_j.gxt"
)
GxtFile.import_from_json(translation_dir / "j.json").save_to_file(build_dir / "j.gxt")
