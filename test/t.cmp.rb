require 'test/unit'
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

  def test_dfloat_sum
    #GC.disable
    px "$a = Num::DFloat.new([3,5]); $a.seq; $a"
    px "$b = Num::DFloat.new(3); $b.seq; $b+=5"
    px "$a.gt $b['*,-']"
    px "$c = Num::DFloat.new(5); $c.seq; $c+=4"
    px "$a.gt $c['-,*']"
    px "$b['*,-'].gt $c['-,*']"
    px "$b['*,-'].eq $c['-,*']"
  end
end
