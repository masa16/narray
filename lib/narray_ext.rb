#  Numerical Array Extention for Ruby
#    (C) Copyright 2000-2008 by Masahiro TANAKA
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

  # delete rows/columns
  def delete_at(*args)
    if args.size > self.rank
      raise ArgumentError, "too many arguments"
    end
    shp = self.shape
    ind = []
    self.rank.times do |i|
      n = shp[i]
      case a=args[i]
      when Integer
        a = n+a if a<0
        raise IndexError, "index(%d) out of range"%[a] if a<0
        x = [0...a,a+1...n]
      when Range
        b = a.first
        b = n+b if b<0
        raise IndexError, "index(%s) out of range"%[a] if b<0
        e = a.last
        e = n+e if e<0
        e -= 1 if a.exclude_end?
        raise IndexError, "index(%s) out of range"%[a] if e<0
        x = [0...b,e+1...n]
      when Array
        x = (0...n).to_a
        x -= a.map do |j|
          raise IndexError, "contains non-integer" unless Integer===j
          (j<0) ? n+j : j
        end
      else
        if a
          raise ArgumentError, "invalid argument"
        else
          x = true
        end
      end
      ind << x
    end
    self[*ind]
  end

# Statistics
  def mean(*ranks)
    if integer?
      a = self.to_type(NArray::DFLOAT)
    else
      a = self
    end
    a = NArray.ref(a)
    a.sum(*ranks) / (rank_total(*ranks))
  end

  def stddev(*ranks)
    if integer?
      a = self.to_type(NArray::DFLOAT)
    else
      a = self
    end
    a = NArray.ref(a)
    n = rank_total(*ranks)
    if complex?
      NMath::sqrt( (( a-a.accum(*ranks).div!(n) ).abs**2).sum(*ranks)/(n-1) )
    else
      NMath::sqrt( (( a-a.accum(*ranks).div!(n) )**2).sum(*ranks)/(n-1) )
    end
  end

  def rms(*ranks)
    if integer?
      a = self.to_type(NArray::DFLOAT)
    else
      a = self
    end
    a = NArray.ref(a)
    n = rank_total(*ranks)
    if complex?
      NMath::sqrt( (a.abs**2).sum(*ranks)/n )
    else
      NMath::sqrt( (a**2).sum(*ranks)/n )
    end
  end

  def rmsdev(*ranks)
    if integer?
      a = self.to_type(NArray::DFLOAT)
    else
      a = self
    end
    a = NArray.ref(a)
    n = rank_total(*ranks)
    if complex?
      NMath::sqrt( (( a-a.accum(*ranks).div!(n) ).abs**2).sum(*ranks)/n )
    else
      NMath::sqrt( (( a-a.accum(*ranks).div!(n) )**2).sum(*ranks)/n )
    end
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
    when FLOAT
    when SFLOAT
    else
      raise TypeError, "NArray type must be (S)FLOAT or (S)COMPLEX."
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
      if idx.size>0
	rr[i] = r[idx]
	xx[i] = x[idx]
	i += idx.size
      end
    end
    # Box-Muller transform
    rr = ( xx * NMath::sqrt( -2 * NMath::log(rr) / rr ) )
    # finish
    rr.reshape!(*self.shape) if self.rank > 1
    rr = rr.to_type(self.typecode) if type!=self.typecode
    if RUBY_VERSION < "1.8.0"
      self.type.refer(rr)
    else
      self.class.refer(rr)
    end
  end
  #alias randomn! randomn

  def randomn!
    self[]= randomn
    self
  end

  #SFloatOne = NArray.sfloat(1).fill!(1)

  #
  # Cumulative sum along dimension +dim+; modifies this array in place.
  #
  # @param [Number] dim non-negative
  #
  # @return [NArray] self
  #
  def cumsum_general! dim=0
    if self.dim > dim
      if self.dim == 1
        # use the built-in version for dimension 1
        self.cumsum_1!
      else
        # for example, if this is a matrix and dim = 0, mask_0 selects the first
        # column of the matrix and mask_1 selects the second column; then we
        # just shuffle them along and accumulate.
        mask_0 = (0...self.dim).map{|d| d == dim ? 0 : true}
        mask_1 = (0...self.dim).map{|d| d == dim ? 1 : true}
        while mask_1[dim] < self.shape[dim]
          self[*mask_1] += self[*mask_0]
          mask_0[dim] += 1
          mask_1[dim] += 1
        end
      end
    end
    self
  end

  #
  # Cumulative sum along dimension +dim+.
  #
  # @param [Number] dim non-negative
  #
  # @return [NArray]
  #
  def cumsum_general dim=0
    self.dup.cumsum_general!(dim)
  end

  # The built-in cumsum only does vectors (dim 1).
  alias cumsum_1 cumsum
  alias cumsum cumsum_general
  alias cumsum_1! cumsum!
  alias cumsum! cumsum_general!


  #
  # Replicate this array to make a tiled array; this is the matlab function
  # repmat.
  #
  # @param [Array<Number>] reps number of times to repeat in each dimension;
  # note that reps.size is allowed to be different from self.dim, and dimensions
  # of size 1 will be added to compensate
  #
  # @return [NArray] with same typecode as self 
  #
  def tile *reps
    if self.dim == 0 || reps.member?(0)
      # Degenerate case: 0 dimensions or dimension 0
      res = NArray.new(self.typecode, 0)
    else
      if reps.size <= self.dim 
        # Repeat any extra dims once.
        reps = reps + [1]*(self.dim - reps.size) 
        tile = self
      else
        # Have to add some more dimensions (with implicit shape[dim] = 1).
        tile_shape = self.shape + [1]*(reps.size - self.dim) 
        tile = self.reshape(*tile_shape)
      end

      # Allocate tiled matrix.
      res_shape = (0...tile.dim).map{|i| tile.shape[i] * reps[i]}
      res = NArray.new(self.typecode, *res_shape)

      # Copy tiles.
      # This probably isn't the most efficient way of doing this; just doing
      # res[] = tile doesn't seem to work in general
      nested_for_zero_to(reps) do |tile_pos|
        tile_slice = (0...tile.dim).map{|i|
          (tile.shape[i] * tile_pos[i])...(tile.shape[i] * (tile_pos[i]+1))}
        res[*tile_slice] = tile
      end
    end
    res
  end

  private

  #
  # This is effectively <tt>suprema.size</tt> nested 'for' loops, in which the
  # outermost loop runs over <tt>0...suprema.first</tt>, and the innermost loop
  # runs over <tt>0...suprema.last</tt>.
  #
  # For example, when +suprema+ is [3], it yields [0], [1] and [2], and when
  # +suprema+ is [3,2] it yields [0,0], [0,1], [1,0], [1,1], [2,0] and [2,1].
  #
  # @param [Array<Integer>] suprema non-negative entries; does not yield if
  #        empty 
  #
  # @return [nil]
  # 
  def nested_for_zero_to suprema
    unless suprema.empty?
      nums = suprema.map{|n| (0...n).to_a}
      nums.first.product(*nums.drop(1)).each do |num|
        yield num
      end
    end
    nil
  end
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
    x = x.to_type(NArray::DFLOAT) if x.integer?
    y = NArray.to_na(y) unless y.kind_of?(NArray)
    y = y.to_type(NArray::DFLOAT) if y.integer?
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
