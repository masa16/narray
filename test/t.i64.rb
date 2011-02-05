require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      a.debug_print
    end
    puts
    a
  end

  def test_int64
    #GC.disable
    #px "Num::Int64.step_unit"
    px "$b = Num::Int64.new(5).seq"
    px "$a = Num::Int64.cast([1,3,-7,13,-19])"
    px "$a.abs"
    px "-$a"
    px "~$a"
    px "$a & $b"
    px "$a ^ $b"

    px "$a.to_a"
    px "$a[2]"
    px "$a[2] = 123; $a"

    px "$x = 2**63-1"
    px "$a[2] = $x; $a"

    px "$c = Num::Int64.new(5).seq"
    px "$a * $c"


    px "Num::DFloat.cast($a)"

    px "$i = Num::Int32.new(5).seq"
    px "Num::Int64.cast($i)"

    px "Num::Int32.cast($a)"
  end

end
