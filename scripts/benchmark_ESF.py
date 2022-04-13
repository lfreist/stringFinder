"""
Author: Leon Freist <freist@informatik.uni-freiburg.de>
"""

import os
import sys
import argparse
import multiprocessing
import subprocess as sp
import matplotlib.pyplot as plt


def display_progress(done: int, of: int):
    progress = done / of
    bars = "|" * int(progress * 80)
    spaces = " " * (80 - len(bars))
    print(f"[{bars}{spaces}] - {round(progress * 100, 2)}%", end="\r")


class Benchmark:
    def __init__(self, esf_path: str, keyword: str, src: str, meta_file: str = None):
        self.esf_path = os.path.abspath(esf_path)
        self.keyword = keyword
        self.src = os.path.abspath(src)
        self.meta_file = meta_file

    def subprocess(self, data_file: str, meta_file: str = None, n_proc: int = 1):
        if meta_file:
            cmd = f"{self.esf_path} {self.keyword} {data_file} -m {meta_file} -p -j {n_proc} -b 30"
        else:
            cmd = f"{self.esf_path} {self.keyword} {data_file} -p -j {n_proc} -b 30"
        proc = sp.Popen(cmd, shell=True, stdout=sp.PIPE)
        proc.wait()
        output = proc.stdout.read().decode()
        return float(output.split(" ")[1])

    def bm_proc(self, data_file: str, meta_file: str = None):
        return {pc: self.subprocess(data_file, meta_file, pc) for pc in range(1, multiprocessing.cpu_count())}

    def run(self, bm_procs: bool = False):
        result = {}
        if os.path.isfile(self.src):
            try:
                compression_level = int(self.src.split("-")[1][1:])
                block_size = int(self.src.split("-")[2].split(".")[0][2:])
            except ValueError:
                compression_level = -1
                block_size = -1
            print(f"Running benchmark on {self.src}...")
            if bm_procs:
                result[self.src] = {
                    "compression_level": compression_level,
                    "block_size": block_size,
                    "run_time": self.bm_proc(self.src, self.meta_file)
                }
            else:
                result[self.src] = {
                    "compression_level": compression_level,
                    "block_size": block_size,
                    "run_time": self.subprocess(self.src, self.meta_file)
                }
        else:
            files = list(filter(lambda e: e.endswith(".zst"), os.listdir(self.src)))
            files.sort()
            counter = 0
            print(f"Benchmarking {len(files)} files...")
            for file in files:
                display_progress(counter, len(files))
                data_file = os.path.join(self.src, file)
                meta_file = os.path.join(self.src, ".".join(file.split(".")[:-1]) + ".meta")
                try:
                    compression_level = int(file.split("-")[1][1:])
                    block_size = int(file.split("-")[2].split(".")[0][2:])
                except ValueError:
                    compression_level = -1
                    block_size = -1
                    print("Make sure your file is named something like '<base_name>-l<compression>-bs<block-size>.esf.zst'")
                if not os.path.exists(meta_file):
                    skipped.append(file)
                else:
                    if bm_procs:
                        result[file] = {
                            "compression_level": compression_level,
                            "block_size": block_size,
                            "run_time": self.bm_proc(data_file, meta_file)
                        }
                    else:
                        result[file] = {
                            "compression_level": compression_level,
                            "block_size": block_size,
                            "run_time": self.subprocess(data_file, meta_file)
                        }
                counter += 1
            display_progress(counter, len(files))
            print()
            print(f"Done benchmarking files... Benchmarked {len(result)} files.")
        return self.plot(result)

    @staticmethod
    def plot(result):
        fig, ax = plt.subplots()
        keys = []
        proc = False
        tmp = result.copy()
        result = {}
        for k, v in tmp.items():
            if type(v["run_time"]) == float:
                keys.append(f"{v['compression_level']}|{v['block_size']}")
                result[keys[-1]] = v
            else:
                result = v["run_time"]
                keys = v["run_time"].keys()
                proc = True
                break
        if not proc:
            ax.bar(keys, [result[k]["run_time"] for k in keys])
            ax.set_xlabel('Files')
            ax.set_ylabel(r'Wall time [s]')
        else:
            ax.bar(keys, [result[k] for k in keys])
            ax.set_xlabel('Processors')
            ax.set_ylabel(r'Wall time [s]')
        return plt


def parse_command_line_arguments():
    parser = argparse.ArgumentParser(prog="benchmark.py", description="Benchmark ESF")
    parser.add_argument("esf", metavar="ESF_PATH", type=str, help="Path to executable ESF file")
    parser.add_argument("file", metavar="FILE", type=str, help="Path to ESF file or folder containing files")
    parser.add_argument("--keyword", "-k", metavar="KEYWORD", type=str, help="Keyword to be searched for", default="keyword")
    parser.add_argument("--output", "-o", metavar="PNG", type=str, help="Path to output file", default="benchmark.png")
    parser.add_argument("--meta", metavar="META_FILE", type=str, help="Path to ESF meta file")
    parser.add_argument("--benchmark_procs", action="store_true", help="Benchmark different number of threads")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_command_line_arguments()
    esf = args.esf
    keyword = args.keyword
    output = args.output
    src = os.path.abspath(args.file)
    meta = os.path.abspath(args.meta) if os.path.isfile(src) else ""
    bm_proc = args.benchmark_procs

    bm = Benchmark(esf, keyword, src, meta)
    plt = bm.run(bm_proc)
    plt.savefig(output)
