spec = Gem::Specification.new do |s|
  s.name = "subset_sum"
  s.version = "1.0.0"
  s.author = "Jeremy Evans"
  s.email = "code@jeremyevans.net"
  s.homepage = "http://subset-sum.rubyforge.org/"
  s.platform = Gem::Platform::RUBY
  s.summary = "Simple Subset Sum Solver with C and Pure Ruby Versions"
  s.files = %w"LICENSE extconf.rb subset_sum.c subset_sum.rb spec/subset_sum_spec.rb"
  s.require_paths = ["."]
  s.extensions << 'extconf.rb'
  s.test_files = %w"spec/subset_sum_spec.rb"
  s.has_rdoc = true
  s.rubyforge_project = 'subset-sum'
end
