#  Numerical Array Extention for Ruby
#    (C) Copyright 2000-2003 by Masahiro TANAKA
#
#  This program is free software.
#  You can distribute/modify this program
#  under the same terms as Ruby itself.
#  NO WARRANTY.
#
class NArray

  def integer?
    self.typecode==NArray::BYTE ||
    self.typecode==NArray::SINT ||
    self.typecode==NArray::LINT
  end
  def complex?
    self.typecode==NArray::DCOMPLEX ||
    self.typecode==NArray::SCOMPLEX
  end

  def all?
    where.size == size
  end

  def any?
    where.size > 0
  end

  def none?
    where.size == 0
  end

  def ==(other)
    if other.kind_of?(NArray)
      (shape == other.shape) && eq(other).all?
    else
      false
    end
  end

  def rank_total(*ranks)
    if ranks.size>0
      idx = []
      ranks.each{|i| idx.push(*i)}
      # ranks is expected to be, e.g., [1, 3..5, 7]
      a = self.shape
      n = 1
      idx.each{|i| n *= a[i]}
      n
    else
      self.total
    end
  end

# Statistics
  def mean(*ranks)
    if integer?
      a = self.to_f
    else
      a = self
    end
    a.sum(*ranks) / (rank_total(*ranks))
  end

  def stddev(*ranks)
    if integer?
      a = self.to_f
    else
      a = self
    end
    n = rank_total(*ranks)
    NMath::sqrt( (( a-a.accum(*ranks).div!(n) )**2).sum(*ranks)/(n-1) )
  end

  def median(rank=nil)
    shape = self.shape
    rank = shape.size-1 if rank==nil
    s = sort(rank).reshape!(true,*shape[rank+1..-1])
    n = s.shape[0]
    if n%2==1
      s[n/2,false]
    else
      s[n/2-1..n/2,false].sum(0)/2
    end
  end


# Normal distributed random number;  valid for floating point types
  def randomn
    size = self.size
    case type = self.typecode
    when COMPLEX; type=FLOAT
    when SCOMPLEX; type=SFLOAT
    end
    rr = NArray.new(type,size)
    xx = NArray.new(type,size)
    i = 0
    while i < size
      n = size-i
      m = ((n+Math::sqrt(n))*1.27).to_i
      x = NArray.new(type,m).random!(1) * 2 - 1
      y = NArray.new(type,m).random!(1) * 2 - 1
      r = x**2 + y**2
      idx = (r<1).where
      idx = idx[0...n] if idx.size > n
      rr[i] = r[idx]
      xx[i] = x[idx]
      i += idx.size
    end
    # Box-Muller transform
    rr = ( xx * NMath::sqrt( -2 * NMath::log(rr) / rr ) )
    # finish
    rr.reshape!(*self.shape) if self.rank > 1
    rr = rr.to_type(self.typecode) if type!=self.typecode
    self.type.refer(rr)
  end
  alias randomn! randomn

  #SFloatOne = NArray.sfloat(1).fill!(1)
end


module NMath
  PI = Math::PI
  E = Math::E

  def recip x
    1/x.to_f
  end

# Trigonometric function
  def csc x
    1/sin(x)
  end
  def csch x
    1/sinh(x)
  end
  def acsc x
    asin(1/x.to_f)
  end
  def acsch x
    asinh(1/x.to_f)
  end

  def sec x
    1/cos(x)
  end
  def sech x
    1/cosh(x)
  end
  def asec x
    acos(1/x.to_f)
  end
  def asech x
    acosh(1/x.to_f)
  end

  def cot x
    1/tan(x)
  end
  def coth x
    1/atanh(x)
  end
  def acot x
    atan(1/x.to_f)
  end
  def acoth x
    atanh(1/x.to_f)
  end

# Statistics
  def covariance(x,y,*ranks)
    x = NArray.to_na(x) unless x.kind_of?(NArray)
    x = x.to_f if x.integer?
    y = NArray.to_na(y) unless y.kind_of?(NArray)
    y = y.to_f if y.integer?
    n = x.rank_total(*ranks)
    xm = x.accum(*ranks).div!(n)
    ym = y.accum(*ranks).div!(n)
    ((x-xm)*(y-ym)).sum(*ranks) / (n-1)
  end

  module_function :csc,:sec,:cot,:csch,:sech,:coth
  module_function :acsc,:asec,:acot,:acsch,:asech,:acoth
  module_function :covariance
end


module FFTW
  def convol(a1,a2)
    n1x,n1y = a1.shape
    n2x,n2y = a2.shape
    raise "arrays must have same shape" if n1x!=n2x || n1y!=n2y 
    (FFTW.fftw( FFTW.fftw(a1,-1) * FFTW.fftw(a2,-1), 1).real) / (n1x*n1y)
  end
  module_function :convol
end

require 'nmatrix'
