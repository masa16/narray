from Numeric import *
import time

REPEAT = 100
ARRSZ = 1000000

def bench_array(type=Float64):
  return arrayrange(ARRSZ).astype(type)

def bench_time(func,repeat=REPEAT):
  start = time.clock()
  for i in range(repeat):
    func()
  stop = time.clock()
  print "  Time: %7.3f sec" % (stop-start)
