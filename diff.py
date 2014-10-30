import sys

PRINT_OUTPUT = True
WRITE_OUTPUT = True
VERBOSE_PRINT = False

if '--nooutfile' in sys.argv:
	WRITE_OUTPUT = False
	sys.argv.remove('--nooutfile')

if '--noprintout' in sys.argv:
	PRINT_OUTPUT = False
	sys.argv.remove('--noprintout')

if '-v' in sys.argv:
	VERBOSE_PRINT = True
	sys.argv.remove('-v')
elif '--verbose' in sys.argv:
	VERBOSE_PRINT = True
	sys.argv.remove('--verbose')
if len(sys.argv) > 1 and len(sys.argv[1]) > 0 and not sys.argv[1].startswith('-'):
	test_name = sys.argv[1]
else:
	try:
		input_func = raw_input #python2
	except NameError as e:
		input_func = input #python3
	finally:
		test_name = input_func("Test name: ")

expected_file = "test/expected/{0}-expected.txt".format(test_name)
output_file = "test/output/{0}-output.txt".format(test_name)

expected  = []
output = []

with open(expected_file) as f:
	for l in f:
		expected.append(l.split())
with open(output_file) as f:
	for l in f:
		output.append(l.split())

MAX_Y = len(output)
MAX_X = len(output[0])

buff = ""
vline = "+" + "-"*MAX_X + "+\n"
buff += vline
print("Dimensions are: {0}x{1}".format(MAX_X, MAX_Y))
for j in range(MAX_Y):
	buff += "|"
	for i in range(MAX_X):
		if expected[j][i] != output[j][i]:
			if VERBOSE_PRINT:
				print("Mismatch ({0}, {1}): Expected {2}, Output {3}".format(i, j, expected[j][i], output[j][i]))
			buff += "X"
		elif expected[j][i] == "00":
			buff += " "
		else:
			buff += "."
	buff += "|\n"
buff += vline

if WRITE_OUTPUT:
	with open("{0}-diffs.txt".format(test_name), "w") as outf:
		outf.write(buff)

if PRINT_OUTPUT:
	print(buff)
