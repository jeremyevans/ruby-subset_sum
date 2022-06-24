if command = ENV.delete('COVERAGE')
  require 'simplecov'

  SimpleCov.start do
    enable_coverage :branch
    command_name command
    add_filter "/spec/"
    add_group('Missing'){|src| src.covered_percent < 100}
    add_group('Covered'){|src| src.covered_percent == 100}
  end
end

require 'subset_sum'
ENV['MT_NO_PLUGINS'] = '1' # Work around stupid autoloading of plugins
require 'minitest/global_expectations/autorun'

describe "SubsetSum.subset_sum" do
  it "should raise TypeError unless the first argument is an array" do
    proc{SubsetSum.subset_sum(1, 1)}.must_raise(TypeError)
    proc{SubsetSum.subset_sum(nil, 1)}.must_raise(TypeError)
  end
  
  it "should raise TypeError unless the second argument is an Integer" do
    proc{SubsetSum.subset_sum([1,2,3], [1,2,3])}.must_raise(TypeError)
    proc{SubsetSum.subset_sum([1,2,3], nil)}.must_raise(TypeError)
  end
  
  it "should return nil if no subset sums to the given amount" do
    SubsetSum.subset_sum([1,2,3], -1).must_be_nil
    SubsetSum.subset_sum([1,2,3], 7).must_be_nil
  end
  
  it "should handle case where subset sum is computed within Timeout" do
    SubsetSum.subset_sum([1,2,3], -1, 1).must_be_nil
    SubsetSum.subset_sum([1,2,3], 7, 1).must_be_nil
    SubsetSum.subset_sum([1,2,3], 0, 1).must_equal []
    SubsetSum.subset_sum([1,2,3], 1, 1).must_equal [1]
    SubsetSum.subset_sum([1,2,3], 2, 1).must_equal [2]
    [[1,2], [3]].must_include(SubsetSum.subset_sum([1,2,3], 3, 1))
    SubsetSum.subset_sum([1,2,3], 4, 1).must_equal [1,3]
    SubsetSum.subset_sum([1,2,3], 5, 1).must_equal [2,3]
  end
  
  it "should return a subset of the given array summing to the given amount" do
    SubsetSum.subset_sum([1,2,3], 0).must_equal []
    SubsetSum.subset_sum([1,2,3], 1).must_equal [1]
    SubsetSum.subset_sum([1,2,3], 2).must_equal [2]
    [[1,2], [3]].must_include(SubsetSum.subset_sum([1,2,3], 3))
    SubsetSum.subset_sum([1,2,3], 4).must_equal [1,3]
    SubsetSum.subset_sum([1,2,3], 5).must_equal [2,3]
    SubsetSum.subset_sum([1,2,3], 6).must_equal [1,2,3]
    
    SubsetSum.subset_sum([-1,-2,-3], 0).must_equal []
    SubsetSum.subset_sum([-1,-2,-3], -1).must_equal [-1]
    SubsetSum.subset_sum([-1,-2,-3], -2).must_equal [-2]
    [[-1,-2], [-3]].must_include(SubsetSum.subset_sum([-1,-2,-3], -3))
    SubsetSum.subset_sum([-1,-2,-3], -4).must_equal [-1,-3]
    SubsetSum.subset_sum([-1,-2,-3], -5).must_equal [-2,-3]
    SubsetSum.subset_sum([-1,-2,-3], -6).must_equal [-1,-2,-3]
    
    a = [355104, 476077, 476303, 224658, -17532, -183480, -286788, 238271, 231845, -227454, 226199, 105438, 316870, 353652, 173563, 244958, 367896, 105046, 495797, 447209, 397810, -394348, 242527, 17532, -57224, -38084, 82375, 445376, -297793, 368660, -65413, 96325, -472195, -23826, -113982, -355574, 331821]
    b = [-17532, 226199, 105438, 353652, 173563, 244958, 397810, 242527, 17532, -38084, 82375, 445376, 368660, -65413, -23826, 331821]
    s = b.inject(0){|x,y|y+x}
    SubsetSum.subset_sum(a, s).inject(0){|x,y|y+x}.must_equal s
  end
  
  it "should raise a SubsetSum::TimeoutError if the Timeout expires" do
    a = [333471, -81424, 168803, -14874, 269559, 276470, -112006, -73874, -122941, 137631, -28130, -158579, -422171, 27107, 320112, -36290, -285551, 8352, 148555, -191195, -378065, -98153, 446370, 364645, -410213, -18978, 473135, -86692, 220362, -131996, -350007, -151015, -406022, -357901, -180250, 234239, -342453, -355331, -288241, 14474, 389351, 98297, -241761, 96174, 21747, 231199, -473800, -172155, 65848, -36045, -449758, 255701, 366189, -181547, -334067, 61902, -369357, -444588, 196073, 173373, 348303, 445476, 233108, 146771, -124962, 426707, -443489, -147630, -40380, 462470, -460874, -488721, -316681, 58097, 374972, -35163, 101042, -143586, 176358, -443511, 262241, 79581, -251333, 432832, 216996, 290187, -216371, 412922, -250978, 282206, -379257, 240712, 245726, -478854, 185713, 187485, -213379, -185167, -475251, -127179, -158661]
    b = [333471, -14874, 276470, 137631, -28130, -158579, 27107, -36290, 148555, -191195, -378065, 473135, -86692, -350007, -151015, -406022, 234239, -355331, 21747, 231199, -172155, -36045, -449758, 366189, -334067, -369357, -444588, 348303, 445476, -124962, 426707, 462470, -460874, 58097, 374972, -35163, 101042, 79581, -216371, 240712, -213379, -185167]
    s = b.inject(0){|x,y|y+x}
    proc{SubsetSum.subset_sum(a, s, 1).inject(0){|x,y|y+x}}.must_raise(SubsetSum::TimeoutError)
  end
  
  it "should not use the C version if the integers are Bignums" do
    if SubsetSum.respond_to?(:_subset_sum, true)
      SubsetSum.stub :_subset_sum, proc{raise} do
        SubsetSum.subset_sum([2**100, 1, 2,], 1+2**100).must_equal [2**100, 1]
      end
    else
        SubsetSum.subset_sum([2**100, 1, 2,], 1+2**100).must_equal [2**100, 1]
    end
  end
end

