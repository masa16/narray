require 'narray'
require 'irb/xmp'
# xmp :: http://www.ruby-lang.org/en/raa-list.rhtml?name=xmp

m1 = NMatrix.float(2,2).indgen!
m2 = NMatrix[[0,1.2],[1.5,0]]

v1 = NVector[0.5,1.5]
v2 = NVector.float(2,2).indgen!

a  = NArray.float(2,2).indgen!

xmp 'm1'
xmp 'm1.inverse'
xmp 'm2'
xmp 'm1*m2'
xmp 'm2*m1'
xmp 'm1+m2'
xmp '3.14*m1'
xmp 'm2*1.25'
xmp 'v1'
xmp 'v2'
xmp '1.25*v1'
xmp 'NMath.sqrt(v2**2)'
xmp 'v1*v2'
xmp 'm1*v1'
xmp 'v2*m2'
xmp 'm1.diagonal([98,99])'
xmp 'NMatrix.float(4,3).unit'

puts "\n=== following will fail ...\n"
xmp 'm1+v1'
xmp 'm1+1'
