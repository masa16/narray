require 'test/unit'
#require "complex"
require "narray"
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

  def test_dfloat_op
    #GC.disable
    px "$c = Num::DFloat.new([2,2,2,30]).seq" # fill(0)
    px "$a = Num::DFloat.new([40,30]).seq"
    px "$a.inspect"
    #Num::NArray.inspect_cols=0
    #Num::NArray.inspect_rows=0
    px "$b = Num::DFloat.new(100).fill(0)"
    px "$d = Num::DFloat.new([4,3]).seq"
    px "$d.format_to_a('%+.3e')"
    px "$d.format_to_a"
    GC.disable
    #px "$d.text"
    px "$d.format('%.4f')"
    px "$d.format"
  end
end
