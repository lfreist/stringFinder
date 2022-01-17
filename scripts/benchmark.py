"""
Benchmark grep vs ESF on compressed and uncompressed files.

Author: Leon Freist <freist@informatik.uni-freiburg.de>
"""

import argparse
import subprocess
import os
import pathlib
import statistics
import shutil

import matplotlib.pyplot as plt

from typing import List

from utilities import timed, progress_bar


class Benchmark:
    def __init__(self, benchmark_file: str, keyword: str = "keyword", *args, **kwargs):
        self.benchmark_file = benchmark_file
        self.keyword = keyword
        self.programs = {k: {"path": v, "measurements": []} for k, v in kwargs.items() if shutil.which(v)}
        for program_path in args:
            if shutil.which(program_path) is not None:
                program = os.path.basename(program_path)
                counter = 1
                while program in self.programs:
                    program = f"{program}_{counter}"
                    counter += 1
                self.programs[program] = {"path": program_path, "measurements": []}
        self.progress_counter = 0

    def benchmark(self, iterations: int = 1):
        print(f"Benchmarking {len(self.programs)} tools on {self.benchmark_file!r} using {iterations} iterations.")
        print(progress_bar(0, 100), end="\r")
        for it in range(iterations):
            for k, v in self.programs.items():
                v["measurements"].append(self._benchmark_compressed([v["path"], self.keyword]))
                self.progress_counter += 1
                print(progress_bar(self.progress_counter, len(self.programs.keys())*iterations), end="\r")
        print()

    def _get_zstdcat_command(self) -> List[str]:
        return ["zstdcat", self.benchmark_file]

    @timed
    def _benchmark_compressed(self, cmd: List[str]):
        zstd_proc = subprocess.Popen(self._get_zstdcat_command(), stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        grep_proc = subprocess.run(cmd, stdin=zstd_proc.stdout, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        zstd_proc.wait()

    def plot(self):
        mean = [statistics.mean(v["measurements"]) for _, v in self.programs.items()]
        stddev = [statistics.stdev(v["measurements"]) if len(v["measurements"]) > 1 else 0
                  for _, v in self.programs.items()]
        fig, ax = plt.subplots()
        ax.bar(self.programs.keys(), mean, yerr=stddev, align="center")
        ax.set_title(self._build_title())
        ax.set_xlabel('Programs')
        ax.set_ylabel(r'Wall time [s]')
        return plt

    def _build_title(self):
        return f"Benchmark {' vs '.join(self.programs.keys())}"


def parse_command_line_arguments():
    parser = argparse.ArgumentParser(prog="benchmark.py",
                                     description="Benchmark grep vs ESF on compressed and uncompressed files")
    parser.add_argument("input_file", metavar="INPUT_FILE", type=str, help="File on which benchmark is performed")
    parser.add_argument("keyword", metavar="KEYWORD", type=str, help="Keyword to be searched for")
    parser.add_argument("--compress", "-c", metavar="LEVEL", type=int, default=0,
                        help="Compress file and run benchmark on compressed file (Level 1-19)")
    parser.add_argument("--output", "-o", metavar="PNG", type=str, default="results",
                        help="Path to output file")
    parser.add_argument("--programs", metavar="PATH", type=str, nargs="+", help="Paths to executables")
    parser.add_argument("--iterations", metavar="INT", type=int, default=2, help="Set number of runs per program")
    return parser.parse_args()


@timed
def execute_searcher(shell_cmd: str):
    subprocess.Popen(shell_cmd, shell=True)


def run_compression(file: str, output: str = None, level: int = 3) -> str:
    """
    Create destination directory and build compression command as string in shell format
    :param file: file to be compressed
    :param output: ...
    :param level: compression level
    :return: shell command as string
    """
    assert 0 < level < 20
    assert not file.encode(".zst"), f"File {file} already compressed? If not, rename to another extension than 'zst'."
    if output is None:
        output = f"comp/{'.'.join(file.split('.')[:-1] + ['zst'])}"
    path = "/".join(output.split("/")[:-1])
    if not os.path.exists(path):
        pathlib.Path(path).mkdir(parents=True)
    print(f"Compressing {file} to {output}")
    p = subprocess.Popen(["zstd", f"-{level}", f"-o {output}"])
    p.wait()
    print("done.")
    return output


if __name__ == "__main__":
    cargs = parse_command_line_arguments()
    if cargs.compress > 0:
        file = run_compression(cargs.input_file, level=cargs.compress)
    else:
        file = cargs.input_file
    b = Benchmark(file, cargs.keyword, *cargs.programs)
    b.benchmark(20)
    plt = b.plot()
    path = "/".join(cargs.output.split("/")[:-1])
    output = ".".join(cargs.output.split(".")[:-1]) + ".png"
    counter = 1
    while os.path.exists(output):
        output = f"{output.split('.')[:-1]}_{counter}.png"
        counter += 1
    plt.savefig(output)
    print(f"Saved figure at {output}")
