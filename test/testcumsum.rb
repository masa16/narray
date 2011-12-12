require 'test/unit_tests'

class TestCumSum < Test::Unit::TestCase
  def test_narray_cumsum
    # Degenerate case: dimension 0.
    assert_equal NArray.float(0), NArray.float(0).cumsum
    assert_equal NArray.int(0),   NArray.int(0).cumsum

    # Single-element vector.
    v = NArray.float(1).fill!(42)
    assert_equal v, v.cumsum
    assert_equal v.typecode, v.cumsum.typecode
    v = NArray.int(1).fill!(42)
    assert_equal v, v.cumsum
    assert_equal v.typecode, v.cumsum.typecode

    # Vector.
    v = NArray.float(2).indgen!
    assert_narray_close NArray[   0.0,  1.0], v.cumsum
    v = NArray.float(3).indgen! + 1
    assert_narray_close NArray[   1.0,  3.0,  6.0], v.cumsum
    assert_equal v, v.cumsum(1) # dim 1 doesn't exist; cumsum has no effect

    # Matrix.
    m = NArray.float(3,2).indgen! + 1
    assert_narray_close NArray[[  1.0,  3.0,  6.0],
                               [  4.0,  9.0, 15.0]], m.cumsum
    assert_narray_close NArray[[  1.0,  2.0,  3.0],
                               [  5.0,  7.0,  9.0]], m.cumsum(1)
    assert_equal m, m.cumsum(2) # dim 2 doesn't exist; cumsum has no effect

    # Array with dim 3 with one extra dim.
    d3 = NArray.int(3,2,1).indgen! - 1
    assert_equal NArray[[[  -1,  -1,   0],
                         [   2,   5,   9]]], d3.cumsum(0)
    assert_equal NArray[[[  -1,   0,   1],
                         [   1,   3,   5]]], d3.cumsum(1)
    assert_equal NArray[[[  -1,   0,   1],
                         [   2,   3,   4]]], d3.cumsum(2)
    assert_equal NArray[[[  -1,   0,   1],
                         [   2,   3,   4]]], d3.cumsum(3) # dim 3 doesn't exist
  end
end

