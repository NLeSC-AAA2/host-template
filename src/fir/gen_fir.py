#!/usr/bin/env python

import argparse
from string import Template

def host(filter_weights, args):
    def render_channel(c):
        vals = ", ".join(str(filter_weights[c, t]) for t in range(args.taps))
        return "{ " + vals + " }"

    print Template("""\
namespace fir
{
constexpr int NR_TAPS = $taps;
constexpr int NR_CHANNELS = $channels;

const cl_short filterWeights[NR_CHANNELS][NR_TAPS] =
{""").safe_substitute(taps=args.taps, channels=args.channels),

    print "\n, ".join(render_channel(c) for c in range(args.channels))
    print "};\n}"

def fpga(filter_weights, args):
    print Template("""\
constant int NR_TAPS = $taps;
constant int NR_CHANNELS = $channels;
constant int VECTOR_SIZE = $vector_size;
constant short filterWeights[] =
{""").safe_substitute(taps=args.taps, channels=args.channels, vector_size=args.vector_size),

    if args.vector_size:
        for c in range(args.channels / args.vector_size):
            for t in range(args.taps):
                for i in range(args.vector_size):
                    print str(filter_weights[(c * args.vector_size) + i, t]) + ", ",
                print
            print
    else:
        for c in range(args.channels):
            for t in range(args.taps):
                print str(filter_weights[c, t]) + ",",
            print

    print "};"

if __name__ != "__main__":
    exit(1)

def add_common_args(parser):
    parser.add_argument('--taps', metavar='N', type=int, action='store',
                        dest='taps', help='Number of taps used per FIR filter.')

    parser.add_argument('--channels', metavar='N', type=int, action='store',
                        dest='channels', help='Number of channels.')

    parser.add_argument('file', metavar='PATH', type=str, nargs=1,
                    help='File with FIR filter coefficients.')

parser = argparse.ArgumentParser(
    description='Generate a binary decision tree from input training data.',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)


subparsers = parser.add_subparsers(help='sub-command help')
host_parser = subparsers.add_parser('host',
        help='generate FIR filter weights for host')
add_common_args(host_parser)
host_parser.set_defaults(handler=host)


fpga_parser = subparsers.add_parser('fpga',
        help='generate FIR filter weights for fpga')

fpga_parser.add_argument('--vector-size', metavar='N', type=int, action='store',
                    dest='vector_size', help='')

add_common_args(fpga_parser)
fpga_parser.set_defaults(handler=fpga)

args = parser.parse_args()
filter_weights = dict()

with open(args.file[0], 'r') as input_file:
    for t in range(args.taps):
        for c in range(args.channels):

            n = input_file.readline()
            if n == '':
                raise Exception("Found end of file before reading in all filter weights!")
            else:
                n = int(n)

            filter_weights[c,t] = n

args.handler(filter_weights, args)
