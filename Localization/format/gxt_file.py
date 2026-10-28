import json
import logging
from dataclasses import dataclass
from pathlib import Path

from construct import (
    Bytes,
    Const,
    Container,
    CString,
    ExprAdapter,
    GreedyRange,
    IfThenElse,
    Int16ul,
    Int32ul,
    NullTerminated,
    OneOf,
    Prefixed,
    Struct,
    Terminated,
    this,
)

log = logging.getLogger(__name__)


@dataclass
class GxtFile:
    language: bytes
    tkey: list
    tdat: list[str]

    GXT_FILE = Struct(
        "magic" / Const(b"GBL"),
        "language" / OneOf(Bytes(1), [b"E", b"F", b"G", b"I", b"S", b"J"]),
        "version" / Const(100, Int16ul),
        "tkey_tag" / Const(b"TKEY"),
        "tkey"
        / Prefixed(
            Int32ul,
            GreedyRange(
                Struct(
                    "offset" / Int32ul,
                    "key" / Bytes(8),
                )
            ),
        ),
        "tdat_tag" / Const(b"TDAT"),
        "tdat"
        / Prefixed(
            Int32ul,
            IfThenElse(
                this.language == b"J",
                GreedyRange(
                    ExprAdapter(
                        NullTerminated(GreedyRange(Int16ul), term=b"\x00\x00"),
                        decoder=lambda words, _: "".join(
                            (
                                chr(word)
                                if i == 0 and 0x2100 <= word <= 0x21FF
                                else word.to_bytes(2, "big").lstrip(b"\x00").decode("gbk")
                            )
                            for i, word in enumerate(words)
                        ),
                        encoder=lambda text, _: [
                            (
                                ord(char)
                                if i == 0 and 0x2100 <= ord(char) <= 0x21FF
                                else int.from_bytes(char.encode("gbk"), "big")
                            )
                            for i, char in enumerate(text)
                        ],
                    )
                ),
                GreedyRange(CString("utf_16_le")),
            ),
        ),
        Terminated,
    )

    @classmethod
    def load_from_file(cls, file_path: str) -> "GxtFile":
        data = cls.GXT_FILE.parse(Path(file_path).read_bytes())
        return cls(
            language=data.language,
            tkey=list(data.tkey),
            tdat=list(data.tdat),
        )

    def save_to_file(self, file_path: str):
        data = self.GXT_FILE.build(
            {
                "language": self.language,
                "tkey": self.tkey,
                "tdat": self.tdat,
            }
        )
        Path(file_path).write_bytes(data)

    @classmethod
    def import_from_json(cls, file_path: str) -> "GxtFile":
        data = json.loads(Path(file_path).read_text(encoding="utf-8"))

        tkey = []
        tdat = []
        offset = 0

        for item in sorted(data["texts"], key=lambda item: item["key"]):
            key = item["key"].encode("ascii")
            text = item["text"]
            tkey.append(
                Container(
                    offset=offset,
                    key=key.ljust(8, b"\x00"),
                )
            )
            tdat.append(text)
            offset += len(CString("utf_16_le").build(text))

        return cls(
            language=data["language"].encode("ascii"),
            tkey=tkey,
            tdat=tdat,
        )

    def export_to_json(self, file_path: str) -> None:
        text_by_offset = dict(
            zip(
                sorted(entry.offset for entry in self.tkey),
                self.tdat,
                strict=True,
            )
        )

        data = {
            "language": self.language.decode("ascii"),
            "texts": [
                {
                    "key": entry.key.rstrip(b"\x00").decode("ascii"),
                    "text": text_by_offset[entry.offset],
                }
                for entry in self.tkey
            ],
        }

        Path(file_path).write_text(
            json.dumps(data, ensure_ascii=False, indent=2) + "\n",
            encoding="utf-8",
        )
