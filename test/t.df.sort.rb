require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

#NArray.debug=true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
      p a
      #a.debug_print
    end
    puts
    a
  end

  def randgen(shape)
    x = NArray::DFloat.new(shape).rand*50
    x.floor
  end

  def test_dfloat_op
    #GC.disable
    px "$a = NArray::DFloat.cast([5,4,2,1,3,-1,-2,-3,-4,-5])"
    px "$a.sort"
    px "$a"
    px "$a.inplace"
    px "$a.inplace?"
    px "$a.sort"
    px "$a"
    px "$a.inplace?"
    #return
    px "$a = NArray::DFloat.cast([[5,4,2,1,3],[-1,-2,-3,-4,-5]])"
    px "$a.sort(-1)"

    #px "$a = NArray::DFloat.cast([[5,4,2,1,3],[-1,-2,-3,-4,-5]])"
    px "$a.sort(0)"

    #px "$a = NArray::DFloat.new([2,4,5]).rand"
    px "$a = randgen([2,4,5])"

    px "$a.sort(-1)"
    #px "$a[',,+'].sort"
    #px "$a['*,+,+'].sort"
    #px "$a.sort(-2..-1)"
    px "$a = randgen([2,3,2])"
    px "$a.sort"
    px "$a.median"
    #px "$a['+,+,'].median"
    px "$a.median(-1)"

    px "$a = randgen([2,3,2])"
    px "$idx = $a.sort_index"
    px "$a[$idx.flatten.to_a]"
    px "$idx = $a.sort_index(1)"
    px "$a[$idx.flatten.to_a]"
    px "$idx = $a['+,+,'].sort_index"
    px "$a[$idx.flatten.to_a]"
  end
end
