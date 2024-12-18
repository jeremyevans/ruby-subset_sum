require 'rake/clean'
require "rdoc/task"

CLEAN.include %w"rdoc Makefile subset_sum.o subset_sum.so subset_sum-*.gem"
ENV['RUBYLIB'] = ".#{File::PATH_SEPARATOR}#{ENV['RUBYLIB']}"

task :default => [:spec]
task :spec => [:build] do
  sh "#{FileUtils::RUBY} #{"-w" if RUBY_VERSION >= '3'} #{'-W:strict_unused_block' if RUBY_VERSION >= '3.4'} spec/subset_sum_spec.rb"
  File.delete('subset_sum.so')
  sh "#{FileUtils::RUBY} #{"-w" if RUBY_VERSION >= '3'} #{'-W:strict_unused_block' if RUBY_VERSION >= '3.4'} spec/subset_sum_spec.rb"
end

task :spec_cov => [:build] do
  ENV['COVERAGE'] = 'c'
  sh "#{FileUtils::RUBY} spec/subset_sum_spec.rb"
  File.delete('subset_sum.so')
  ENV['COVERAGE'] = 'ruby'
  sh "#{FileUtils::RUBY} spec/subset_sum_spec.rb"
end

task :build =>[:clean] do
  sh %{#{FileUtils::RUBY} extconf.rb}
  sh %{make}
end

RDoc::Task.new do |rdoc|
  rdoc.rdoc_dir = "rdoc"
  rdoc.options += ["--quiet", "--line-numbers", "--inline-source", '--title', 'subset_sum: Simple Subset Sum Solver with C and pure ruby versions', '--main', 'subset_sum.rb']

  begin
    gem 'hanna'
    rdoc.options += ['-f', 'hanna']
  rescue Gem::LoadError
  end

  rdoc.rdoc_files.add %w"README CHANGELOG LICENSE lib/**/*.rb"
end

desc "Package subset_sum"
task :package do
  sh %{gem build subset_sum.gemspec}
end

task :speedup => [:build] do
  puts "C Version"
  sh %{#{FileUtils::RUBY} spec/c_speedup.rb}
  File.delete('subset_sum.so')
  puts "Ruby Version"
  sh %{#{FileUtils::RUBY} spec/c_speedup.rb}
end
