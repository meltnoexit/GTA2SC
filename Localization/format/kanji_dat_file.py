from dataclasses import dataclass
from pathlib import Path

from construct import (
    Array,
    Bytes,
    Const,
    GreedyRange,
    Int16ul,
    Int32ul,
    Prefixed,
    Struct,
    Terminated,
)


@dataclass
class KanjiDatFile:
    kidx: list[int]
    kbit: list[bytes]
    KANJI_DAT_FILE = Struct(
        "magic" / Const(b"KANJ"),
        "version" / Const(100, Int16ul),
        "kidx_tag" / Const(b"KIDX"),
        "kidx"
        / Prefixed(
            Int32ul,
            Array(65536, Int16ul),
        ),
        "kbit_tag" / Const(b"KBIT"),
        "kbit"
        / Prefixed(
            Int32ul,
            GreedyRange(Bytes(32)),
        ),
        Terminated,
    )

    @classmethod
    def load_from_file(cls, file_path: str) -> "KanjiDatFile":
        data = cls.KANJI_DAT_FILE.parse(Path(file_path).read_bytes())
        return cls(
            kidx=list(data.kidx),
            kbit=list(data.kbit),
        )

    def save_to_file(self, file_path: str) -> None:
        data = self.KANJI_DAT_FILE.build(
            {
                "kidx": self.kidx,
                "kbit": self.kbit,
            }
        )
        Path(file_path).write_bytes(data)
