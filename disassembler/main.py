import argparse

from rom import ROM
from disassembler import Disassembler


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=str)
    parser.add_argument("--instrumentation", action='append', default=[])
    parser.add_argument("--sym")
    parser.add_argument("--source")
    parser.add_argument("--output", type=str, required=True)
    args = parser.parse_args()

    rom = ROM(args.rom)
    disassembler = Disassembler(rom)
    disassembler.export(args.output)