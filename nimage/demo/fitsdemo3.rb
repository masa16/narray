# FITS demo 
require 'fits'
require 'nimage'

f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )
image = f.data

NImage.show image

nx = image.shape[0]
ny = image.shape[1]

# re-sampling
x = NArray.int(nx/2).indgen!*2
y = NArray.int(ny/2).indgen!*2
NImage.show image[x,y]
# 'M51 (x0.5, resample)'

# averaging
NImage.show image[0...nx/2*2,0...ny/2*2].to_f.
  reshape!(2,nx/2,2,ny/2).sum(0,2).div!(4).reshape!
# 'M51 (x0.5, average)'

print "Hit return key..."
STDIN.getc
