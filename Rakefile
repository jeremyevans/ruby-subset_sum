require 'rake/clean'

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

desc "Generate rdoc"
task :rdoc do
  rdoc_dir = "rdoc"
  rdoc_opts = ["--line-numbers", "--inline-source", '--title', 'subset_sum: Simple Subset Sum Solver with C and pure ruby versions']

  begin
    gem 'hanna'
    rdoc_opts.concat(['-f', 'hanna'])
  rescue Gem::LoadError
  end

  rdoc_opts.concat(['--main', 'subset_sum.rb', "-o", rdoc_dir] +
    %w"README CHANGELOG LICENSE subset_sum.rb"
  )

  FileUtils.rm_rf(rdoc_dir)

  require "rdoc"
  RDoc::RDoc.new.document(rdoc_opts)
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
