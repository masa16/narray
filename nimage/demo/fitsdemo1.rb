# FITS demo 
require 'fits'
require 'nimage'

f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )
p f.data

v = NImage.show f.data

print "Hit return key..."
STDIN.getc
v.close
