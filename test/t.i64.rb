require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
      p a
    end
    puts
    a
  end

  def test_int64
    #GC.disable
    #px "NArray::Int64.step_unit"
    px "$b = NArray::Int64.new([5]).seq"
    px "$a = NArray::Int64.cast([1,3,-7,13,-19])"
    px "$a.abs"
    px "-$a"
    #px "~$a"
    px "$a & $b"
    px "$a ^ $b"

    px "$a.to_a"
    px "$a[2]"
    px "$a[2] = 123; $a"

    px "$x = 2**63-1"
    px "$a[2] = $x; $a"

    px "$c = NArray::Int64.new([5]).seq"
    px "$a * $c"


    px "NArray::DFloat.cast($a)"

    px "$i = NArray::Int32.new([5]).seq"
    px "NArray::Int64.cast($i)"

    px "NArray::Int32.cast($a)"
  end

end
