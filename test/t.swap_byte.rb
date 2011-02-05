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
      a.debug_print
    end
    puts
    a
  end

  def test_swap_byte
    #GC.disable
    px "$a = Num::DFloat.new([5]).seq"
    px "$a.host_order?"
    px "$a.byte_swapped?"
    px "$a.little_endian?"
    px "$a.big_endian?"
    px "$a.network_order?"
    px "$a.vacs_order?"
    px "$a.to_host"

    px "$s = $a.swap_byte"
    px "$s.host_order?"
    px "$s.byte_swapped?"
    px "$s.little_endian?"
    px "$s.big_endian?"
    px "$s.network_order?"
    px "$s.vacs_order?"
    px "$s.to_network"
    px "$s.to_vacs"
    px "$s.to_host"
    px "$s.inplace.to_host"
    px "$s"
    return

    px "$t = $s.swap_byte"

    px "$s[0..1]"

    px "$s * 2 + 1"

    px "$b = Num::Int32.new(5).seq"

    px "$b.swap_byte"
    px "$c = Num::Int16.new(5).seq"
    px "$c.swap_byte"
  end
end
