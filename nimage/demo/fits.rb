# A simple class for handling FITS images
#	by Masahiro TANAKA 1999/12/19
#    No document, sorry.
#
# Require:  Ruby/NArray
#    http://koala.astro.isas.ac.jp/~masa/ruby/
#
# Example:
#    f = Fits.read('somthing.fits')
#    f.data # => 2-dimentional NArray data

require 'narray'

class Fits
  attr_accessor :data
  attr_accessor :header

  def initialize
    @data   = ""
    @header = []
  end

  def data_to_narray(data,type,shape)
    NArray.to_na( data, type, *shape ).ntoh
  end


  def read(fname)
    f = File.open(fname, "r")
    @header = []
    nbytes = 0
    foo = {}

    while 
      line = f.read(80)
      nbytes += 80;

      @header.push line

      name = line[0,8].strip!
      break if name=="END"

      rest = line[8..-1]
      if rest =~ /^= +('[^']*')/ #'
	foo[name] = $1
      elsif rest =~ /^= +([^\s\/]+)/
	foo[name] = $1
      end
    end

    # skip extra space
    nbytes %= 2880
    f.read(2880-nbytes) if nbytes > 0

    naxis = foo["NAXIS"].to_i

    case foo["BITPIX"]
    when "8";	elm_size = 1; type = NArray::BYTE
    when "16";	elm_size = 2; type = NArray::SINT
    when "32";	elm_size = 4; type = NArray::LINT
    when "-32";	elm_size = 4; type = NArray::SFLOAT
    when "-64";	elm_size = 8; type = NArray::DFLOAT
    end

    size = 1
    dims = []
    for i in 1..naxis
      key = "NAXIS#{i}"
      if foo.key? key
	n = foo[key].to_i
	size = size * n
	dims.push n
	#printf "%s=%i\n", key,n
      end
    end

    @data = data_to_narray( f.read(size*elm_size), type, dims )
    f.close

    if foo.key? "BSCALE"
      s = foo["BSCALE"].to_f
      @data = @data.to_f.mul!(s) if s != 1
    end

    if foo.key? "BZERO"
      s = foo["BZERO"].to_f
      @data = @data.to_f.add!(s) if s != 0
    end

    self
  end

end

def Fits.read(*a)
  Fits.new.read(*a)
end
