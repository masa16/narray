# FITS demo 
require 'fits'
require 'nimage'

f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )

NImage.show f.data #, 'M51'

nx = f.data.shape[0]
ny = f.data.shape[1]

x = NArray.int(nx/2).indgen!*2
y = NArray.int(ny/2).indgen!*2
NImage.show f.data[x,y] #, 'M51 (x0.5)'

x = NArray.float(nx*1.5).indgen!/1.5
y = NArray.float(ny*1.5).indgen!/1.5
NImage.show f.data[x,y] #, 'M51 (x1.5)'

x = NArray.float(nx).indgen!
y = NArray.float(ny).indgen!.newdim!(0)
i = y / (ny/2) - 1
i = (x-nx/2) * NMath.sqrt(i.abs) + nx/2 + y*nx
d = f.data.clone.fill!(0)
d[i] = f.data
NImage.show d #, 'M51 (x1.5)'

print "Hit return key..."
STDIN.getc
