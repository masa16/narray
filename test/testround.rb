require 'narray'

def testround a
  print "a = ";  p a
  print "a.floor = ";  p a.floor
  print "a.ceil  = ";  p a.ceil
  print "a.round = ";  p a.round
  print "\n"
end

testround NArray.float(4,2).indgen!/4-2

testround NArray.complex(5,1).indgen!-2 +
  NArray.complex(1,2).indgen * 1.im - 0.5.im
