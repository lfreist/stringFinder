"""
Author: Leon Freist <freist@informatik.uni-freiburg.de>
"""

import os
import sys
import subprocess as sp
import matplotlib.pyplot as plt


def display_progress(done: int, of: int):
    if done % 100 != 0 or not PROGRESS:
        return
    progress = done / of
    bars = "|" * int(progress * 80)
    spaces = " " * (80 - len(bars))
    print(f"[{bars}{spaces}] - {round(progress * 100, 2)}%", end="\r")


class Benchmark:
    def __init__(self, esf_path: str, keyword: str, files_dir: str):
        self.esf_path = esf_path
        self.keyword = keyword
        self.files_dir = files_dir
        self.results = {}

    def subprocess(self, data_file: str, meta_file: str):
        process = sp.Popen([self.esf_path, self.keyword, data_file, f"--meta-file {meta_file}", "--performance"],
                           stdout=sp.PIPE)
        process.wait()
        return float(process.stdout.split(" ")[1])

    def run(self, block_size_filter: int = -1, compression_level_filter: int = -1):
        files = list(filter(lambda e: e.endswith(".zst"), os.listdir(self.files_dir)))
        files.sort()
        skipped = []
        counter = 0
        self.results = {}
        print(f"Benchmarking {len(files)} files...")
        for file in files:
            display_progress(counter, len(files))
            data_file = os.path.abspath(os.path.join(self.files_dir, file))
            meta_file = os.path.abspath(os.path.join(self.files_dir, ".".join(file.split(".")[:-1]) + ".meta"))
            try:
                compression_level = int(file.split("-")[1][1:])
                block_size = int(file.split("-")[2].split(".")[0][2:])
            except ValueError:
                compression_level = -1
                block_size = -1
                print("Make sure your file is named something like '<base_name>-l<compression>-bs<block-size>.esf.zst'")
            if 0 < block_size_filter != block_size:
                continue
            if 0 < compression_level_filter != compression_level:
                continue
            if not os.path.exists(meta_file):
                skipped.append(file)
            else:
                run_time = subprocess(esf_path, data_file, meta_file)
                self.results[file] = {
                    "compression_level": compression_level,
                    "block_size": block_size,
                    "run_time": run_time
                }
            counter += 1
        print()
        print(f"Done benchmarking files... Benchmarked {len(self.results)} files.")
        if skipped:
            print(f"Following files were skipped due to missing meta files: {skipped}")

    def plot(self):
        fig, ax = plt.subplots()
        ax.bar(self.results.keys(), [v["run_time"] for k, v in self.results.items()])
        ax.set_xlabel('Files')
        ax.set_ylabel(r'Wall time [s]')
        plt.plot()


if __name__ == "__main__":
    if sys.argc != 3 or sys.argc != 4:
        print("Usage: benchmark.py path/to/ESF path/to/files/ [keyword]")
        exit(1)
    esf = sys.argv[1]
    directory = sys.argv[2]
    keyword = sys.argv[3] if sys.argc == 4 else "keyword"
    bm = Benchmark(esf, keyword, directory)
    bm.run()
