require 'test/unit_tests'

class TestTile < Test::Unit::TestCase
  def test_narray_tile
    # Degenerate case: tile on a dimension 0 array.
    assert_equal NArray.float(0), NArray.float(0).tile
    assert_equal NArray.float(0), NArray.float(0).tile(0)
    assert_equal NArray.float(0), NArray.float(0).tile(0,0)
    assert_equal NArray.float(0), NArray.float(0).tile(1,1)

    # Degenerate case: tile 0 times on some dimension.
    assert_equal NArray.float(0), NArray.float(1).tile(0)
    assert_equal NArray.float(0), NArray.float(1).tile(0,0)
    assert_equal NArray.float(0), NArray.float(2,3).tile(0)
    assert_equal NArray.float(0), NArray.float(3,4,2).tile(1,2,0)
    assert_equal NArray.float(0), NArray.float(3,4,2).tile(1,0,2)

    # Degenerate case: tile with no args returns copy of original.
    assert_equal NArray.float(1).fill!(1), NArray.float(1).fill!(1).tile
    assert_equal NArray.float(1,2).indgen!, NArray.float(1,2).indgen!.tile

    # Tile a scalar.
    assert_equal NArray[1.0, 1.0],
                 NArray.float(1).fill!(1).tile(2) # row vector
    assert_equal NArray[[1.0],
                        [1.0]],
                 NArray.float(1).fill!(1).tile(1,2) # column vector
    assert_equal NArray[[[1.0]],
                        [[1.0]]],
                 NArray.float(1).fill!(1).tile(1,1,2) # add a dimension
    assert_equal NArray[[1.0, 1.0, 1.0],
                        [1.0, 1.0, 1.0]],
                 NArray.float(1).fill!(1).tile(3,2) # matrix

    # Tile a vector.
    v = NArray.float(2).indgen!
    assert_equal NArray[0.0, 1.0, 0.0, 1.0], v.tile(2)
    assert_equal NArray[0.0, 1.0, 0.0, 1.0, 0.0, 1.0], v.tile(3)
    assert_equal NArray[[0.0, 1.0],
                        [0.0, 1.0]], v.tile(1,2)
    assert_equal NArray[[0.0, 1.0],
                        [0.0, 1.0],
                        [0.0, 1.0]], v.tile(1,3)

    # Tile a matrix.
    m = NArray.float(2,3).indgen!
    assert_equal NArray[[0.0, 1.0],
                        [2.0, 3.0],
                        [4.0, 5.0]], m.tile
    assert_equal NArray[[0.0, 1.0, 0.0, 1.0],
                        [2.0, 3.0, 2.0, 3.0],
                        [4.0, 5.0, 4.0, 5.0]], m.tile(2)
    assert_equal m.tile(2), m.tile(2,1)
    assert_equal NArray[[0.0, 1.0],
                        [2.0, 3.0],
                        [4.0, 5.0],
                        [0.0, 1.0],
                        [2.0, 3.0],
                        [4.0, 5.0]], m.tile(1,2)
    assert_equal NArray[[[0.0, 1.0],
                         [2.0, 3.0],
                         [4.0, 5.0]],
                        [[0.0, 1.0],
                         [2.0, 3.0],
                         [4.0, 5.0]]], m.tile(1,1,2)

    # Tile another matrix.
    m = NArray.float(3,2).indgen!
    assert_equal NArray[[0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0]], m.tile
    assert_equal NArray[[0.0, 1.0, 2.0, 0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0, 3.0, 4.0, 5.0]], m.tile(2)
    assert_equal NArray[[0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0],
                        [0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0],
                        [0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0]], m.tile(1,3)
    assert_equal NArray[[0.0, 1.0, 2.0, 0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0, 3.0, 4.0, 5.0],
                        [0.0, 1.0, 2.0, 0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0, 3.0, 4.0, 5.0],
                        [0.0, 1.0, 2.0, 0.0, 1.0, 2.0],
                        [3.0, 4.0, 5.0, 3.0, 4.0, 5.0]], m.tile(2,3)
  end
end

