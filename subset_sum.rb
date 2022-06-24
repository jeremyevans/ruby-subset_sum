# ruby-subset_sum implements a subset sum solver for ruby.
#
# Homepage: http://ruby-subsetsum.jeremyevans.net
# Source: https://github.com/jeremyevans/ruby-subset_sum

# This module provides both a C and pure ruby simple subset sum problem solver.
# The subset sum problem is, given a set of numbers, can the sum of any subset
# of those numbers equal a given number.  This problem is NP-complete.
# 
# Both the C and pure ruby versions implement a fairly simple
# meet-in-the-middle algorithm.  The C version uses an AVL tree to store the
# data, while the pure ruby version uses a ruby hash. For the C version to be
# used, the sum of the positive numbers and the sum of the negative numbers in
# the set, as well as the wanted number, must all be Fixnums.  Additionally,
# max_seconds should be nil or a Fixnum.
module SubsetSum
  extend self

  # Exception raised when timeout expires
  class TimeoutError < StandardError
  end
  
  # Return first subset of values that sum to want, using the meet in the
  # middle algorithm (O(n * 2^(n/2)).
  def subset_sum(values, want, max_seconds=nil)
    raise(TypeError, "values must be an array of Integers") unless values.is_a?(Array)
    raise(TypeError, "want must be an Integer") unless want.is_a?(Integer)
    
    # Optimization by removing 0 values and doing some simple checks
    values = values.reject{|x| x == 0}
    values.each{|value| return [value] if value == want}
    return values if sum(values) == want
    pos, neg = values.partition{|x| x > 0}
    sp, sn = sum(pos), sum(neg)
    return pos if sp == want
    return neg if sn == want
    
    # Use the C version if it exists and all values will be inside the machine
    # limits
    if respond_to?(:_subset_sum, true) && _subset_sum_supported?(want, sp, sn, max_seconds.to_i)
      return _subset_sum(values, want, max_seconds.to_i)
    end

    # The pure ruby version 
    sums = {}
    start_time = Time.now if max_seconds
    l = values.length/2
    subsets(values[0...l]) do |subset|
      _check_timeout(start_time, max_seconds)
      sums[sum(subset)] = subset
    end
    subsets(values[l..-1]) do |subset|
      _check_timeout(start_time, max_seconds)
      if subset2 = sums[want - sum(subset)]
        return subset2 + subset
      end
    end
    nil
  end

  private

  # Check time elapsed since start_time does not exceed max_seconds
  def _check_timeout(start_time, max_seconds)
    raise(TimeoutError, "timeout expired") if max_seconds and Time.now - start_time > max_seconds
  end

  # Yield all subsets of the array to the block.
  def subsets(array, skip = 0, &block)
    yield(array)
    (array.length-1).downto(skip){|i| subsets(array[0...i] + array[i+1..-1], i, &block)}
  end
  
  # Return the sum of the values.
  def sum(values)
    values.inject(0){|x,y| x+=y}
  end
end

begin
  require_relative 'subset_sum.so'
rescue LoadError
end
