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

  def test_uint
    #GC.disable
    #px "Num::UInt32.step_unit"
    #px "$b = Num::UInt32.new(5).seq"
    px "$a = Num::UInt32.cast([2**32-1, 2**32-2])"
    px "$a[0]"
    px "$b = Num::DFloat.cast([2.0**32-1, 2.0**32-2])"
    px "$c = Num::UInt32.cast($b)"

    px "$d = Num::Int32.cast([1,-3,7,-13,19])"
    px "$e = Num::UInt32.cast($d)"

    px "$g = Num::UInt16.cast([0,2**16-1])"
    px "$h = Num::Int16.cast($g)"

    px "$f = Num::Int16.cast([0,2**16-1])"
    px "$i = Num::UInt16.cast($f)"
    px "$i[1]"

    px "-Num::UInt32.cast([1,3,7,13,19])"

    px "Num::UInt32.cast([1,0])-1"

    px "Num::UInt32.cast([1,3]).lt(Num::UInt32.cast([1,0])-1)"

    px "$j = Num::UInt32.cast([0,3,7,13,19])"
    px "$j ** Num::Int32.cast([0,0,3,9,2])"
    px "$j ** [0,0,3,9,2]"

    px "$z = Num::UInt32.cast([2.0**32-1, 2.0**32-2])"
  end


end
