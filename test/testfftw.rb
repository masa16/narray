require 'narray'

x = NArray.complex(1024,1024).indgen!

p x
puts 'executing fftw ...'
t1 = Time.times.utime
y = FFTW.fftw( x, 1 )
t2 = Time.times.utime
print "time: ",t2 - t1,"\n"
p y

exit

x = NArray.complex(128,128).indgen!
10000.times{ x = FFTW.fftw( x, 1 ) }
