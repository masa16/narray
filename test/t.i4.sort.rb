require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      #a.debug_print
    end
    puts
    a
  end

  def randgen(shape)
    x = Num::Int32.new(shape).rand(50)
    x.floor
  end

  def test_dfloat_op
    #GC.disable
    px "$a = Num::Int32.cast([[5,4,2,1,3],[-1,-2,-3,-4,-5]])"
    px "$a.sort"
    px "$a"
    px "$a = Num::Int32.cast([[5,4,2,1,3],[-1,-2,-3,-4,-5]])"
    px "$a.sort(-1)"

    px "$a = Num::Int32.cast([[5,4,2,1,3],[-1,-2,-3,-4,-5]])"
    px "$a.sort(0)"
    #px "$a = Num::Int32.new([2,4,5]).rand"
    px "$a = randgen([2,4,5])"
    px "$a.sort(-1)"
    px "$a[',,+'].sort"
    px "$a[',+,+'].sort"
    px "$a.sort(-2..-1)"
    px "$a"
    px "$a = randgen([2,3,2])"
    px "$a.sort(0,1)"
    px "$a.sort!(0,2)"
    px "$x = Num::Int8.new([3,4,5]).rand(50)"
    px "$x.sort(0,2)"

    px "$a = randgen([2,3,2])"
    px "$idx = $a.sort_index"
    px "$a[$idx.flatten.to_a]"
    px "$idx = $a.copy['+,+,'].sort_index"
    px "$a[$idx.flatten.to_a]"
    px "$a.median"
    px "$a['+,+,'].median"
    px "$a.median(-1)"

    px "$a = Num::Int8.new([2,3,2]).rand(50)"
    px "$idx = $a.sort_index"
    px "$a[$idx.flatten.to_a]"
    px "$idx = $a['+,+,'].sort_index"
    px "$a[$idx.flatten.to_a]"

  end
end
