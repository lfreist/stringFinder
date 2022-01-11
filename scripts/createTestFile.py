"""
Create test files "sf-test-xgb.txt" of given sizes (xgb) for StringFinder
Author Leon Freist <freist@informatik.uni-freiburg.de>
"""

import os
import argparse
from random import randint


PROGRESS = False
WORDS = []
FILE_SIZE = 0
OUTPUT_FILE = ""


def parse_command_line_arguments():
	parser = argparse.ArgumentParser(prog="createTestFile.py", description="Create a test file of a given size for StringFinder.")
	parser.add_argument("words_file", metavar="WORDS_FILE", type=str, help="File which holds possible words")
	parser.add_argument("--size", "-s", metavar="GiB", type=int, default="5", help="Size of the test file in GiB")
	parser.add_argument("--output", "-o", metavar="OUTPUT_FILE", type=str, default="sf-test.txt", help="Path to output file")
	parser.add_argument("--progress", "-p", default=0, action="count", help="Display progress")
	return parser.parse_args()


def display_progress(done: int, of: int):
	if done % 100 != 0 or not PROGRESS:
		return
	progress = done/of
	bars = "|" * int(progress*80)
	spaces = " " * (80-len(bars))
	print(f"[{bars}{spaces}] - {round(progress*100, 2)}%", end="\r")


def create_test_file():
	bytes_wrote = 0
	with open(OUTPUT_FILE, "a") as f:
		while (bytes_wrote < FILE_SIZE):
			display_progress(bytes_wrote, FILE_SIZE)
			min_line_length = randint(5, 50) * 10
			line = []
			while (min_line_length > 0):
				word = WORDS[randint(0, len(WORDS)-1)]
				line.append(word)
				min_line_length -= len(word)
			line_str = " ".join(line)
			print(line_str, file=f)
			bytes_wrote += len(line_str)
	print()
	print(f"Wrote {bytes_wrote} in total.")


if __name__ == "__main__":
	args = parse_command_line_arguments()
	PROGRESS = bool(args.progress)
	FILE_SIZE = args.size * 1000000000
	OUTPUT_FILE = args.output
	with open(args.words_file, "r") as f:
		WORDS = f.readlines()
		WORDS = list(map(lambda word: word.strip(), WORDS))
	if os.path.exists(OUTPUT_FILE):
		print(f"WARNING: File {OUTPUT_FILE} already exists.")
		tmp = OUTPUT_FILE.split(".")
		if len(tmp) == 1:
			OUTPUT_FILE += "-n.txt"
		else:
			try:
				n = int(tmp[-2][-1]) + 1
				tmp[-2] = tmp[-2][:-1] + str(n)
			except ValueError:
				tmp[-2] += "-1"
			OUTPUT_FILE = ".".join(tmp)
		print(f"Writinng output to {OUTPUT_FILE}")
	create_test_file()
