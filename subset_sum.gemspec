spec = Gem::Specification.new do |s|
  s.name = "subset_sum"
  s.version = "1.0.2"
  s.author = "Jeremy Evans"
  s.email = "code@jeremyevans.net"
  s.homepage = "http://ruby-subsetsum.jeremyevans.net/"
  s.platform = Gem::Platform::RUBY
  s.summary = "Simple Subset Sum Solver with C and Pure Ruby Versions"
  s.files = %w"LICENSE CHANGELOG extconf.rb subset_sum.c subset_sum.rb"
  s.require_paths = ["."]
  s.extensions << 'extconf.rb'
  s.required_ruby_version = ">= 1.9.2"
  s.add_development_dependency "minitest-global_expectations"
end
